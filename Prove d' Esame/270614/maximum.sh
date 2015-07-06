#! /bin/bash

tmpPath="tmp"

#2. Esercizio di Programmazione Shell
#Si realizzi un file comandi Unix con la seguente interfaccia:
#maximum <gruppo> <string> <dir>
#dove:
#• gruppo e string sono stringhe;
#• dir è il nome assoluto di un direttorio esistente nel file system
#Dopo aver effettuato tutti gli opportuni controlli sui parametri in ingresso, il file
#comandi deve cercare tutti i file che abbiano come group_id la stringa <gruppo>
#all'interno della gerarchia individuata dal direttorio <dir> (cioè ispezionando
#ricorsivamente anche tutti i sottodirettori di <dir>).
#Per ogni file individuato, lo script deve analizzarne il contenuto e contare il numero
#di occorrenze della stringa <string>.
#Al termine, il file comandi dovrà stampare a video una riga del tipo:
#nomefile MAX
#dove MAX è il numero massimo di occorrenze per file di <string> trovato tra tutti
#i file analizzati, e nomefile è il nome assoluto del file analizzato con il massimo
#numero di occorrenze di <string>.
#riga 20

#N.B nel mio caso ho unico group=franncesco con groupid=1000
#N.B [ ] di test vogliono sempre lo spazio tra gli argomenti iniziale e finale

#controllo sul numero di argomenti
if [ $# -ne 3 ]
then
	printf "Numero di argomenti sbagliato! $#\n"
	exit 1
fi

#sarebbe opportuno anche un test sull'esistenza di quel groupId. Sò farlo solo sul mio groupId
if [ $1 -ne "`id -g`" ]
then 
	printf "$1 non è l'id del mio gruppo!\n"
	#cmq non esco
fi

#controllo se terzo argomento è una cartella
if ! test -d "$3"
then
	printf "Argomento 3 non rappresenta una cartella!\n"
	exit 2
fi

#effettuo lo script di ricerca all'interno della cartella datami!
#sono sicuro che sia una cartella quindi:

#serve aggiornamento della variabile PATH al fine di lanciare correttamente ad ogni iterazione
#lo script ricorsivo che per ipotesi ho in ~/Desktop/
oldpath=$PATH
PATH=$PATH:`pwd`
printf "PATH Aggiornato: $PATH\n"

#mi appoggio ad un file temporaneo per scrivere i nomi assoluti con il numero delle occorrenze
#N.B > per creazione file vuole path relativo e non assoluto!!!
>"$tmpPath"
chmod 00777 "$tmpPath"

recursiveMaximum.sh "$1" "$2" "$3" "`pwd`/$tmpPath"

PATH=$oldpath

#stampo la riga nomefile MAX
#ha senso solo se file temp non vuoto! --> controlla!
filelines="`cat $tmpPath`"
if test "$filelines"
then
	rigamax=0
	max=0
	i=0
	for line in $filelines
	do
		k=`expr $i % 2`
		if [ $k -eq 0 ]
		then
			if [ $line -gt $max ]
			then
				max=$line
				rigamax=$i
			fi
		else
			j=`expr $rigamax + 1`
			if [ $i -eq $j ]
			then
				nomefile=$line
			fi
		fi
		i=`expr $i + 1`
	done

	if [ $max -eq 0 ]
	then
		printf "Nessuna occorrenza trovata\n"
		exit 0
	else
		printf "$nomefile $max\n"
	fi
else
	printf "Nessun file appartenente a quel group id è stato trovato!\n"
fi

unlink "$tmpPath"

exit 0