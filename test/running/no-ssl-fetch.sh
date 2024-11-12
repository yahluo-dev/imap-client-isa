#!/bin/bash

HOST=localhost
CERT=/etc/ssl/certs/mailcert.pem
PORT=143
AUTHFILE=./test-credentials
MAILBOX=INBOX
OUTDIR=./test-outdir

./imapcl $HOST -p $PORT -a $AUTHFILE -b $MAILBOX -o $OUTDIR  # Full messages

./imapcl $HOST -p $PORT -a $AUTHFILE -b $MAILBOX -o $OUTDIR -h # Only headers

./imapcl $HOST -a $AUTHFILE -b $MAILBOX -o $OUTDIR # Default port for IMAP

