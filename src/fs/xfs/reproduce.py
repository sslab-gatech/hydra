#!/usr/bin/python2

# SPDX-License-Identifier: MIT

import os, sys, subprocess

d = sys.argv[1]
o = sys.argv[2]
l = os.listdir(d)
t = 0

for fn in l:
  print fn, str(t)
  p = subprocess.check_output(['./xfs_standalone', 'repro', 'xfs-1ag-crc.img', os.path.join(o, str(t) + '.img'), os.path.join(d, fn)])
  t += 1



