#!/usr/bin/python
#CHANGE ONLY, IF YOU KNOW, WHAT YOU DO!
#OPKMANAGER WILL CRASH IF YOUR OUTPUT IS INVALID!
import subprocess
import argparse
import time
import calendar
import string
import sys

class RegisterAction(argparse.Action):
	def __call__(self, parser, namespace, values, option_string=None):
		print 'Google drive "Repository"' # Name
		print "web" # Type (maybe web for web, or anything else for usb)
		print "https://docs.google.com/uc?export=download\&id=" #URL
		print "google_drive.py --update" #Call for updating the list
		print "G" #letter to show

class UpdateAction(argparse.Action):
	def __call__(self, parser, namespace, values, option_string=None):
		process = subprocess.Popen('wget --no-check-certificate --timeout='+str(values[0])+' -qO- https://docs.google.com/folderview?id=0B9EgGumkJaabSE9TaEdFbVVBSlU#list',stdout=subprocess.PIPE,shell=True)
		output = process.stdout.read().split('\n')[2]
		output = output.split('<div class="flip-entry" id="entry-')
		first = 1
		for output_part in output:
			if first:
				first = 0
				continue
			filename = output_part.split('"')[0]
			real_filename = output_part.split('<div class="flip-entry-title">')[1].split('</div>')[0]
			splitted_filename = real_filename.split('.opk')
			version = output_part.split('<div class="flip-entry-last-modified"><div>')[1].split('</div')[0]
			if len(splitted_filename) < 2:
				continue
			print "["+splitted_filename[0]+"]"
			print "download_filename: "+filename
			print "filename: "+real_filename
			t = time.strptime(version,"%d.%m.%y")
			print "version: " + str(calendar.timegm(t)) #NEEDED!
			print ""
		
		

def main():
	parser = argparse.ArgumentParser(description="Ziz's Repository script")
	parser.add_argument('--register', nargs=0, action=RegisterAction)
	parser.add_argument('--update', nargs=1, action=UpdateAction)
	args = parser.parse_args()

if __name__ == "__main__":
	main()
