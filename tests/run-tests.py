#!/usr/bin/python

#######################
# Kram Test Framework #
#   Seed Unit Tests   #
#######################

# So! Ideas...
# We obviously need not just 'pass' tests, but also 'fail' tests, and to make
# sure that they throw the correct exceptions. Something a bit more flexible
# than a Bash script that just checks to make sure all the .jses exit properly.

# I'm thinking of a comment directly after the shebang in the tests that gives
# expected output (on one line) and expected exit value... scripts get run
# by Python, which checks the comment and the exit value, runs the tests,
# and records run information (rev #, uname, test times, etc.) in an
# easily-emailed format (and append to a log file, for safekeeping).

# Test script could also have an option to perform timing/memory-use tests, 
# automatically running numerous times and averaging, etc... (find a way around
# launching overhead... reimplement main.c here?), and recording to a running
# log so we can keep track of performance/mem-use regressions, etc...

# TODO: test return value

import os
import re
import sys

passed = []
failed = []

for f in os.listdir("."):
	if f.endswith(".js"):
		rfile = open(f, "r")
		test_code = rfile.readlines()
		test_in = test_code[2].replace("// STDIN:","").rstrip().replace("\\n","\n");
		test_out = "^" + test_code[3].replace("// STDOUT:","").rstrip().replace("\\n","\n") + "$";
		test_err = "^" + test_code[4].replace("// STDERR:","").rstrip().replace("\\n","\n") + "$";
		
		(n,out,err) = os.popen3("./" + f)
		
		if(test_in != ""):
			n.write(test_in + "\004")
			n.close()
		
		run_out = "".join(out.readlines()).rstrip()
		run_err = "".join(err.readlines()).rstrip()
	
		if not re.match(test_out,run_out):
			failed.append([f,test_out,run_out,0,run_err])
			sys.stdout.write("x")
		elif not re.match(test_err,run_err):
			failed.append([f,test_err,run_err,1])
			sys.stdout.write("x")
		else:
			passed.append([f,test_out,run_out])
			sys.stdout.write(".")
		sys.stdout.flush()
print

revnof = os.popen("svn info | grep \"Revision\"")
revno = "".join(revnof.readlines()).rstrip()

print "libseed test run (%s):" % revno
print "%d tests passed; %d tests failed.\n" % (len(passed), len(failed))
for fail in failed:
	print "-------------FAILED TEST---------------"
	print "Name: %s" % fail[0]
	if fail[3]:
		print "  Expected Error:\t" + fail[1]
		print "  Actual Error:\t" + fail[2]
	else:
		print "  Expected Output:\t" + fail[1]
		print "  Actual Output:\t" + fail[2]
		print "  STDERR:\t\t" + fail[4]
if len(failed):
	print "---------------------------------------"	
