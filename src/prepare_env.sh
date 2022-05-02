#!/bin/sh

tar -xvzf samples.tar.gz
chown -R $USER samples

mkdir seed
./combined/create_corpus_consistency istat/btrfs-10.istat seed
