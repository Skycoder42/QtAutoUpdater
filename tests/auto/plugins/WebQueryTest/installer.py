# This Python file uses the following encoding: utf-8

import sys
import urllib.parse
import urllib.request

def notifyDidInstall(baseUrl, success, version):
	query = {
		"success": success,
		"version": version
	}
	request = urllib.request.urlopen(baseUrl, urllib.parse.urlencode(query).encode("utf-8"))
	request.read()

if __name__ == "__main__":
	print("Running install.py with arguments:", *sys.argv[1:4], file=sys.stderr)
	notifyDidInstall(*sys.argv[1:4])
