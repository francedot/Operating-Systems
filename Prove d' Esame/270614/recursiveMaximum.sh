#! /bin/bash

#mi sposto in questa cartella!
cd $3

printf "Sono entrato in "`pwd`"\n"

if test "`ls`"
then
	for f in *
	do
		if ! test -d $f
		then
			#è un file
			#testo che abbia groupId=$1
			if [ "$1" -eq `stat --format=%g "$f"` ]
			then
				printf "$f appartiene a groupid $1\n"
				occ=`cat $f | grep -o "$2" | wc -l`
				printf "$occ\n" >> "$4"
				printf "`pwd`/$f\n" >> "$4"
			fi
		else
			#è una directory
			printf "Mi trovo davanti la cartella $f!\n"
			#rilancio lo script
			"$0" $1 $2 "$f" "$4"
		fi
	done
else
	printf "Cartella $3 vuota\n"
	#exit 0
fi
exit 0