# Si realizzi un file comandi Unix con la seguente interfaccia:
# count.sh <dir> <string>
# dove:
# • dir è un direttorio assoluto esistente nel filesystem;
# • <string> una stringa.
# Dopo aver effettuato tutti gli opportuni controlli sui parametri in ingresso, il file
# comandi si deve occupare di cercare, in ciascun sottodirettorio di dir, tutti i file
# regolari nelle cui ultime 10 righe compaia <string> almeno una volta.
# Per ciascun file così trovato all'interno di un sottodirettorio, si scriva in un opportuno
# file di report (es. report.txt) nel sottodirettorio corrente, contenente la stringa
# <nomeassoluto_file> <numeroOccorrenzeString>
# cioè una stringa che indichi il nome del file analizzato e il numero effettivo di
# occorrenze di <string> trovate nelle ultime 10 righe del file.

#! /bin/bash

#controllo argomenti --> vedi altri snippet
if [ $# -ne 2 ]
then
	echo -e "Numero di argomenti sbagliato! $#\n"
	exit 1
fi

if ! test -d "$1"
then
	echo -e "Argomento 1 non rappresenta una cartella!\n"
	exit 2
fi

#lo script ricorsivo che per ipotesi ho in ~/Desktop/
oldpath=$PATH
PATH=$PATH:`pwd`
echo -e "PATH Aggiornato: $PATH"

#creo stringa inversa
>tmp
chmod 00777 tmp
printf "$2" >> tmp
inv=`rev tmp`
rm tmp

#echo $inv
#exit 0

#lancio lo script ricorsivo passando tot parametri
recursiveCount.sh "$1" "$inv"

PATH=$oldpath

#tutto andato a buon fine
exit 0
