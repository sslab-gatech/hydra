#!/usr/bin/python3

import os
import sys

if len(sys.argv) != 2:
    print(f"[-] Usage: ./{os.path.basename(__file__)} path/to/testcase")
    print(f"For example, ./{os.path.basename(__file__)} /tmp/mosbench/tmpfs-separate/10/log/fileGd0Cbq-297139")
    sys.exit(-1)

testcase_full = sys.argv[1]
testcase_name = os.path.basename(sys.argv[1])

utils = os.path.dirname(os.path.realpath(__file__))
show = os.path.join(utils, "../", "combined", "program_show")

os.system(f"{show} {testcase_full}-prog > {os.getcwd()}/{testcase_name}.tmp.c")

with open(f"{testcase_name}.tmp.c", "r") as f:
    lines = f.readlines()

f = open(f"{testcase_name}.c", "w")
for line in lines:
    if line.strip().startswith("{"):
        f.write(line)
        f.write('\n\tchdir("test"); // mount point\n\n')

    elif line.strip().startswith("return"):
        f.write('\n\tsystem("echo b > /proc/sysrq-trigger"); // force restart\n')
        f.write("\treturn 0;\n}")
        break

    else:
        f.write(line)

f.close()

os.remove(f"{os.getcwd()}/{testcase_name}.tmp.c")

print(f"[+] Done. Your test case program is {os.getcwd()}/{testcase_name}.c")

