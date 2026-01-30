#!/bin/bash
echo "Stopping service"
sudo systemctl stop amp-hub
# Make backups
sudo cp /usrbin/amp-hub /usr/bin/amp-hub.backup
# Install the files
sudo cp bin/amp-hub /usr/bin
echo "Starting service"
sudo systemctl start amp-hub

