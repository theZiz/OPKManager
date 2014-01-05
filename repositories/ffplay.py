#!/usr/bin/python
#CHANGE ONLY, IF YOU KNOW, WHAT YOU DO!
#OPKMANAGER WILL CRASH IF YOUR OUTPUT IS INVALID!
import subprocess
import argparse
import time
import calendar

class RegisterAction(argparse.Action):
	def __call__(self, parser, namespace, values, option_string=None):
		print "FFPlay Mini Repo" # Name
		print "web" # Type (maybe web for web, or anything else for usb)
		print "https://dl.dropboxusercontent.com/s/q5uvsbuqozg6tnb/" #URL
		print "ffplay.py --update" #Call for updating the list
		print "F" #letter to show

class UpdateAction(argparse.Action):
	def __call__(self, parser, namespace, values, option_string=None):
		process = subprocess.Popen('wget --timeout='+str(values[0])+' --user-agent="Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:25.0) Gecko/20100101 Firefox/25.0" -qO- http://boards.dingoonity.org/gcw-releases/ffplay-media-player/',stdout=subprocess.PIPE,shell=True)
		output = process.stdout.read().split('\n')
		if len(output) > 1:
			for line in output:
				if line.find('<em>Last Edit: ') != -1:
					zeit = line.split('<em>Last Edit: ')[1].split(' by ')[0]
					if zeit.startswith('<strong>Today</strong> at'):
						rest = zeit.split('<strong>Today</strong> at')[1]
						date = time.strftime("%B %d, %Y,");
						zeit = date + rest
					t = time.strptime(zeit,"%B %d, %Y, %I:%M:%S %p")
					version = str(calendar.timegm(t)) #NEEDED!
					break
			print "[FFplay-sub]" #Starts a new entry
			print "filename: ffplay-gcw0.opk"
			print "description: FFplay is a very simple and portable media player using the FFmpeg-2.1.1 libraries and the SDL library."
			print 'long_description:  Controls:\\n- Play/Pause: "Y" button\\n- Seek backward/forward 10 seconds: D-pad left/right\\n- Seek backward/forward 1 minute: D-pad down/up\\n- Seek to percentage in file corresponding to fraction of width: Mouse click\\n- Cycle audio channel: "Left shoulder"\\n- Show audio waves: "X" button\\n- Exit: "Select" button'
			print "version: "+version
			print "" #line breaks make the like beatiful

def main():
	parser = argparse.ArgumentParser(description="Ziz's Repository script")
	parser.add_argument('--register', nargs=0, action=RegisterAction)
	parser.add_argument('--update', nargs=1, action=UpdateAction)
	args = parser.parse_args()

if __name__ == "__main__":
	main()
