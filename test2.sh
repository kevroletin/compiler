#!/bin/sh

echo '' > test.log

for i in `seq -w 1 30`
do
	echo "$i: "
	echo "$i: " >> test2.log
	bin/Debug/compiler -s test2/$i.in > test2/$i.res	
	diff -q -b test2/$i.out test2/$i.res
	diff -b --side-by-side test2/$i.out test2/$i.res >> test.log
done
