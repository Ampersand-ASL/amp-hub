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
#include <sched.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h> 

#include <execinfo.h>
#include <signal.h>
#include <pthread.h>

// Needed for DNS lookup
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <arpa/inet.h>

#include <iostream>
#include <cmath> 
#include <queue>

#include "kc1fsz-tools/Log.h"
#include "kc1fsz-tools/linux/StdClock.h"
#include "kc1fsz-tools/MicroDNS.h"
#include "kc1fsz-tools/NetUtils.h"

#include "RegisterTask.h"
#include "StatsTask.h"
#include "EventLoop.h"
#include "ThreadUtil.h"
#include "TimerTask.h"

#include "service-thread.h"

using namespace std;
using namespace kc1fsz;

static const char* POKE_HOST_NAME = "61057.nodes.allstarlink.org";
static int POKE_PORT = 4570;

void service_thread(Log* l, const char* version, 
    copyableatomic<std::string>* pokeAddr,
    threadsafequeue2<MessageCarrier>* reqQueue) {

    amp::setThreadName("SVC");

    Log& log = *l;
    log.info("Start service_thread");
    StdClock clock;

    // Sleep waiting to change real-time status
    sleep(10);
    
    // All of this stuff lowers the priority of the service thread
    amp::lowerThreadPriority();

    RegisterTask registerTask(log, clock);
    if (getenv("AMP_ASL_REG_URL") != 0) {
        registerTask.configure(getenv("AMP_ASL_REG_URL"), getenv("AMP_NODE0_NUMBER"), 
            getenv("AMP_NODE0_PASSWORD"), atoi(getenv("AMP_IAX_PORT")));
    }

    StatsTask statsTask(log, clock, version);
    if (getenv("AMP_ASL_STAT_URL") != 0) {
        statsTask.configure(getenv("AMP_ASL_STAT_URL"), getenv("AMP_NODE0_NUMBER"));
    }

    // This timer task checks for messages on the request queue 
    // and distributes them appropriately.
    TimerTask timer2(log, clock, 5, 
        [&log, reqQueue, &statsTask]() {
            MessageCarrier msg;
            if (reqQueue->try_pop(msg, 10)) {
                // The node list for stats reporting
                if (msg.isSignal(Message::SignalType::LINK_REPORT))
                    statsTask.setNodeList((const char*)msg.body());
            }
        }
    );

    // This is a timer that does a DNS resolution on the "poke node"
    // periodically and puts the result into the pokeAddr that is 
    // shared with the main thread.
    TimerTask timer1(log, clock, 120, 
        [&log, pokeAddr]() {
            // DNS lookup
            unsigned char answer[128];
            int answerLen;
            answerLen = res_query(POKE_HOST_NAME, 1, 1, answer, sizeof(answer));
            if (answerLen < 0) {
                log.error("Unable to resolve address for %s", POKE_HOST_NAME);
                return;
            }
            uint32_t addr;
            int rc2 = microdns::parseDNSAnswer_A(answer, answerLen, &addr);
            if (rc2 < 0) {
                log.error("Unable to resolve address for %s (2)", POKE_HOST_NAME);
                return;
            }
            char dottedAddr[32];
            formatIP4Address(addr, dottedAddr, sizeof(dottedAddr));
            char addrAndPort[64];
            snprintf(addrAndPort, 64, "%s:%d", dottedAddr, POKE_PORT);
            pokeAddr->set(string(addrAndPort));
        }
    );

    // Main loop        
    Runnable2* tasks2[] = { &registerTask, &timer1, &timer2, &statsTask };
    EventLoop::run(log, clock, 0, 0, tasks2, std::size(tasks2));
}
