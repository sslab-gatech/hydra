#!/usr/bin/python2
import os, sys, subprocess

d = sys.argv[1]
o = sys.argv[2]
l = os.listdir(d)
t = 0

for fn in l:
  print str(t)
  p = subprocess.check_output(['./reiserfs_standalone', 'repro', 'reiserfs.img', os.path.join(o, str(t) + '.img'), os.path.join(d, fn)])
  t += 1



