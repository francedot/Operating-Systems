#!/bin/sh

# esercitazione6.sh 

if test $# != 2
then echo Errore nei parametri
        exit
fi

echo ciao
max=0
filemax=NULL

#for i in *  per ciclare su tutti i file del direttorio corrente
for i in $1*  # per ciclare su tutti i file del direttorio passato come parametro
do
echo i=$i
if test -r $i
        then
                occ=`cat $i | grep -o $2|wc -l`
                echo $occ
                if [ $occ -gt $max ]
                then 
                        max=$occ
                        filemax=$i
                fi
        fi

done
echo "RISULTATO: il massimo numero di occorrenze di $2 è nel file $filemax (trovate $max occorrenze)"
