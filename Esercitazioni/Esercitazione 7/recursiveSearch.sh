#! /bin/bash

cd $3
printf "Sono in `pwd` ricorsivo\n"

#verifica che la cartella non sia vuota. N.B devo mettere doppi apici perchè devo verificare se stringa vuota altrimenti è
#come se non avessi l'argomento di test!
if test "`ls`"
then
	for f in *
	do
		if ! test -d $f
		then
			dimB=`stat --format=%s $f`
			dimkB=`echo "$dimB" / 1000.0 | bc -l`
			printf "file $f ha dim $dimkB kB\n"
			
			greaterMin=`echo "$dimkB >= $1" | bc -l`
			lesserMax=`echo "$dimkB <= $2" | bc -l`

			if [ $greaterMin -gt 0 -a $lesserMax -gt 0 ]
			then
				printf "`pwd`/$f: $dimkB kB\n" >> "/home/francesco/Desktop/output"
			fi
		else
			printf "Mi trovo davanti la cartella $f!\n"
			#rilancio lo script
			"$0" $1 $2 "$f"
		fi
	done
else
	printf "Cartella $3 vuota\n"
fi
exit 0