#!/usr/bin/python
#CHANGE ONLY, IF YOU KNOW, WHAT YOU DO!
#OPKMANAGER WILL CRASH IF YOUR OUTPUT IS INVALID!
import subprocess
import argparse

class RegisterAction(argparse.Action):
	def __call__(self, parser, namespace, values, option_string=None):
		print "Ziz's Repository" # Name
		print "web" # Type (maybe web for web, or anything else for usb)
		print "http://ziz.gp2x.de/downloads/" #URL
		print "./repositories/ziz.py --update" #Call for updating the list

class UpdateAction(argparse.Action):
	def __call__(self, parser, namespace, values, option_string=None):
		print "[Puzzletube]" #Starts a new entry
		print "" #optional line break
		print "version: 1387697148" #Version. NEEDED.
		print "description: A puzzle game. On a tube!"#Description. Not needed.
		print "url_addition: puzzletube/"#Url addition. Not needed.
		print "filename: puzzletube-gcw-1.3.2.0.opk" #Filename: Of course NEEDED.
		print "" #line breaks make the like beatiful

def main():
	parser = argparse.ArgumentParser(description="Ziz's Repository script")
	parser.add_argument('--register', nargs=0, action=RegisterAction)
	parser.add_argument('--update', nargs=0, action=UpdateAction)
	args = parser.parse_args()

if __name__ == "__main__":
	main()
