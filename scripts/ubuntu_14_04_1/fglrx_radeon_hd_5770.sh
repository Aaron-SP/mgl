#!/bin/bash

# Update repo package lists
sudo apt-get update

# Install proprietary AMD drivers for HD Radeon 5770
sudo apt-get install fglrx-updates xvba-va-driver libva-glx1 libva-egl1 vainfo
