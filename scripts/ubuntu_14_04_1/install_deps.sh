#!/bin/bash

# Update repo package lists
sudo apt-get update

# Install dependencies
sudo apt-get install build-essential git libgl1-mesa-dev libglu1-mesa-dev xorg-dev libglew-dev libfreetype6-dev libvorbis-dev libalut-dev -y
