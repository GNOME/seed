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

for root, dirs, files in os.walk(os.path.join(mcwd,"javascript")):
	for filename in files:
		f = os.path.join(root, filename)
		if f.endswith(".js") and not f.endswith("_.js") and not os.path.basename(f).startswith("."):
			attempts = 0
			while attempts < 10:
				try:
					rfile = open(f, "r")
					test_code = rfile.readlines()
					test_retval = int(test_code[1].replace("// Returns:","").rstrip().replace("\\n","\n"));
					test_in = test_code[2].replace("// STDIN:","").rstrip().replace("\\n","\n");
					test_out = "^" + test_code[3].replace("// STDOUT:","").rstrip().replace("\\n","\n") + "$";
					test_err = "^" + test_code[4].replace("// STDERR:","").rstrip().replace("\\n","\n") + "$";

					p = subprocess.Popen(f, shell=True,
								 stdin=subprocess.PIPE, stdout=subprocess.PIPE,
								 stderr=subprocess.PIPE, close_fds=True,
								 cwd=os.path.join(mcwd,"javascript"))
					(out,err)=(p.stdout, p.stderr)

					(run_out,run_err)=p.communicate(test_in + "\004")
					run_out = run_out.rstrip()
					run_err = run_err.rstrip()
					run_err = re.sub("Xlib:  extension \".*\" missing on display \".*\".\n?","",run_err)

					out.close()
					err.close()

					if not re.match(test_out,run_out):
						failed.append([filename,test_out,run_out,0,run_err])
						sys.stdout.write("x")
					elif not re.match(test_err,run_err):
						failed.append([filename,test_err,run_err,1])
						sys.stdout.write("x")
					elif p.returncode != test_retval:
						failed.append([filename,test_retval,p.returncode,2]);
						sys.stdout.write("x")
					else:
						passed.append([filename])
						sys.stdout.write(".")
						sys.stdout.flush()
					break
				except:
					attempts += 1
			if attempts == 10:
				print "WARNING: Strange error in " + f + "\n\n"

p = subprocess.Popen(os.path.join(mcwd, "c", "test"), shell=True,
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
