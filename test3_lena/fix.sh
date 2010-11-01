for i in `seq -w 1 60`
do
	perl fix.pl $i.in.ans > $i.out
done
