#! /bin/bash

#mi sposto in questa cartella! N.B: controlla quale parametro ti ha dato ricerca.sh!
cd $1

echo "Sono entrato in "`pwd`""

#mi sono spostato in un nuovo sottodirettorio quindi creo il file di report
>report.txt
chmod 00777 report.txt

if test "`ls`"
then
	for f in *
	do
		if test -f $f
		then
			#inverto il file
			>invF
			chmod 00777 invF
			rev $f > invF
			numeroOccorrenzeStringa=0
			index=0
			while read line
			do
				# do something with $line here
				#echo "Letta $line"
				if [ $index -lt 10 ]
				then	
					>invL
					chmod 00777 invL
					echo "$line" >> invL
					cat invL
					occ=`grep -o $2 invL | wc -l`
					rm invL
					#echo $occ
					numeroOccorrenzeStringa=`expr $numeroOccorrenzeStringa + $occ`
					echo "occorrenze: $numeroOccorrenzeStringa"
					echo "indice: $index"
				fi
				index=`expr $index + 1`
			done <invF
			rm invF

			echo "file: $f"
			echo "occorrenze finali: $numeroOccorrenzeStringa"
			if [ $numeroOccorrenzeStringa -gt 0 ]
			then	
				nomeassoluto_file="`pwd`/$f"
				echo ""$nomeassoluto_file" $numeroOccorrenzeStringa"
			fi
			
			#echo $f

			
		elif test -d $f
		then
			#è una directory
			echo "Mi trovo davanti la cartella $f!"
			#rilancio lo script. N,B controlla i parametri passati da ricerca.sh
			#e quelli da passare!
			"$0" "$f" "$2"
		fi
	done
else
	echo "Cartella $3 vuota"
	#torno indietro nella ricorsione
	exit 0
fi

#non mettere exit 0 altrimenti ti esce dallo script alla prima cartella vuota!
