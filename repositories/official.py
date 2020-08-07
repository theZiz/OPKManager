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
		print "Official Repository" # Name
		print "web" # Type (maybe web for web, or anything else for usb)
		print "http://www.gcw-zero.com/files/upload/opk/" #URL
		print "official.py --update" #Call for updating the list
		print "O" #letter to show

class UpdateAction(argparse.Action):
	def __call__(self, parser, namespace, values, option_string=None):
		process = subprocess.Popen('wget --timeout='+str(values[0])+' -qO- http://ziz.gp2x.de/gcw-repos/count.php',stdout=subprocess.PIPE,shell=True)
		process = subprocess.Popen('wget --timeout='+str(values[0])+' -qO- http://www.gcw-zero.com/downloads',stdout=subprocess.PIPE,shell=True)
		#process = subprocess.Popen('wget --timeout='+str(values[0])+' -qO- http://ziz.gp2x.de/temp/test.htm',stdout=subprocess.PIPE,shell=True)
		#process = subprocess.Popen('cat downloads',stdout=subprocess.PIPE,shell=True)
		output = process.stdout.read().split('<div class="downloads_overview">')
		for output_part in output:
			part = output_part.split('\n')
			line_number = 0;
			not_found = 1;
			while (line_number < len(part)):
				if (part[line_number].strip().startswith('<span class="downloads_title">')):
					not_found = 0;
					break;
				line_number += 1;
			if not_found:
				continue;
			program_name_description = part[line_number];
			name = program_name_description.split('>')[1].split('<')[0];
			if (name == ""):
				continue
			line_number = 0;
			not_found = 1;
			while (line_number < len(part)):
				if (part[line_number].strip().startswith('<a class="downloads_link"')):
					not_found = 0;
					break;
				line_number += 1;
			if not_found:
				continue;
			filename = part[line_number].split('href="file.php?file=')[1].split('">')[0];
			print "["+name+"]"
			description = program_name_description.split('>')[3];
			print "description: "+description
			print "filename: " + filename
			l = len(part)
			found_version = 0
			found_image = 0
			found_long = 0;
			for i in range(0,l-1):
				if string.find(part[i],'Publication Date') != -1:
					version = part[i+1]
					version = version.split('>')[1]
					version = version.split('<')[0]
					t = time.strptime(version,"%A, %d %b %Y")
					print "version: " + str(calendar.timegm(t)) #NEEDED!
					found_version = 1
				if string.find(part[i],'<div class="downloads_preview"') != -1:
					image = part[i];
					image = image.split("background-image: url('")[1].split("');")[0];
					print "image_url: http://www.gcw-zero.com/" + image
				if string.find(part[i],'<p class="more fade">') != -1:
					long_description = part[i];
					long_description = long_description.split('<p class="more fade">')[1].split("</p>")[0];
					long_description = long_description.replace('<br /> ','\\n')
					long_description = long_description.split('>')
					sys.stdout.write("long_description: ")
					for long_description_part in long_description:
						sys.stdout.write(long_description_part.split('<')[0])
					sys.stdout.write('\n')
					found_long = 1
				if (found_version and found_image and found_long):
					break
			print ""


def main():
	parser = argparse.ArgumentParser(description="Ziz's Repository script")
	parser.add_argument('--register', nargs=0, action=RegisterAction)
	parser.add_argument('--update', nargs=1, action=UpdateAction)
	args = parser.parse_args()

if __name__ == "__main__":
	main()
