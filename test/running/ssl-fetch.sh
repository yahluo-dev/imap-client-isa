#!/bin/bash

HOST=localhost
CERT=/etc/ssl/certs/mailcert.pem
CERTDIR=./certs
PORT=993
AUTHFILE=./test-credentials
MAILBOX=INBOX
OUTDIR=./test-outdir

./imapcl $HOST -T -c $CERT -p $PORT -a $AUTHFILE -b $MAILBOX -o $OUTDIR  # Full messages

./imapcl $HOST -T -c $CERT -p $PORT -a $AUTHFILE -b $MAILBOX -o $OUTDIR -h # Only headers

./imapcl $HOST -T -c $CERT -a $AUTHFILE -b $MAILBOX -o $OUTDIR # Default port for IMAPS

./imapcl $HOST -T -C $CERTDIR -p $PORT -a $AUTHFILE -b $MAILBOX -o $OUTDIR # Cert dir
