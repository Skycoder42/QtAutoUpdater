#!/usr/bin/python
# $1 The readme to be transformed
# $2 brief description
# $pwd: dest dir

import sys
import os.path

def readFirst(line, brief, out):
	if line[0:2] != "# ":
		raise ValueError("Expected first line to start with '# '")
	# skip the first line
	if brief is not None:
		out.write(line + "\n");
		out.write("@brief " + brief + "\n\n")
	out.write("[TOC]\n\n")

readCounter = 0
def readMore(line, label, offset, out):
	global readCounter
	if line[0:2] == "##":
		out.write(line[1:] + " {{#qtautoupdater_{}_label_{}}}\n".format(label, readCounter))
		readCounter += 1
	else:
		out.write(line + "\n")

#read args
readme = sys.argv[1]
brief = sys.argv[2] if len(sys.argv) > 2 else None
doxme = os.path.basename(readme)
label, _ = os.path.splitext(doxme)

inFile = open(readme, "r")
outFile = open(doxme, "w")

isFirst = True
for line in inFile:
	if isFirst:
		readFirst(line[:-1], brief, outFile)
		isFirst = False
	else:
		readMore(line[:-1], label, 1 if brief is None else 0, outFile)

inFile.close();
outFile.close();
