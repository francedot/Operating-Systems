#! /bin/bash

#mi sposto in questa cartella!

cd $2

printf "Sono entrato in "`pwd`"\n"

#exit 0

if test "`ls`"
then
	for f in *
	do
		if ! test -d $f
		then
			#è un file
			#salvo il timestamp dell' ultimo accesso
			lastAccess=`stat --format="%X" "$f"`
			fileAbs="`pwd`"/$f
			printf "\"$lastAccess,"$fileAbs"\"\n" >> "$3"
			#cerco se questo file è presente in log
			lastAccessLog=`grep "$fileAbs" "$1" | cut -d'"' -f2 | cut -d',' -f1`
			#campo=`cut -d' ' -f1` "$riga"
			#printf "LA riga è: "$riga"\n"
			if [ $lastAccess -le $lastAccessLog ]
			then
				printf ""$fileAbs"\n"
			fi
		else
			#è una directory
			printf "Mi trovo davanti la cartella $f!\n"
			#rilancio lo script
			"$0" $1 $f "$3"
		fi
	done
else
	printf "Cartella "$2" vuota\n"
	#exit 0
fi
exit 0
