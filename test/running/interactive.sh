#!/bin/bash

HOST=localhost
CERT=/etc/ssl/certs/mailcert.pem
CERTDIR=./certs
PORT=993
AUTHFILE=./test-credentials
MAILBOX=INBOX
OUTDIR=./test-outdir

./imapcl $HOST -a $AUTHFILE -b $MAILBOX -o $OUTDIR -i <(echo "DOWNLOADALL") # No TLS

./imapcl $HOST -T -c $CERT -a $AUTHFILE -b $MAILBOX -o $OUTDIR -i <(echo "DOWNLOADALL") # TLS


./imapcl $HOST -a $AUTHFILE -b $MAILBOX -o $OUTDIR -i <(echo -e "READNEW")

./imapcl $HOST -a $AUTHFILE -b $MAILBOX -o $OUTDIR -i <(echo -e "DOWNLOADNEW")

./imapcl $HOST -a $AUTHFILE -b $MAILBOX -o $OUTDIR -i <(echo "QUIT")

