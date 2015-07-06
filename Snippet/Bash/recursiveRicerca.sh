#! /bin/bash

#mi sposto in questa cartella! N.B: controlla quale parametro ti ha dato ricerca.sh!
cd $3

echo "Sono entrato in "`pwd`""

if test "`ls`"
then
	for f in *
	do
		if ! test -d $f
		then
			#è un file
			#fai quello che devi fare
			#E.G:
			#testo che abbia groupId=$1
			#if [ "$1" -eq `stat --format=%g "$f"` ]
			#then
			#	printf "$f appartiene a groupid $1\n"
			#	occ=`cat $f | grep -o "$2" | wc -l`
			#	printf "$occ\n" >> "$4"
			#	printf "`pwd`/$f\n" >> "$4"
			#fi
		else
			#è una directory
			echo "Mi trovo davanti la cartella $f!"
			#rilancio lo script. N,B controlla i parametri passati da ricerca.sh
			#e quelli da passare!
			"$0" $1 $2 "$f" "$4"
		fi
	done
else
	echo "Cartella $3 vuota"
	#torno indietro nella ricorsione
	#exit 0
fi

#non mettere exit 0 altrimenti ti esce dallo script alla prima cartella vuota!
