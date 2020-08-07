#!/bin/sh
# Change the folder to YOUR sparrow3d folder!
cd ../sparrow3d
#FILES=./target-files/*
#The program only makes sense for a couple of targets:
FILES="./target-files/gcw.mk ./target-files/rg350.mk"
echo "Compiling for all targets..."
for f in $FILES
do
	cd ../OPKManager
	TARGET=`echo "$f" | cut -d/ -f3 | cut -d. -f1`
	make clean > /dev/null
	make TARGET=$TARGET > /dev/null
	if [ $? -ne 0 ]; then
		echo "Error compiling for \033[1;31m$TARGET\033[0m!"
	else
		echo "Everything fine with \033[1;32m$TARGET\033[0m!"
	fi
done
echo "Compiling for default..."
make clean > /dev/null
make > /dev/null
