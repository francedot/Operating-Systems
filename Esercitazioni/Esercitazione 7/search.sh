#! /bin/bash

#controllo sintassi
if [ $# -lt 3 ]
then
	printf "Sintassi non rispettata\n"
	exit 1
fi

#controllo correttezza direttori
i=0
for dir in $*
do
	case "$i" in
	0 | 1 ) ;;
	* ) 
		if ! test -d "$dir"
		then
			printf "$dir non è una directory!\n"
			exit 2 
		else
			printf "$dir corretta!\n"
		fi;;
	esac
	i=`expr $i + 1`
done

oldpath=$PATH
PATH=$PATH:`pwd`
printf "PATH Aggiornato: $PATH\n"

#ciclo per ogni direttorio e ad ogni direttorio mando l'algoritmo ricorsivo!

>/home/francesco/Desktop/output
chmod 00777 /home/francesco/Desktop/output

i=0
for f in $*
do
	case "$i" in
	0 | 1 ) ;;
	* ) 	printf "Mando la ricorsione per $f\n"
		recursiveSearch.sh $1 $2 "$f"
		;;
	esac
	i=`expr $i + 1`
done

PATH=$oldpath