#!/bin/bash
VERSION=$AMP_HUB_VERSION
ARCH=aarch64
rm -rf /tmp/amp-hub-$VERSION_$ARCH.tar.gz
cd /tmp
tar -czf /tmp/amp-hub-$VERSION_$ARCH.tar.gz amp-hub-$VERSION_$ARCH



