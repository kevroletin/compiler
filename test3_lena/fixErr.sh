for i in `seq -w 1 50`
do
	a=`perl fixErr.pl $i.in.ans $i.res`
	if [ "$a" = "matched" ]
	then
		cp $i.res $i.out
		echo $i
	fi
done
