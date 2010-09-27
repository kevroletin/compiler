#!/bin/sh

echo '' > test.log

for i in `seq -w 1 40`
do
	bin/Debug/compiler -l test/$i.in > test/$i.res	
	echo "$i: "
	echo "$i: " >> test.log
	echo `diff -q -b test/$i.out test/$i.res` 
	echo `diff -b --side-by-side test/$i.out test/$i.res` >> test.log
done
