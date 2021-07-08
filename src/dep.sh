#!/bin/bash

if [[ $(lsb_release -rs) == "16.04" ]]; then
  echo "install dependencies for Ubuntu 16.04"
  sudo apt-get -y install cmake libboost-dev libarchive-dev zlib1g-dev \
    flex bison bc libunistring0 libunistring-dev \
    e2fslibs-dev f2fs-tools \
    python3-zmq python-zmq python3-aiozmq python3-zmq-dbg libzmq3-dev libzmq1 \

elif [[ $(lsb_release -rs) == "18.04" ]]; then
  echo "install dependencies for Ubuntu 19.04"
  sudo apt-get -y install cmake libboost-dev libarchive-dev zlib1g-dev \
    flex bison bc libunistring2 libunistring-dev \
    e2fslibs-dev f2fs-tools \
    python3-zmq python-zmq python3-aiozmq python3-zmq-dbg libzmq3-dev libzmq5 \

elif [[ $(lsb_release -rs) == "20.04" ]]; then
  echo "install dependencies for Ubuntu 20.04"
  sudo apt-get -y install cmake libboost-dev libarchive-dev zlib1g-dev \
    flex bison bc libunistring2 libunistring-dev \
    e2fslibs-dev f2fs-tools \
    python3-zmq python3-zmq-dbg libzmq3-dev libzmq5 \

  pip3 install aiozmq

else
  echo "Ubuntu 16.04 / 18.04 / 20.04 is required"

fi
