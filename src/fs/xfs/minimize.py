# SPDX-License-Identifier: MIT

import subprocess, sys

image_path = sys.argv[1]
fuzz_path = sys.argv[2]
meta_path = 'meta.img'
out_image = 'out.img'
fuzz_image = 'fuzz.img'

# get meta image
subprocess.check_call(['./xfs_standalone', 'test', image_path, 'out.img', meta_path])

def flush():
  f = open(fuzz_image, 'wb')
  f.write(out_buf)
  f.close()

def reproduce():
  subprocess.check_call(['./xfs_standalone', 'repro', image_path, out_image, fuzz_image])

def test():
  try:
    p = subprocess.check_call(['./xfs-fsfuzz', '-t', 'xfs', '-i', out_image])
  except subprocess.CalledProcessError as e:
    if e.returncode == -6 or e.returncode == -11:
      return True
    else:
      return False

out_buf = ''
final_buf = ''
fuzz_buf = open(fuzz_path, 'rb').read()
meta_buf = open(meta_path, 'rb').read()
for i in xrange(len(fuzz_buf)):
  if meta_buf[i] != fuzz_buf[i]:
    out_buf = final_buf + meta_buf[i] + fuzz_buf[i+1:]
    flush()
    reproduce()
    if test():
      print i
      final_buf += meta_buf[i]
    else:
      final_buf += fuzz_buf[i]
  else:
    final_buf += fuzz_buf[i]

f = open('final_meta.img', 'wb')
f.write(final_buf)
f.close()
subprocess.check_call(['./xfs_standalone', 'repro', image_path, 'final.img', 'final_meta.img'])
