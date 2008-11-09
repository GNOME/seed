#!/usr/bin/python

import os
import re
import sys
import stat

rfile = open(sys.argv[1], "r")
test_code = rfile.readlines()

if(test_code[1].find("// STDIN") >= 0):
    test_in = test_code[1].replace("// STDIN:","").rstrip().replace("\\n","\n")
else:
    test_in = ""

(n,out,err) = os.popen3("./" + sys.argv[1])
		
if(test_in != ""):
	n.write(test_in + "\004")
	n.close()

outf = open(sys.argv[1].replace("_.js",".js"),"w")

def sanitize(san):
    san = san.replace("\\","\\\\");
    san = san.replace("(","\(");
    san = san.replace(")","\)");
    san = san.replace("[","\[");
    san = san.replace("]","\]");
    san = san.replace("{","\{");
    san = san.replace("}","\}");
    san = san.replace(".","\.");
    san = san.replace("*","\*");
    san = san.replace("$","\$");
    san = san.replace("^","\^");
    san = san.replace("/","\/");
    san = san.replace("+","\+");
    san = san.replace("\n","\\n");
    return san

outf.write(test_code[0])
outf.write("// Returns: 0\n")
outf.write("// STDIN:" + test_in + "\n")
outf.write("// STDOUT:" + sanitize("".join(out.readlines()).rstrip()) + "\n")
outf.write("// STDERR:" + sanitize("".join(err.readlines()).rstrip()) + "\n")

if(test_in != ""):
	outf.write("".join(test_code[2:]))
else:
	outf.write("".join(test_code[1:]))

outf.close()

os.chmod(sys.argv[1].replace("_.js",".js"), stat.S_IREAD | stat.S_IEXEC | stat.S_IWRITE)
