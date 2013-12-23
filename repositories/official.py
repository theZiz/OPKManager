#!/usr/bin/python
#CHANGE ONLY, IF YOU KNOW, WHAT YOU DO!
#OPKMANAGER WILL CRASH IF YOUR OUTPUT IS INVALID!
import subprocess
import argparse
import time
import calendar
import string

class RegisterAction(argparse.Action):
	def __call__(self, parser, namespace, values, option_string=None):
		print "Official Repository" # Name
		print "web" # Type (maybe web for web, or anything else for usb)
		print "http://www.gcw-zero.com/files/upload/opk/" #URL
		print "./repositories/official.py --update" #Call for updating the list

class UpdateAction(argparse.Action):
	def __call__(self, parser, namespace, values, option_string=None):
		process = subprocess.Popen('wget -qO- http://www.gcw-zero.com/downloads',stdout=subprocess.PIPE,shell=True)
		output = process.stdout.read().split('<div class="downloads_overview">')
		for output_part in output:
			part = output_part.split('\n')
			program_name_description = part[9]
			name = program_name_description.split('>')[1].split('<')[0];
			if (name == ""):
				continue
			print "["+name+"]"
			description = program_name_description.split('>')[3];
			print "description: "+description
			filename = part[4].split('href="file.php?file=')[1].split('">')[0];
			print "filename: " + filename
			l = len(part)
			for i in range(0,l-1):
				if string.find(part[i],'Publication Date') != -1:
					version = part[i+1]
					version = version.split('>')[1]
					version = version.split('<')[0]
					t = time.strptime(version,"%A, %d %b %Y")
					print "version: " + str(calendar.timegm(t)) #NEEDED!
					break
		

def main():
	parser = argparse.ArgumentParser(description="Ziz's Repository script")
	parser.add_argument('--register', nargs=0, action=RegisterAction)
	parser.add_argument('--update', nargs=0, action=UpdateAction)
	args = parser.parse_args()

if __name__ == "__main__":
	main()
