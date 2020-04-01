#!/bin/bash

# ubuntu 16.04
sudo apt-get -y install cmake libboost-dev libarchive-dev zlib1g-dev \
    flex bison bc libunistring0 libunistring-dev \
    e2fslibs-dev f2fs-tools \
    python3-zmq python-zmq python3-aiozmq python3-zmq-dbg libzmq3-dev libzmq1 \

#ubuntu 18.04
sudo apt-get -y install cmake libboost-dev libarchive-dev zlib1g-dev \
    flex bison bc libunistring2 libunistring-dev \
    e2fslibs-dev f2fs-tools \
    python3-zmq python-zmq python3-aiozmq python3-zmq-dbg libzmq3-dev libzmq5 \
