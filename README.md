This repo builds the Ampersand Hub which is based on 
the [Ampersand Linking Project](https://github.com/Ampersand-ASL)
by [Bruce MacKinnon KC1FSZ](https://www.qrz.com/db/KC1FSZ).

# Building AMP Hub With Install

    # Make sure you have all the packages needed to build
    sudo apt install cmake build-essential git xxd libasound2-dev libcurl4-gnutls-dev Libusb-1.0-0-dev
    export AMP_HUB_VERSION=1.0
    export ARCH=$(uname -m)
    git clone https://github.com/Ampersand-ASL/amp-hub.git
    cd asl-hub
    git submodule update --init
    cmake -DCMAKE_INSTALL_PREFIX=/tmp/amp-hub_${AMP_HUB_VERSION}_${ARCH} -B build
    cmake --build build --target amp-hub
    cmake --install build --component amp-hub

# Making Package

    rm -rf /tmp/amp-hub_${AMP_HUB_VERSION}_${ARCH}.tar.gz
    cd /tmp
    tar -czf /tmp/amp-hub_${AMP_HUB_VERSION}_${ARCH}.tar.gz amp-hub_${AMP_HUB_VERSION}_${ARCH}

# Debian Package Notes (NOT WORKING YET)

Making the package for the asl-parrot:

    # Move the version number forward in src/main-parrot.cpp
    # Update the change log (new entries at top)
    sudo apt install debmake debhelper
    export AMP_HUB_VERSION=1.0
    cd amp-hub
    scripts/make-source-tar-hub.sh
    cd /tmp
    tar -xzmf amp-hub-$AMP_HUB_VERSION.tar.gz
    cd amp-hub-$AMP_HUB_VERSION
    debmake
    debuild
    # Move the package to the distribution area
    cp /tmp/amp-hub_$AMP_HUB_VERSION-1_arm64.deb (distribution area)

Looking at the contents:

    dpkg -c amp-hub_$AMP_HUB_VERSION-1_arm64.deb 

Installing from a .deb file:

    wget https://mackinnon.info/ampersand/releases/amp-hub_1.0-1_arm64.deb
    sudo apt install ./amp-hub_1.0-1_arm64.deb

_(There may be a "Notice" displayed during the install related to a permission issue. This
can safely be ignored.)_

Uninstall:

    sudo apt remove ./amp-hub_1.0-1_arm64.deb

Service Commands:

    sudo systemctl enable amp-hub
    sudo systemctl start amp-hub
    journalctl -u amp-hub -f

# Environment Variables Used At Runtime

    export AMP_NODE0_NUMBER=
    export AMP_NODE0_PASSWORD=
    # Authentication used for public nodes
    # OPEN=No authentication checks by default
    export AMP_IAX_AUTHMODE=OPEN
    # A comma-separated list of nodes that are allowed to connect
    export AMP_IAX_ALLOWLIST=1000,672731
    export AMP_IAX_PROTO=IPV4
    export AMP_IAX_PORT=4569
    export AMP_HTTP_PORT=8080
    # Not used for private nodes
    #export AMP_ASL_REG_URL=https://register.allstarlink.org
    # Not used for private nodes
    #export AMP_ASL_STAT_URL=http://stats.allstarlink.org/uhandler
    export AMP_ASL_DNS_BASE=nodes.allstarlink.org
    # Pointer to Piper TTS files (voice and the espeak runtime files)
    export AMP_PIPER_DIR=/usr/etc

# A Useful AWS Command

This command could be used to determine the private IP address of the 
diagnostic interface:

    aws ec2 describe-network-interfaces
