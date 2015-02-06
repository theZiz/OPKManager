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
		print "ziz.py --update" #Call for updating the list
		print "Z" #letter to show

class UpdateAction(argparse.Action):
	def __call__(self, parser, namespace, values, option_string=None):
		process = subprocess.Popen('wget --timeout='+str(values[0])+' -qO- http://ziz.gp2x.de/downloads/puzzletube/index.htm',stdout=subprocess.PIPE,shell=True)
		output = process.stdout.read().split('\n')
		if len(output) > 1:
			print "[Puzzletube]" #Starts a new entry
			for line in output:
				if line.startswith("<a href=puzzletube.opk"):
					#searching the filename itself
					parts = line.split('<a href=')
					parts = parts[1].split(' ')
					print "filename: " + parts[0] #NEEDED!
				if line.startswith("Updated at the "):
					#Parsing the time
					parts = line.split('Updated at the ')
					t = time.strptime(parts[1],"%d.%m.%Y %H:%M.")
					print "version: " + str(calendar.timegm(t)) #NEEDED!
			print "description: A puzzle game. On a tube!"#Description. Not needed.
			print "url_addition: puzzletube/"#Url addition. Not needed.
			print "image_url: http://ziz.gp2x.de/screenshots/puzzletube.png"
			print "" #line breaks make the like beatiful
		process = subprocess.Popen('wget --timeout='+str(values[0])+' -qO- http://ziz.gp2x.de/downloads/Sparrow-C4A-Manager/index.htm',stdout=subprocess.PIPE,shell=True)
		if len(output) > 1:
			print "[Sparrow C4A Manager]"
			output = process.stdout.read().split('\n')
			for line in output:
				if line.startswith("<a href=Sparrow-C4A-Manager.opk"):
					#searching the filename itself
					parts = line.split('<a href=')
					parts = parts[1].split(' ')
					print "filename: " + parts[0] #NEEDED!
				if line.startswith("Updated at the "):
					#Parsing the time
					parts = line.split('Updated at the ')
					t = time.strptime(parts[1],"%d.%m.%Y %H:%M.")
					print "version: " + str(calendar.timegm(t)) #NEEDED!
			print "description: An alternative manager for Compo4All"
			print "url_addition: Sparrow-C4A-Manager/"
			print "image_url: http://ziz.gp2x.de/screenshots/Sparrow-C4A-Manager.png"
			print ""
		process = subprocess.Popen('wget --timeout='+str(values[0])+' -qO- http://ziz.gp2x.de/downloads/OPKManager/index.htm',stdout=subprocess.PIPE,shell=True)
		if len(output) > 1:
			print "[OPKManager]"
			output = process.stdout.read().split('\n')
			for line in output:
				if line.startswith("<a href="):
					#searching the filename itself
					parts = line.split('<a href=')
					parts = parts[1].split(' ')
					print "filename: " + parts[0] #NEEDED!
				if line.startswith("Updated at the "):
					#Parsing the time
					parts = line.split('Updated at the ')
					t = time.strptime(parts[1],"%d.%m.%Y %H:%M.")
					print "version: " + str(calendar.timegm(t)) #NEEDED!
			print "description: A manager for all your opk files"
			print "long_description: OPK Manager is a manager for OPK files. You can:\\n* Copy opk files\\n* Move opk files\\n* Delete opk files\\n* Install new opk files via internet\\n* See descriptions\\n* See the installation date\\n* See screenshots"
			print "url_addition: OPKManager/"
			print "image_url: http://ziz.gp2x.de/screenshots/OPKManager.png"
			print ""
		process = subprocess.Popen('wget --timeout='+str(values[0])+' -qO- http://ziz.gp2x.de/downloads/glutexto/index.htm',stdout=subprocess.PIPE,shell=True)
		if len(output) > 1:
			print "[glutexto]"
			output = process.stdout.read().split('\n')
			for line in output:
				if line.startswith("<a href=glutexto.opk"):
					#searching the filename itself
					parts = line.split('<a href=')
					parts = parts[1].split(' ')
					print "filename: " + parts[0] #NEEDED!
				if line.startswith("Updated at the "):
					#Parsing the time
					parts = line.split('Updated at the ')
					t = time.strptime(parts[1],"%d.%m.%Y %H:%M.")
					print "version: " + str(calendar.timegm(t)) #NEEDED!
			print "description: A simple text editor"
			print "long_description: A text editor for the gcw. Use it for small changes of text configs or viweing large log files."
			print "url_addition: glutexto/"
			print "image_url: http://ziz.gp2x.de/screenshots/glutexto.png"
			print ""
		process = subprocess.Popen('wget --timeout='+str(values[0])+' -qO- http://ziz.gp2x.de/downloads/snowman/index.htm',stdout=subprocess.PIPE,shell=True)
		if len(output) > 1:
			print "[Snowman]"
			output = process.stdout.read().split('\n')
			for line in output:
				if line.startswith("<a href=snowman.opk"):
					#searching the filename itself
					parts = line.split('<a href=')
					parts = parts[1].split(' ')
					print "filename: " + parts[0] #NEEDED!
				if line.startswith("Updated at the "):
					#Parsing the time
					parts = line.split('Updated at the ')
					t = time.strptime(parts[1],"%d.%m.%Y %H:%M.")
					print "version: " + str(calendar.timegm(t)) #NEEDED!
			print "description: A jump and run about a snowman"
			print "url_addition: snowman/"
			print "image_url: http://ziz.gp2x.de/screenshots/snowman.png"
			print ""
		process = subprocess.Popen('wget --timeout='+str(values[0])+' -qO- http://ziz.gp2x.de/downloads/hase/index.htm',stdout=subprocess.PIPE,shell=True)
		if len(output) > 1:
			print "[Hase]"
			output = process.stdout.read().split('\n')
			for line in output:
				if line.startswith("<a href=hase.opk"):
					#searching the filename itself
					parts = line.split('<a href=')
					parts = parts[1].split(' ')
					print "filename: " + parts[0] #NEEDED!
				if line.startswith("Updated at the "):
					#Parsing the time
					parts = line.split('Updated at the ')
					t = time.strptime(parts[1],"%d.%m.%Y %H:%M.")
					print "version: " + str(calendar.timegm(t)) #NEEDED!
			print "description: A Prototype about hares in space. Like worms."
			print "url_addition: hase/"
			print "image_url: http://ziz.gp2x.de/screenshots/hase320.png"
			print ""
		process = subprocess.Popen('wget --timeout='+str(values[0])+' -qO- http://ziz.gp2x.de/downloads/sissi/index.htm',stdout=subprocess.PIPE,shell=True)
		if len(output) > 1:
			print "[Sissi]"
			output = process.stdout.read().split('\n')
			for line in output:
				if line.startswith("<a href=sissi.opk"):
					#searching the filename itself
					parts = line.split('<a href=')
					parts = parts[1].split(' ')
					print "filename: " + parts[0] #NEEDED!
				if line.startswith("Updated at the "):
					#Parsing the time
					parts = line.split('Updated at the ')
					t = time.strptime(parts[1],"%d.%m.%Y %H:%M.")
					print "version: " + str(calendar.timegm(t)) #NEEDED!
			print "description: A simple SDL IRC client"
			print "url_addition: sissi/"
			print "image_url: http://ziz.gp2x.de/screenshots/sissi320.png"
			print ""
		process = subprocess.Popen('wget --timeout='+str(values[0])+' -qO- http://ziz.gp2x.de/downloads/meteoroid3d/index.htm',stdout=subprocess.PIPE,shell=True)
		if len(output) > 1:
			print "[Meteoroid3D]"
			output = process.stdout.read().split('\n')
			for line in output:
				if line.startswith("<a href=meteoroid3d.opk"):
					#searching the filename itself
					parts = line.split('<a href=')
					parts = parts[1].split(' ')
					print "filename: " + parts[0] #NEEDED!
				if line.startswith("Updated at the "):
					#Parsing the time
					parts = line.split('Updated at the ')
					t = time.strptime(parts[1],"%d.%m.%Y %H:%M.")
					print "version: " + str(calendar.timegm(t)) #NEEDED!
			print "description: The first REAL 3D game for the GCW"
			print "url_addition: meteoroid3d/"
			print "image_url: http://ziz.gp2x.de/screenshots/meteoroid3d_320.png"
			print ""
def main():
	parser = argparse.ArgumentParser(description="Ziz's Repository script")
	parser.add_argument('--register', nargs=0, action=RegisterAction)
	parser.add_argument('--update', nargs=1, action=UpdateAction)
	args = parser.parse_args()

if __name__ == "__main__":
	main()
