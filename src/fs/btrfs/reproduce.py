#!/usr/bin/python2
import os, sys, subprocess

d = sys.argv[1]
o = sys.argv[2]
l = os.listdir(d)
t = 0

for fn in l:
  print str(t)
  if t >= 0:
    p = subprocess.check_output(['./btrfs_standalone', 'repro', 'btrfs.img', os.path.join(o, str(t) + '.img'), os.path.join(d, fn)])
    # p = subprocess.check_output(['./btrfs_standalone', 'repro', 'btrfs.img', os.path.join(o, fn + '.img'), os.path.join(d, fn)])
  
  t += 1

  # if t > 200:
  # break
  
  



