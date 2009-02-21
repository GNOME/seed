#!/usr/bin/python

################################
#   Seed Unit Test Framework   #
################################

import os
import re
import sys
import subprocess
import difflib

passed = []
failed = []

mcwd = os.getcwd()

for f in os.listdir("javascript"):
	if f.endswith(".js") and not f.endswith("_.js"):
		rfile = open(mcwd + "/javascript/"+f, "r")
		test_code = rfile.readlines()
		test_retval = int(test_code[1].replace("// Returns:","").rstrip().replace("\\n","\n"));
		test_in = test_code[2].replace("// STDIN:","").rstrip().replace("\\n","\n");
		test_out = "^" + test_code[3].replace("// STDOUT:","").rstrip().replace("\\n","\n") + "$";
		test_err = "^" + test_code[4].replace("// STDERR:","").rstrip().replace("\\n","\n") + "$";
		
		p = subprocess.Popen(mcwd + "/javascript/" + f, shell=True,
							 stdin=subprocess.PIPE, stdout=subprocess.PIPE,
							 stderr=subprocess.PIPE, close_fds=True,
							 cwd=mcwd+"/javascript/")
		(out,err)=(p.stdout, p.stderr)
		
		(run_out,run_err)=p.communicate(test_in + "\004")
		run_out = run_out.rstrip()
		run_err = run_err.rstrip()
		
		out.close()
		err.close()
	
		if not re.match(test_out,run_out):
			failed.append([f,test_out,run_out,0,run_err])
			sys.stdout.write("x")
		elif not re.match(test_err,run_err):
			failed.append([f,test_err,run_err,1])
			sys.stdout.write("x")
		elif p.returncode != test_retval:
			failed.append([f,test_retval,p.returncode,2]);
			sys.stdout.write("x")
		else:
			passed.append([f])
			sys.stdout.write(".")
		sys.stdout.flush()

p = subprocess.Popen(mcwd + "/c/test", shell=True,
					 stdin=subprocess.PIPE, stdout=subprocess.PIPE,
					 stderr=subprocess.PIPE, close_fds=True);
(c_out, c_err) = p.communicate()

for c_test in c_out.rstrip().split("\n"):
	c_status = c_test.split(": ");
	a_error = c_err.rstrip().replace("**\nERROR:","").replace("\nAborted","")
	if (len(c_status) < 2) or (c_status[1] != "OK"):
		failed.append(["c_test" + c_status[0], "OK", a_error,1])
		sys.stdout.write(".")
	else:
		passed.append([c_status[0]])
	sys.stdout.flush()

print

print "libseed test run:"
print "%d tests passed; %d tests failed.\n" % (len(passed), len(failed))
for fail in failed:
	print "-------------FAILED TEST---------------"
	print "Name: %s" % fail[0]
	if fail[3] == 1:
		for line in difflib.unified_diff(fail[1].replace("\\","").replace("^","").replace("$","").split("\n"),
										 fail[2].split("\n"),
										 fromfile="Expected Error",
										 tofile="Actual Error"):
			print line.rstrip()
	elif fail[3] == 0:
		for line in difflib.unified_diff(fail[1].replace("\\","").replace("^","").replace("$","").split("\n"),
										 fail[2].split("\n"),
										 fromfile="Expected Output",
										 tofile="Actual Output"):
			print line.rstrip()
		print "  Error Output:\t\t" + fail[4]
	elif fail[3] == 2:
		print "  Expected Retval:\t%d" % fail[1]
		print "  Actual Retval:\t%d" % fail[2]
if len(failed):
	print "---------------------------------------"	
