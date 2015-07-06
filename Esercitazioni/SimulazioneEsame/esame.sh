#!/bin/bash

printf "dentr\n"

if [ $# != 2 ]
then
	printf "Errore sintassi file comandi\n"
	exit 1
fi

if ! test -f $1
then
	printf "log deve essere un file\n"
	exit 2
fi

if ! test -d $2
then
	printf "dir deve essere una cartella\n"
	exit 3
fi

tmpPath="/tmp/tmp_access"

oldpath=$PATH
PATH=$PATH:`pwd`
printf "PATH Aggiornato: $PATH\n"

#mi appoggio ad un file temporaneo per scrivere i nomi assoluti con il numero delle occorrenze
#N.B > per creazione file vuole path relativo e non assoluto!!!
>"$tmpPath"
chmod 00777 "$tmpPath"

esameRecursive.sh "`"pwd"`/$1" "$2" "$tmpPath"

PATH=$oldpath

rm $1
cp "$tmpPath" log

exit 0
