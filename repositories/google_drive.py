#!/usr/bin/python
#CHANGE ONLY, IF YOU KNOW, WHAT YOU DO!
#OPKMANAGER WILL CRASH IF YOUR OUTPUT IS INVALID!
import subprocess
import argparse
import time
import calendar
import string
import sys
import datetime

class RegisterAction(argparse.Action):
	def __call__(self, parser, namespace, values, option_string=None):
		print 'Google drive "Repository"' # Name
		print "web" # Type (maybe web for web, or anything else for usb)
		print "https://docs.google.com/uc?export=download\&id=" #URL
		print "google_drive.py --update" #Call for updating the list
		print "G" #letter to show

class UpdateAction(argparse.Action):
	def __call__(self, parser, namespace, values, option_string=None):
		process = subprocess.Popen('wget --no-check-certificate --timeout='+str(values[0])+' -qO- https://drive.google.com/folderview?hl=en\&id=0B9EgGumkJaabSE9TaEdFbVVBSlU#list',stdout=subprocess.PIPE,shell=True)
		output = process.stdout.read().split('\n')[1]
		output = output.split('<div class="flip-entry" id="entry-')
		now = datetime.datetime.now()
		first = 1
		for output_part in output:
			if first:
				first = 0
				continue
			filename = output_part.split('"')[0]
			real_filename = output_part.split('<div class="flip-entry-title">')[1].split('</div>')[0]
			splitted_filename = real_filename.split('.opk')
			if len(splitted_filename) < 2: # not ending with opk
				continue
			version = output_part.split('<div class="flip-entry-last-modified"><div>')[1].split('</div')[0]
			print "["+splitted_filename[0]+"]"
			print "download_filename: "+filename
			print "filename: "+real_filename
			try:
				t = time.strptime(version,"%m/%d/%y")
			except ValueError:
				try:
					# Mon d format
					date_split = version.split(' ')
					day = date_split[1];
					month = 1
					if (date_split[0] == 'Jan'):
						month = 1
					if (date_split[0] == 'Feb'):
						month = 2
					if (date_split[0] == 'Mar'):
						month = 3
					if (date_split[0] == 'Apr'):
						month = 4
					if (date_split[0] == 'May'):
						month = 5
					if (date_split[0] == 'Jun'):
						month = 6
					if (date_split[0] == 'Jul'):
						month = 7
					if (date_split[0] == 'Aug'):
						month = 8
					if (date_split[0] == 'Sep'):
						month = 9
					if (date_split[0] == 'Oct'):
						month = 10
					if (date_split[0] == 'Nov'):
						month = 11
					if (date_split[0] == 'Dec'):
						month = 12
					new_version = str(month) + "/" + day + "/" + str(now.year)
					t = time.strptime(new_version,"%m/%d/%Y")
				except ValueError:
					try:
						new_version = str(now.month) + "/" + str(now.day) + "/" + str(now.year) + " " + version
						t = time.strptime(new_version,"%m/%d/%Y %I:%M %p")
					except ValueError:
						new_version = str(now.month) + "/" + str(now.day) + "/" + str(now.year)
						t = time.strptime(new_version,"%m/%d/%Y")
			print "version: " + str(calendar.timegm(t)) #NEEDED!
			print "description: " + splitted_filename[0]
			print ""

def main():
	parser = argparse.ArgumentParser(description="Google Drive Repository script")
	parser.add_argument('--register', nargs=0, action=RegisterAction)
	parser.add_argument('--update', nargs=1, action=UpdateAction)
	args = parser.parse_args()

if __name__ == "__main__":
	main()
