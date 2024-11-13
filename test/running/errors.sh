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

# Interactive

./imapcl $HOST -o $OUTDIR -a $CREDSOK -i <(echo "NOT AN ACTUAL COMMAND") # Invalid command

# Unexpected data from server

./imapcl $HOST -o $OUTDIR -a $CREDSOK -T -p 143 # Trying to connect to an IMAP port with TLS

./imapcl $HOST -o $OUTDIR -a $CREDSOK -p 993 # Trying to connect to an IMAPS port without TLS
