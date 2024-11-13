# Inadequate commandline arguments

CREDSOK=./test-credentials
CREDSBAD=./incorrect-test-credentials
CREDSINVALID=./invalid-file-test-credentials
HOST=localhost

./imapcl # No hostname, no outdir, no auth file
./imapcl $HOST # No outdir, no auth file
./imapcl $HOST -o $OUTDIR # No auth file

./imapcl $HOST -o $OUTDIR -a $CREDSOK -p 100000000000 # Port number too large
./imapcl $HOST -o $OUTDIR -a $CREDSOK

# Inadequate combinations of commandline arguments
