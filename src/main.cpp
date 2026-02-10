/**
 * Copyright (C) 2025, Bruce MacKinnon KC1FSZ
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <execinfo.h>
#include <signal.h>
#include <pthread.h>
#include <sys/syscall.h> 

#include <iostream>
#include <cmath> 
#include <queue>
#include <thread>

// 3rd party
#include <curl/curl.h>

// KC1FSZ
#include "kc1fsz-tools/Log.h"
#include "kc1fsz-tools/MTLog2.h"
#include "kc1fsz-tools/linux/StdClock.h"
#include "kc1fsz-tools/fixedqueue.h"
#include "kc1fsz-tools/threadsafequeue.h"
#include "kc1fsz-tools/threadsafequeue2.h"
#include "kc1fsz-tools/linux/MTLog.h"
#include "kc1fsz-tools/copyableatomic.h"

// amp-core
#include "NullLog.h"
#include "LineIAX2.h"
#include "EventLoop.h"
#include "Bridge.h"
#include "MultiRouter.h"
#include "ThreadUtil.h"
#include "MultiRouter.h"
#include "Poker.h"
#include "TTSService.h"
#include "TimerTask.h"

// asl-hub
#include "service-thread.h"
#include "NumberAuthorizerStd.h"

using namespace std;
using namespace kc1fsz;

static const char* VERSION = "20260210.0";
static const char* PUBLIC_USER = "radio";

static void sigHandler(int sig);

static const unsigned callCount = 256;
static amp::BridgeCall bridgeCallSpace[callCount];
static LineIAX2::Call iax2CallSpace[callCount];

int main(int argc, const char** argv) {

    amp::setThreadName("Hub");

    signal(SIGSEGV, sigHandler);
    MTLog2 log;

    log.info("KC1FSZ ASL Hub");
    log.info("Powered by the Ampersand ASL Project https://github.com/Ampersand-ASL");
    log.info("Version %s", VERSION);
    log.info("Call capacity: %u", callCount);

    StdClock clock;
    NullLog traceLog;

    CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
    if (res) {
        log.error("curl_global_init() failed");
        return -1;
    }

    // This is a shared string that is used to pass the poke address from
    // the service thread to the main thread.
    copyableatomic<std::string> pokeAddr;

    // Get the service thread running. This handles registration,
    // status, and the monitor.
    std::thread serviceThread(service_thread, &log, VERSION, &pokeAddr);

    // Setup the message router
    threadsafequeue2<MessageCarrier> respQueue;
    MultiRouter router(respQueue);

    // Setup a background thread to do TTS. 
    // There are queues in/out to handle requests/response.
    // This is hard-coded as line #7.
    threadsafequeue2<MessageCarrier> ttsReqQueue;
    QueueConsumer ttsConsumer7(ttsReqQueue);
    router.addRoute(&ttsConsumer7, 7);
    std::atomic<bool> ttsRun(true);
    std::thread ttsThread(amp::ttsLoop, &log, &ttsReqQueue, &respQueue, &ttsRun);

    // Setup the conference bridge
    amp::Bridge bridge10(log, traceLog, clock, router, amp::BridgeCall::Mode::NORMAL, 
        10, 7, 0, 0, 1, bridgeCallSpace, callCount);
    router.addRoute(&bridge10, 10);
    bridge10.setLocalNodeNumber(getenv("AMP_NODE0_NUMBER"));
    bridge10.setGreeting(getenv("AMP_NODE0_GREETING"));

    // Setup the IAX line
    amp::NumberAuthorizerStd destVal(getenv("AMP_NODE0_NUMBER"));
    amp::NumberAuthorizerStd sourceVal(getenv("AMP_IAX_ALLOWLIST"));
    // IMPORTANT: The directed POKE feature is turned on here!
    LineIAX2 iax2Channel1(log, traceLog, clock, 1, router, &destVal, &sourceVal, 0, 10,
        PUBLIC_USER, iax2CallSpace, callCount);
    router.addRoute(&iax2Channel1, 1);
    //iax2Channel0.setTrace(true);
    iax2Channel1.setPrivateKey(getenv("AMP_PRIVATE_KEY"));
    iax2Channel1.setDNSRoot(getenv("AMP_ASL_DNS_ROOT"));
    if (getenv("AMP_IAX_AUTHMODE")) {
        if (strcmp(getenv("AMP_IAX_AUTHMODE"), "OPEN") == 0) 
            iax2Channel1.setAuthMode(LineIAX2::AuthMode::OPEN);
        else if (strcmp(getenv("AMP_IAX_AUTHMODE"), "SOURCE_IP") == 0) 
            iax2Channel1.setAuthMode(LineIAX2::AuthMode::SOURCE_IP);
        else if (strcmp(getenv("AMP_IAX_AUTHMODE"), "CHALLENGE_ED25519") == 0) 
            iax2Channel1.setAuthMode(LineIAX2::AuthMode::CHALLENGE_ED25519);
    }

    // Determine the address family, defaulting to IPv4
    short addrFamily = getenv("AMP_IAX_PROTO") != 0 && 
        strcmp(getenv("AMP_IAX_PROTO"), "IPV6") == 0 ? AF_INET6 : AF_INET;
    // Open up the IAX2 network connection
    iax2Channel1.open(addrFamily, atoi(getenv("AMP_IAX_PORT")));
    iax2Channel1.setPokeEnabled(true);
    iax2Channel1.setDirectedPokeEnabled(true);
    iax2Channel1.setPokeNodeNumber(getenv("AMP_NODE0_NUMBER"));
    
    // Setup a timer that takes the poke address generated from the service
    // thread and puts it into the IAX line.
    TimerTask timer1(log, clock, 10, 
        [&log, &pokeAddr, &iax2Channel1]() {
            // Log stats
            //log.info("Log timing %lu", log.getSlowestUs());
            // Poke address
            std::string addr = pokeAddr.getCopy();
            if (!addr.empty())
                iax2Channel1.setPokeAddr(addr.c_str());
        }
    );

    // Main loop        
    Runnable2* tasks2[] = { &iax2Channel1, &bridge10, &router, &timer1 };
    EventLoop::run(log, clock, 0, 0, tasks2, std::size(tasks2), nullptr, false);

    return 0;
}

// A crash signal handler that displays stack information
static void sigHandler(int sig) {
    void *array[32];
    // get void*'s for all entries on the stack
    size_t size = backtrace(array, 32);
    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    // Now do the regular thing
    signal(sig, SIG_DFL); 
    raise(sig);
}
