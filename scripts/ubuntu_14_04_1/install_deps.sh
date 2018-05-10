#!/bin/bash

# Update repo package lists
sudo apt-get update

# Install dependencies
sudo apt-get install build-essential git libvorbis-dev libfreetype6-dev xorg-dev libgl1-mesa-dev libglu1-mesa-dev libalut-dev -y
