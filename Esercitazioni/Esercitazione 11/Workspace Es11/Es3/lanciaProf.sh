#!/bin/bash
#Usage: ./lancia.sh fileout timeout tipo_op

#Controllo il numero dei parametri
if [ $# -ne 3 ]; then
	echo "Usage: ./lancia.sh fileout timeout tipo_op"
	exit
fi

fileout=$1
>$fileout

#Controllo che il file sia scrivibile
if [[ ! -w "$fileout" ]]; then
	echo "$timeout is not writable"
	exit
fi

timeout=$2

#Controllo che il timeout sia un numero intero
if [[ "$timeout" == *[!0-9]* ]]; then
	echo "$timeout is not a number"
	exit
fi

tipo_op=$3


if [[ $tipo_op != 'd' ]] && [[ $tipo_op != 'a' ]]; then
	echo "$tipo_op not correct"
	exit
fi

#Genero il figlio tramite la & in modo da permettere al padre di proseguire
java -jar Aeroporto.jar > "$fileout" &


#Interrompo l'esecuzione del padre per $timeout secondi
sleep $timeout

#Mi salvo il pid del figlio facendo il parsing del comando ps
pid_figlio=`ps | grep "Aeroporto.jar" -m 1 | cut -d' ' -f1`
#Uccido il processo figlio
kill -9 $pid_figlio

#Stampo le linee corrispondenti
if [[ $tipo_op == 'd' ]] ; then
	cat $fileout | grep '^decollato'
else
	cat $fileout | grep '^atterrato'
fi

exit


