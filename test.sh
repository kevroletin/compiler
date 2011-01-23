#!/bin/sh

for dir in test*
do
	if [ -d $dir ]
	then
		echo "---$dir---"
		cd $dir
		sh test.sh -n
		cd ..
	fi
done
