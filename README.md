# imapcl -- IMAP Client with TLS Support

Author: *Iaroslav Vasilevskii* (xvasil10)

Made as an assignment part of ISA in winter semester of 2024/25

Final version of the project submitted on 18.11.2024

## Introduction

**imapcl** is an IMAP4rev1[RFC3501] client able to fetch messages from a server. Besides plain-text communication, it also supports communication secured by TLS with the help of the OpenSSL library.

*More detailed information about the project is available in manual.pdf*

*Extensions:*
- Interactive mode


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
imapcl SERVER [-p port] [-T [-c certfile] [-C certdir]] [-n] [-h] -a auth_file [-b MAILBOX] -o out_dir
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

## File list

- *Makefile* - Makefile managing the build process
- *receiver.hpp, receiver.cpp* - Class implementing asynchronous receipt of messages
- *tls_receiver.hpp, tls_receiver.cpp* - A subclass of Receiver supporting TLS
- *command.hpp, command.cpp* - Command class representing commands sent by the user
- *fnv.hpp, fnv.cpp* - Fowler-Noll-Vo\cite{eastlake-fnv-29} hashing function class for creating unique filenames
- *logger.hpp, logger.cpp* - Class implementing logging capabilities with adjustable logging level
- *main.hpp, main.cpp* - Main function and argument parsing
- *parser_logger.hpp, parser_logger.cpp* - Logger subclass for the recursive descent parser
- *response.hpp, response.cpp* - Response class representing messages sent by the server
- *response_parser.hpp, response_parser.cpp* - Class for parsing server response data
- *server.hpp, server.cpp* - Class initializing the socket and managing message sending
- *tls_server.hpp, tls_server.cpp* - Server subclass adding SSL/TLS initialization and management
- *session.hpp, session.cpp* - Class managing user actions on a high level
- *client.hpp, client.cpp* - Class implementing a REPL for interacting with the user.
- *imf_message.hpp, imf_message.cpp* - A class implementing parsing of some components of a message in IMF format.
- *directory_writer.cpp, directory_writer.hpp* - Class for writing messages to an output directory with an appropriate file name
- *test/* - Directory with test files

### Unit tests

*test/unit/make_tcp.cpp* - make\_tcp() tests for Command and its subclasses
*test/unit/parse_response.cpp* - Tests for the response parser
*test/unit/imf_message.cpp* - Tests for the IMFMessage class
*test/unit/test_main.cpp* - Initialization of the Google Test framework

### System tests

*test/running/test\_main.sh* - Main test script, runs the program with specified arguments and input, and compares it against what was expected.
*test/running/tmp/* - Directory for temporary test files
*test/running/expected/* - Directory with expected stderr and stdout outputs

## Limitations

- Interactive mode does not show messages summary in the beginning.
- Argument parsing might fail when `POSIXLY_CORRECT` is set and non-flag arguments are *not* at the very end.
- When the program finishes and the destructor of `Session` is called, there is an inevitable delay of 1 second before the receiver thread times out, leaves uninterruptible sleep, and is joined.
- The time the program has to wait to conclude that the server does not respond depends on the number of entries returned by `getaddrinfo()`.
- System tests are rudimentary and assume a certain server state.
- The parser does not implement the entirety of the grammar. For example, it does not support message continuation requests.
- The receiver has a fixed-size buffer for received data.
