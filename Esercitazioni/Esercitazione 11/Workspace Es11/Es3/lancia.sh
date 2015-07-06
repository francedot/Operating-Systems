#! /bin/bash

#SINTASSI: lancia.sh fileout timeout tipo_op

curPath=`pwd`

if [ $# -ne 3 ]
then
	printf "Errore sintassi file comandi.\n"
	exit 1
fi

if  [[ ! -f "$1" ]]
then
	printf "Primo argomento deve essere il nome di un file.\n"
	exit 2
fi

if [[ ! -w "$1" ]]
then
	printf "Non ho diritto di scrittura sul file %s.\n" "$1"
	exit 3
fi

fileout=$1

#verifico che timeout sia un numero intero
#se c'è qualcosa che non è un un numero fallisce non curandosi di quello che c'è prima o dopo
if [[ $2 == *[!0-9]* ]]
then
	printf "$2 Non è un numero intero.\n"
	exit 3
fi

timeout=$2

if [[ $3 != 'd' && $3 != 'a' ]]
then
	printf "$3 Non è valido.\n"
	exit 4
fi

modo=$3

pathJavaSrc="/home/francesco/Dropbox/Ingegneria/Corsi Attuali/Sistemi Operativi T/Esercizi/Esercitazioni/Esercitazione 11/Workspace Es11/Aerei/src/"
pathJavaBin="/home/francesco/Dropbox/Ingegneria/Corsi Attuali/Sistemi Operativi T/Esercizi/Esercitazioni/Esercitazione 11/Workspace Es11/Aerei/bin/"

#avvio compilazione sorgenti java P0 in background (waiting)
#avvio esecuzione sorgenti java P1 in foreground (not waiting)

cd "$pathJavaSrc"

javac -d "$pathJavaBin" "Aereo.java" "Aereoporto.java" "Mezzo.java" "Pista.java"

cd "$pathJavaBin"

timeout $2 java Aereoporto > "$curPath/$fileout"

cd "$curPath"

case $modo in
d) echo "`grep -w decollo $fileout`";;
a) echo "`grep -w atterraggio $fileout`";;
esac

exit 0

#oppure

java Aereoporto > "$curPath/$1" &

pidF=`ps | grep java -m 1 | cut -d' ' -f3`




sleep $timeout

kill $pidF
#o kill -9

#filtro contenuto fileOut

cd "$curPath"

case $modo in
d) echo "`grep -w decollo $fileout`";;
a) echo "`grep -w atterraggio $fileout`";;
esac

exit 0

#vado a vedere il pid del processo figlio

#timeout -k 3s `java Aereoporto &`

#timeout --kill-after="$curPath$2" `

#timeout --foreground -k "4s" `java "Aereoporto" > "$curPath/$1"`
#timeout --kill-after="$2" `java "Aereoporto" > "$curPath/$1"`



