/**
 * @file main.hpp
 * @brief Main function and argument parsing
 * @author Iaroslav Vasilevskii (xvasil10)
 */
#ifndef MAIN_H
#define MAIN_H

const char *USAGE = "Usage:\n"
                    "imapcl SERVER [-p port] [-T [-c certfile] [-C certaddr]] [-n] [-h] -a auth_file [-b MAILBOX] -o out_dir\n"
                    "\t-p PORT\t Port to connect to. Default is 993 for IMAPS and 143 for plain IMAP.\n"
                    "\t-T\t Enables encryption (use IMAPS)\n"
                    "\t-c FILE\t Specify certificate file\n"
                    "\t-C DIR\t Specify certificate directory (default /etc/ssl/certs)\n"
                    "\t-n\t Only see new messages\n"
                    "\t-h\t Only fetch headers\n"
                    "\t-a AUTH_FILE\t File with login credentials to use\n"
                    "\t-b MAILBOX\t Mailbox to use on the server (default INBOX)\n"
                    "\t-o OUT_DIR\t Directory to store downloaded messages\n"
                    "\t-i\tStart the program in interactive mode";

#endif // MAIN_H
