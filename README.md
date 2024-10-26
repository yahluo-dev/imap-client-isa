# imapcl -- IMAP Client with TLS Support

## Introduction

**imapcl** is an IMAP4rev1[RFC3501] client that I am working on as an assignment in my network applications and administration course. Besides plain-text communication, it also supports communication secured by TLS with the help of the OpenSSL library.

### Theory

**IMAP (Internet Mail Access Protocol)** is a protocol used to access messages stored on the IMAP server. In this program, its basic client-side functionality is implemented in accordance with RFC3501. 

#### TLS

**TLS (Transport Layer Security)** is a cryptographic protocol introduced by the IETF (Internet Engineering Task Force)[RFC8446] that is designed to improve upon _SSL (Secure Sockets layer)_.

## Requirements

### imapcl

- make

- gcc

- OpenSSL

### Tests

- googletest

## Usage

```shell
imapcl SERVER [-p port] [-T [-c certfile] [-C certaddr]] [-n] [-h] -a auth_file [-b MAILBOX] -o out_dir
```

| Option           | Description                                                          |
|------------------|:---------------------------------------------------------------------|
| **-p** PORT      | Port to connect to. Default is 993 for IMAPS and 143 for plain IMAP. |
| **-T**           | Enables encryption (use IMAPS)                                       |
| **-c** FILE      | Specify certificate file                                             |
| **-C** DIR       | Specify certificate directory (default /etc/ssl/certs)               |
| **-n**           | Only see new messages                                                |
| **-h**           | Only fetch headers                                                   |
| **-a** AUTH_FILE | File with login credentials to use                                   |
| **-b** MAILBOX   | Mailbox to use on the server (default INBOX)                         |
| **-o** OUT_DIR   | Directory to store downloaded messages"                              |


