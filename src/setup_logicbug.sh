#!/bin/sh

wget https://gts3.org/~seulbae/samples.tar.gz
tar -xvzf samples.tar.gz
rm samples.tar.gz

mkdir in-lb out-lb seed-lb
./combined/create_corpus istat/btrfs-00.istat seed-lb
rm seed-lb/open_read9

cd lkl; patch -p1 < ../patch/fs-specific-logic-bug-checkers.patch
