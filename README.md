This repo builds the Hub Parrot which is based on 
the [Ampersand Linking Project](https://github.com/Ampersand-ASL)
by [Bruce MacKinnon KC1FSZ](https://www.qrz.com/db/KC1FSZ).

# Building ASL Hub With Install

    # Make sure you have all the packages needed to build
    sudo apt install cmake build-essential git xxd libasound2-dev libcurl4-gnutls-dev Libusb-1.0-0-dev
    git clone https://github.com/Ampersand-ASL/asl-hub.git
    cd asl-hub
    git submodule update --init
    cmake -DCMAKE_INSTALL_PREFIX=/tmp -B build
    cmake --build build --target asl-hub
    cmake --install build --component asl-hub

# Debian Package Notes

Making the package for the asl-parrot:

    # Move the version number forward in src/main-parrot.cpp
    # Update the change log (new entries at top)
    sudo apt install debmake debhelper
    export ASL_HUB_VERSION=1.0
    cd asl-hub
    scripts/make-source-tar-hub.sh
    cd /tmp
    tar -xzmf asl-hub-$ASL_PARROT_VERSION.tar.gz
    cd asl-hub-$ASL_HUB_VERSION
    debmake
    debuild
    # Move the package to the distribution area
    cp /tmp/asl-hub_$ASL_HUB_VERSION-1_arm64.deb (distribution area)

Looking at the contents:

    dpkg -c asl-hub_$ASL_HUB_VERSION-1_arm64.deb 

Installing from a .deb file:

    wget https://mackinnon.info/ampersand/releases/asl-hub_1.0-1_arm64.deb
    sudo apt install ./asl-hub_1.0-1_arm64.deb

_(There may be a "Notice" displayed during the install related to a permission issue. This
can safely be ignored.)_

Uninstall:

    sudo apt remove ./asl-hub_1.0-1_arm64.deb

Service Commands:

    sudo systemctl enable asl-hub
    sudo systemctl start asl-hub
    journalctl -u asl-hub -f

# Environment Variables Used At Runtime

    export AMP_NODE0_NUMBER=
    export AMP_NODE0_PASSWORD=
    export AMP_IAX_PROTO=IPV4
    export AMP_IAX_PORT=4569
    export AMP_HTTP_PORT=8080
    export AMP_ASL_REG_URL=https://register.allstarlink.org
    export AMP_ASL_STAT_URL=http://stats.allstarlink.org/uhandler
    export AMP_ASL_DNS_BASE=nodes.allstarlink.org
    # Pointer to Piper TTS files (voice and the espeak runtime files)
    export AMP_PIPER_DIR=/usr/etc

# A Useful AWS Command

This command could be used to determine the private IP address of the 
diagnostic interface:

    aws ec2 describe-network-interfaces
