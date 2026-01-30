This repo builds the Ampersand Hub which is based on 
the [Ampersand Linking Project](https://github.com/Ampersand-ASL)
by [Bruce MacKinnon KC1FSZ](https://www.qrz.com/db/KC1FSZ).

> [!Important]
>
> If you're looking for the install/user guide [it can be found here](docs/user.md).

# Building AMP Hub With Install

    # Make sure you have all the packages needed to build
    sudo apt -y install cmake build-essential git xxd libasound2-dev libcurl4-gnutls-dev Libusb-1.0-0-dev emacs-nox
    export AMP_HUB_VERSION=20260130
    export AMP_ARCH=$(uname -m)
    git clone https://github.com/Ampersand-ASL/amp-hub.git
    cd amp-hub
    git submodule update --init
    cmake -DCMAKE_INSTALL_PREFIX=/tmp/amp-hub-${AMP_HUB_VERSION}-${AMP_ARCH} -B build
    cmake --build build --target amp-hub
    cmake --install build --component amp-hub

# Making Package

    rm -rf /tmp/amp-hub-${AMP_HUB_VERSION}-${AMP_ARCH}.tar.gz
    cd /tmp
    chmod a+x amp-hub-${AMP_HUB_VERSION}-${AMP_ARCH}/install.sh
    chmod a+x amp-hub-${AMP_HUB_VERSION}-${AMP_ARCH}/upgrade.sh
    tar -czf /tmp/amp-hub-${AMP_HUB_VERSION}-${AMP_ARCH}.tar.gz amp-hub-${AMP_HUB_VERSION}-${AMP_ARCH}
    # Push the package to S3
    scp admin@amp-hub:/tmp/amp-hub-${AMP_HUB_VERSION}-${AMP_ARCH}.tar.gz .
    # etc ..

# Service Commands

    sudo systemctl enable amp-hub
    sudo systemctl start amp-hub
    journalctl -u amp-hub -f

# Environment Variables Used At Runtime

    export AMP_NODE0_NUMBER=672731
    export AMP_NODE0_PASSWORD=
    # Authentication used for public nodes
    # OPEN=No authentication checks by default
    export AMP_IAX_AUTHMODE=OPEN
    # A comma-separated list of nodes that are allowed to connect
    export AMP_IAX_ALLOWLIST=1000,672730
    export AMP_IAX_PROTO=IPV4
    export AMP_IAX_PORT=4568
    # (Not used for private nodes)
    export AMP_ASL_REG_URL=https://register.allstarlink.org
    export AMP_ASL_STAT_URL=http://stats.allstarlink.org/uhandler
    export AMP_ASL_DNS_BASE=nodes.allstarlink.org
    # Pointer to Piper TTS files (voice and the espeak runtime files)
    export AMP_PIPER_DIR=/usr/etc
    # Make sure the libpiper .sos are on the path
    export LD_LIBRARY_PATH=/usr/lib

