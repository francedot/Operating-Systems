#inverto il file
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
		occ=`grep -c $2 invL`
		rm invL
		#echo $occ
		numeroOccorrenzeStringa=`expr $numeroOccorrenzeStringa + $occ`
		echo "occorrenze: $numeroOccorrenzeStringa"
		echo "indice: $index"
	fi
	index=`expr $index + 1`
done < $f
#f=file da iterare

index=0
while read line
do
	# do something with $line here
done < $f
