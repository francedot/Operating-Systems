#! /bin/bash

#controllo argomenti --> vedi altri snippet

#serve aggiornamento della variabile PATH al fine di lanciare correttamente ad ogni iterazione
#lo script ricorsivo che per ipotesi ho in ~/Desktop/
oldpath=$PATH
PATH=$PATH:`pwd`
echo "PATH Aggiornato: $PATH"

#lancio lo script ricorsivo passando tot parametri
recursiveRicerca.sh "$1" "$2" "$3"

PATH=$oldpath

#tutto andato a buon fine
exit 0
