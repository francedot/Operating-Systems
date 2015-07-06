
if ! test -d "$3"
then
	echo "Argomento 3 non rappresenta una cartella!"
	exit 2
fi

if ! test -f $1
then
	echo "log deve essere un file"
	exit 2
fi
