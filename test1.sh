#!/bin/sh

echo '' > test.log

for i in `seq -w 1 50`
do
	echo "$i: "
	echo "$i: " >> test1.log
	bin/Debug/compiler -l test1/$i.in > test1/$i.res	
	diff -q -b test1/$i.out test1/$i.res 
	diff -b --side-by-side test1/$i.out test1/$i.res >> test.log
done
