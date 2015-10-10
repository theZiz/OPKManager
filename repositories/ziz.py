#!/usr/bin/python
#CHANGE ONLY, IF YOU KNOW, WHAT YOU DO!
#OPKMANAGER WILL CRASH IF YOUR OUTPUT IS INVALID!
import subprocess
import argparse
import time
import calendar
import datetime

class RegisterAction(argparse.Action):
	def __call__(self, parser, namespace, values, option_string=None):
		print "Community Repository" # Name
		print "web" # Type (maybe web for web, or anything else for usb)
		print "" #URL
		print "ziz.py --update" #Call for updating the list
		print "C" #letter to show

class UpdateAction(argparse.Action):
	def __call__(self, parser, namespace, values, option_string=None):
		#Ziz:
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
			print "description: A puzzle game. On a tube! Brought to you by Ziz."#Description. Not needed.
			print "url_addition: http://ziz.gp2x.de/downloads/puzzletube/"#Url addition. Not needed.
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
			print "description: An alternative manager for Compo4All. Brought to you by Ziz."
			print "url_addition: http://ziz.gp2x.de/downloads/Sparrow-C4A-Manager/"
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
			print "description: A manager for all your opk files. Brought to you by Ziz."
			print "long_description: OPK Manager is a manager for OPK files. You can:\\n* Copy opk files\\n* Move opk files\\n* Delete opk files\\n* Install new opk files via internet\\n* See descriptions\\n* See the installation date\\n* See screenshots"
			print "url_addition: http://ziz.gp2x.de/downloads/OPKManager/"
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
			print "description: A simple text editor. Brought to you by Ziz."
			print "long_description: A text editor for the gcw. Use it for small changes of text configs or viweing large log files."
			print "url_addition: http://ziz.gp2x.de/downloads/glutexto/"
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
			print "description: A jump and run about a snowman. Brought to you by Ziz."
			print "url_addition: http://ziz.gp2x.de/downloads/snowman/"
			print "image_url: http://ziz.gp2x.de/screenshots/snowman.png"
			print ""
		process = subprocess.Popen('wget --timeout='+str(values[0])+' -qO- http://ziz.gp2x.de/hase/index.htm',stdout=subprocess.PIPE,shell=True)
		if len(output) > 1:
			print "[Hase]"
			output = process.stdout.read().split('\n')
			for line in output:
				if line.startswith("<li><a href=hase.opk"):
					#searching the filename itself
					parts = line.split('<li><a href=')
					parts = parts[1].split(' ')
					print "filename: " + parts[0] #NEEDED!
				if line.startswith("<p>Updated at the "):
					#Parsing the time
					parts = line.split('<p>Updated at the ')
					t = time.strptime(parts[1],"%d.%m.%Y %H:%M.</p>")
					print "version: " + str(calendar.timegm(t)) #NEEDED!
			print "description: A several times rewarded game about hares in space. A bit like worms, but with more gravity fun and online gaming against other GCW, Pandora or even PC users!. Brought to you by Ziz."
			print "url_addition: http://ziz.gp2x.de/hase/"
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
			print "description: A simple SDL IRC client. Brought to you by Ziz."
			print "url_addition: http://ziz.gp2x.de/downloads/sissi/"
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
			print "description: The first REAL 3D game for the GCW. Brought to you by Ziz."
			print "url_addition: http://ziz.gp2x.de/downloads/meteoroid3d/"
			print "image_url: http://ziz.gp2x.de/screenshots/meteoroid3d_320.png"
			print ""
		#David Knight
		process = subprocess.Popen('wget --no-check-certificate --timeout='+str(values[0])+' -qO- https://drive.google.com/folderview?hl=en\&id=0BwYi4RGX-HSaY01ielN2N2lKa1k#list',stdout=subprocess.PIPE,shell=True)
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
			print "url_addition: https://www.googledrive.com/host/"
			print "description: " + splitted_filename[0] + ". Brought to you by David Knight."
		#Gameblabla
		#Oswan
		print "[Oswan]"
		print "filename: oswan.opk" #NEEDED!
		print "description: Gameblabla's Oswan is a Wonderswan emulator based on OswanJ and Alekmaul's Oswan. Brought to you by Gameblabla."
		print "url_addition: https://github.com/gameblabla/oswan/raw/master/"
		print "image_url: https://gameblabla.olympe.in/img/mmn1.png"
		process = subprocess.Popen('wget --no-check-certificate --timeout='+str(values[0])+' -qO- https://github.com/gameblabla/oswan',stdout=subprocess.PIPE,shell=True)
		output = process.stdout.read().split('\n');
		found = 0
		for output_part in output:
			if (output_part.startswith('            <span class="css-truncate css-truncate-target"><a href="/gameblabla/oswan/blob/master/oswan.opk"')):
				found = 1
			if (found == 1 and output_part.startswith('            <span class="css-truncate css-truncate-target"><time datetime="')):
				version = output_part.split('            <span class="css-truncate css-truncate-target"><time datetime="')[1].split('" is="time-ago">')[0]
				t = time.strptime(version,"%Y-%m-%dT%H:%M:%SZ")
				print "version: " + str(calendar.timegm(t)) #NEEDED!
				break
		print ""
		#Methane Brothers
		print "[Super Methane Bros.]"
		print "filename: methane.opk" #NEEDED!
		print "description: Super Methane Brothers is a Bubble Bobble clone originally released for Amiga by Apache Software Ltd.. Brought to you by Gameblabla."
		print "url_addition: https://github.com/gameblabla/methane/raw/master/source/gcw/"
		print "image_url: https://upload.wikimedia.org/wikipedia/en/4/4b/Super_Methane_Bros_sm.png"
		t = time.strptime("2015-09-29","%Y-%m-%d")
		print "version: " + str(calendar.timegm(t)) #NEEDED!
		print ""
		#Ganbare! Natsuki-San!
		print "[Gannatsu Portable]"
		print "filename: gnp.opk" #NEEDED!
		print "description: Puzzle game. Brought to you by Gameblabla."
		print "url_addition: https://gameblabla.olympe.in/files/"
		print "image_url: http://speeddemosarchive.com/gfx/GanbareNatsukiSan_1.jpg"
		t = time.strptime("2015-07-28","%Y-%m-%d")
		print "version: " + str(calendar.timegm(t)) #NEEDED!
		print ""
		#Potator
		print "[Potator]"
		print "filename: potator.opk" #NEEDED!
		print "description: Watara Supervision Emulator. Brought to you by Gameblabla."
		print "url_addition: https://github.com/alekmaul/potator/raw/master/distrib/"
		t = time.strptime("2015-09-04","%Y-%m-%d")
		print "version: " + str(calendar.timegm(t)) #NEEDED!
		print ""
		#Mednafen
		process = subprocess.Popen('wget --no-check-certificate --timeout='+str(values[0])+' -qO- https://github.com/gameblabla/mednafen-gcw',stdout=subprocess.PIPE,shell=True)
		output = process.stdout.read().split('\n');
		found = 0
		for output_part in output:
			if (output_part.startswith('            <span class="css-truncate css-truncate-target"><a href="/gameblabla/mednafen-gcw/tree/master/release"')):
				found = 1
			if (found == 1 and output_part.startswith('            <span class="css-truncate css-truncate-target"><time datetime="')):
				version = output_part.split('            <span class="css-truncate css-truncate-target"><time datetime="')[1].split('" is="time-ago">')[0]
				t = time.strptime(version,"%Y-%m-%dT%H:%M:%SZ")
				break
		print "[Mednafen]"
		print "filename: mednafen_generic.opk" #NEEDED!
		print "description: Mednafan for GB/GBA/NES/LYNX. Brought to you by Gameblabla."
		print "url_addition: https://github.com/gameblabla/mednafen-gcw/raw/master/release/"
		print "version: " + str(calendar.timegm(t)) #NEEDED!
		print ""
		print "[Mednafen PCE]"
		print "filename: mednafen_pce.opk" #NEEDED!
		print "description: Mednafan for PCE. Brought to you by Gameblabla."
		print "url_addition: https://github.com/gameblabla/mednafen-gcw/raw/master/release/"
		print "version: " + str(calendar.timegm(t)) #NEEDED!
		print ""
		print "[Mednafen PCFX]"
		print "filename: mednafen_pcfx.opk" #NEEDED!
		print "description: Mednafan for PCFX. Brought to you by Gameblabla."
		print "url_addition: https://github.com/gameblabla/mednafen-gcw/raw/master/release/"
		print "version: " + str(calendar.timegm(t)) #NEEDED!
		print ""
		print "[Mednafen PSX]"
		print "filename: mednafen_psx.opk" #NEEDED!
		print "description: Mednafan for PSX. Brought to you by Gameblabla."
		print "url_addition: https://github.com/gameblabla/mednafen-gcw/raw/master/release/"
		print "version: " + str(calendar.timegm(t)) #NEEDED!
		print ""

def main():
	parser = argparse.ArgumentParser(description="Community Repository script")
	parser.add_argument('--register', nargs=0, action=RegisterAction)
	parser.add_argument('--update', nargs=1, action=UpdateAction)
	args = parser.parse_args()

if __name__ == "__main__":
	main()
