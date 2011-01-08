#!/bin/sh

num=0
fail=""
for file in *.in
do
    num=$((num+1));
    i=${file%%.in}
    ../debug -g $i.in > $i.s
    if [ "$?" -eq "0" ]
    then
        gcc -m32 $i.s -o a 2> $i.res
        if [ -e a ]
        then
                ./a > $i.res 2> $i.res
                rm a
        fi
    else
        cp $i.s $i.res
    fi
    tmp=$(diff -q -b $i.out $i.res)
    if [ -z $1 ] && [ "$1" != "n" ]; then echo "$i: $tmp"; fi
    if [ -n "$tmp" ]; then fail="$fail $i"; fi
done
echo "Number of tests: $num"
if [ -z  "$fail" ]
then
    echo "Ok";
else
    echo "Failed: $fail"
fi
