# imapcl -- IMAP Client with TLS Support

Author: *Iaroslav Vasilevskii* (xvasil10)
Made as an assignment part of ISA in winter semester of 2024/25

## Introduction

**imapcl** is an IMAP4rev1[RFC3501] client that I am working on as an assignment in my network applications and administration course. Besides plain-text communication, it also supports communication secured by TLS with the help of the OpenSSL library.

*More detailed information about the project is available in manual.pdf*

*Extensions:*
- Interactive mode

*File list*

- *Makefile* - Makefile managing the build process
- *receiver.hpp*, *receiver.cpp* - Class implementing asynchronous receipt of messages
- *tls_receiver.hpp*, *tls_receiver.cpp* - A subclass of \texttt{Receiver} supporting TLS
- *command.hpp*, *command.cpp* - Command class representing commands sent by the user
- _fnv.hpp_, _fnv.cpp_ - Fowler-Noll-Vo\cite{eastlake-fnv-29} hashing function class for creating unique filenames
- _logger.hpp_, _logger.cpp_ - Class implementing logging capabilities with adjustable logging level
- _main.hpp_, _main.cpp_ - Main function and argument parsing
- *parser_logger.hpp*, *parser_logger.cpp* - Logger subclass for the recursive descent parser
- _response.hpp_, _response.cpp_ - Response class representing messages sent by the server
- *response_parser.hpp*, *response_parser.cpp* - Class for parsing server response data
- *server.hpp*, _server.cpp_ - Class initializing the socket and managing message sending
- *tls_server.hpp*, *tls_server.cpp* - Server subclass adding SSL/TLS initialization and management
- _session.hpp_, _session.cpp_ - Class managing user actions on a high level
- _client.hpp_, _client.cpp_ - A class implementing a REPL to interact with the user in interactive mode.
- *imf_message.hpp*, *imf_message.cpp* - A class implementing parsing of some components of a message in IMF format.
- *test/* - Directory with test files, further documented in \textit{Testing}.

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
| **-i**           | Start the program in interactive mode                                |

### Example usage

``` shell
# ./imapcl 127.0.0.1 -p 143 -a credentials -b Important -h -o outdir/
* Fetched 5 messages (headers).

# ./imapcl 127.0.0.1 -p 143 -a credentials -b Important -o outdir/
* Fetched 5 messages.

# ./imapcl 127.0.0.1 -p 143 -a credentials -b Important -n -o outdir/
* Fetched 1 unseen messages.
```
