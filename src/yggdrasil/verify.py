import subprocess
import sys
import time

files = [
    ('test_waldisk.py ', 'WAL Layer'),
    ('test_xv6inode.py', 'Inode layer'),
    ('test_dirspec.py', 'Directory layer'),
    ('test_bitmap.py', 'Bitmap disk refinement'),
    ('test_inodepack.py', 'Inode disk refinement'),
    ('test_partition.py', 'Multi disk partition refinement'),
]

n = time.time()

for i, pt in files:
    sys.stdout.write('Verifying %s.' % pt)
    sys.stdout.flush()
    outp = ""
    lastp = time.time()
    w = subprocess.Popen('python2 %s' % i, shell=True, stdin=subprocess.PIPE, stderr=subprocess.PIPE)
    np = 0
    pn = time.time()
    while True:
        out = w.stderr.read(1)
        outp += out
        if not out:
            t = time.time() - pn
            sys.stdout.write("%s%f seconds\n" % ('.' * (50 - np - len(pt) - len(str(int(t)))), t))
            w.wait()
            if w.returncode != 0:
                print
                print 'Failure.'
                print outp
                sys.exit(1)
            break
        if out == '.':
            if time.time() - lastp > 1:
                np += 1
                sys.stdout.write(out)
                sys.stdout.flush()
                lastp = time.time()


print
print 'Success. Verified Yxv6 in %fs' % (time.time() - n)
