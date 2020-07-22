#!/usr/bin/python
# -*- coding: utf-8 -*-
import os
import subprocess

# In case of failure while creating the process, an appropriate
# exception is thrown, unfortunately depending on the system.
# For example, if the file is not found, WindowsError will be thrown
# on Windows systems, but OSError on GNU/Linux systems.
if os.name == 'nt':
    exception_type = WindowsError
    executable = "C:\\Windows\\System32\\notepad.exe"
else:
    exception_type = OSError
    executable = "/usr/bin/vim"

try:
    p = subprocess.Popen(executable)
    print "Process started (pid: %u). Waiting for it to exit." % p.pid
    p.wait()
    print "The child process has exited."
except exception_type as e:
    print "Failed to sprawn process: ", e

# Alternatively, functions from the subprocess module that wait for the
# child process to terminate (meaning no "handle" is returned to the
# child process).
#
# subprocess.call(["C:\\Windows\\System32\\Notepad.exe"])
# subprocess.check_call(["C:\\Windows\\System32\\Notepad.exe"])
# subprocess.call_output(["C:\\Windows\\System32\\Notepad.exe"])
# subprocess.check_output(["C:\\Windows\\System32\\Notepad.exe"])