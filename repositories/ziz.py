#!/usr/bin/python
#CHANGE ONLY, IF YOU KNOW, WHAT YOU DO!
#OPKMANAGER WILL CRASH IF YOUR OUTPUT IS INVALID!
import subprocess
import argparse
import time
import calendar

class RegisterAction(argparse.Action):
	def __call__(self, parser, namespace, values, option_string=None):
		print "Ziz's Repository" # Name
		print "web" # Type (maybe web for web, or anything else for usb)
		print "http://ziz.gp2x.de/downloads/" #URL
		print "./repositories/ziz.py --update" #Call for updating the list

class UpdateAction(argparse.Action):
	def __call__(self, parser, namespace, values, option_string=None):
		print "[Puzzletube]" #Starts a new entry
		process = subprocess.Popen('wget -qO- http://ziz.gp2x.de/downloads/puzzletube/index.htm',stdout=subprocess.PIPE,shell=True)
		output = process.stdout.read().split('\n')
		for line in output:
			if line.startswith("<a href=puzzletube-gcw"):
				#searching the filename itself
				parts = line.split('<a href=')
				parts = parts[1].split(' ')
				print "filename: " + parts[0] #NEEDED!
			if line.startswith("Updated at the "):
				#Parsing the time
				parts = line.split('Updated at the ')
				t = time.strptime(parts[1],"%d.%m.%Y %H:%M:%S .")
				print "version: " + str(calendar.timegm(t)) #NEEDED!
		print "description: A puzzle game. On a tube!"#Description. Not needed.
		print "url_addition: puzzletube/"#Url addition. Not needed.
		print "" #line breaks make the like beatiful
		print "[Sparrow C4A Manager]"
		process = subprocess.Popen('wget -qO- http://ziz.gp2x.de/downloads/Sparrow-C4A-Manager/index.htm',stdout=subprocess.PIPE,shell=True)
		output = process.stdout.read().split('\n')
		for line in output:
			if line.startswith("<a href=Sparrow-C4A-Manager-gcw"):
				#searching the filename itself
				parts = line.split('<a href=')
				parts = parts[1].split(' ')
				print "filename: " + parts[0] #NEEDED!
			if line.startswith("Updated at the "):
				#Parsing the time
				parts = line.split('Updated at the ')
				t = time.strptime(parts[1],"%d.%m.%Y %H:%M:%S .")
				print "version: " + str(calendar.timegm(t)) #NEEDED!
		print "description: An alternative manager for Compo4All"
		print "url_addition: Sparrow-C4A-Manager/"
		print ""

def main():
	parser = argparse.ArgumentParser(description="Ziz's Repository script")
	parser.add_argument('--register', nargs=0, action=RegisterAction)
	parser.add_argument('--update', nargs=0, action=UpdateAction)
	args = parser.parse_args()

if __name__ == "__main__":
	main()
