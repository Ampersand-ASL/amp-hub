#!/bin/bash
# Create the install directories
mkdir -p /usr/bin
mkdir -p /usr/etc
mkdir -p /usr/lib
# Move the files
sudo cp -r bin /usr
sudo cp -r lib /usr
sudo cp -r etc /usr
# Steps necessary to run server under systemd
sudo useradd -r -s /sbin/nologin amp-hub
sudo cp /usr/etc/amp-hub.service /lib/systemd/system
sudo systemctl enable amp-hub

