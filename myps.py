#!/usr/bin/python
# -*- coding: utf-8 -*-
import glob

print "%5s %-15s %-12s %s" % ("PID", "Name", "State", "Command line")
print "-" * 60

# For each entry in the proc directory ...
for name in glob.glob("/proc/*"):

    # Check that the entry only consists of numbers.
    pid = name.replace("/proc/", "")
    if not pid.isdigit():
        continue
    pid = int(pid)

    # Load the command line from the cmdline. Subsequent arguments
    # are separated by binary zero.
    with open ("/proc/%i/cmdline" % pid) as f:
        cmdline = f.read().replace("\0", " ")

    # Load a handful of information from the status file.
    info = {}
    for line in open ("/proc/%i/status" % pid):
        data = line.strip().split(":\t")
        if len(data) == 2:
            info[data[0]] = data[1].strip()

    # List information about the process.
    print "%5u %-15s %-12s %s" % (pid, info["Name"], info["State"], cmdline)