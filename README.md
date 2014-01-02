OPKManager
==========

This is "OPKManager", a manager for your opk files installed on your gcw zero. With this program you can copy, move and delete your opk files in your internal space, sd card and even connected usb devices. Use the buttons described in the main window to do these tasks ([a], [d], [w]). The icon beside the program name tells you, where your application lays. Use your D-Pad to navigate through the items and to change the sorting.

Installation
============

If you are connected with the internet e.g. via WiFi or USB network, press [l] to update the list of online available programs. You will see a new globe-like icon on some applications. These applications are available online and you can newly install or update them like you copy programs with [a]. Just select a repository instead of a local location as source.

Details
=======

Press [s] to get some details about the selected program. You will see the programs name and at least
one version and the location of this version. The version is in fact just the date when you installed the application. If some of your sources have a newer version than the others, a little explanation mark will be shown in the main view. So you easily see, if one of your applications e.g. has an update in a repository ready to install. If a screenshot is available press [d] to download and show it.

Repositories
============

Your repositories are scripts, which are stored on your device and online. The scripts tell the OPKManager, which packets exist, where to download them, whether their have images and so on. Your local scripts are stored in /usr/local/home/.config/OPKManager/scripts. The list of your online script locations is created at first start in /usr/local/home/.config/OPKManager/repositories.txt . Via ftp or an editor on the gcw you can edit this file to delete or add repository scripts. Every script in this list will be downloaded to the previously named location. If a script with the same name
allready exist, it will be overwritten (updated). However if you delete the url in the repositories.txt the script will still be used while updating your repositories, beause just local repositories are possible, too. If you don't want this, don't forget to delete the script after editing the repositories.txt.

Aliases
=======

Some programs have different names in the opk file than in the repository. With this two entries would be shown, e.g. one for Unnamend Monkey Game and one for UMG. So I use alias files to define, which names describe the same program. These alias files are stored in /usr/local/home/.config/OPKManager/alias. Furthermore all urls in /usr/local/home/.config/OPKManager/alias.txt are downloaded at update time to have update to date aliases, especially if new programs are added to the repository.

Closing words
=============

To run the selected program press [S]. To exit the OPKManager press [E].

This program was developed by Ziz. It comes with no warranty of anything. ;) It is licensed under the GPL2+. If you sell this program, Richard Stallman will come to you at night...
