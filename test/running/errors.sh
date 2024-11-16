#!/bin/bash
IMAPCL=./imapcl
TESTSTMPDIR=./test/running/tmp
TESTSEXPDIR=./test/running/expected
CREDSOK=./test-credentials
CREDSBAD=./incorrect-test-credentials
CREDSINVALID=./invalid-file-test-credentials
HOST=localhost

# USAGE=$(printf "%s", "$USAGE") # Resolve all the escape sequences

run-test()
{
    local test_name="$1"
    local test_args_file="$TESTSEXPDIR/$(echo $test_name)_cmd.txt"
    local test_stdin_file="$TESTSEXPDIR/$(echo $test_name)_stdin.txt"
    local expected_stderr_file="$TESTSEXPDIR/$(echo $test_name)_stderr.txt"
    local expected_stdout_file="$TESTSEXPDIR/$(echo $test_name)_stdout.txt"

    local test_args_file
    local expected_stderr
    local expected_stdout

    test_args="$(cat $test_args_file)"
    expected_stderr="$(cat $expected_stderr_file)"
    expected_stdout="$(cat $expected_stdout_file)"
    echo $test_args;

    if [ -f "$test_stdin_file" ]; then
        $IMAPCL $test_args 2>$TESTSTMPDIR/$(echo "$test_name")_stderr.txt 1>$TESTSTMPDIR/$(echo "$test_name")_stdout.txt <(cat "$test_stdin_file")
    else
        $IMAPCL $test_args 2>$TESTSTMPDIR/$(echo "$test_name")_stderr.txt 1>$TESTSTMPDIR/$(echo "$test_name")_stdout.txt
    fi

    local actual_stderr=$(cat $TESTSTMPDIR/$(echo "$test_name")_stderr.txt)
    local actual_stdout=$(cat $TESTSTMPDIR/$(echo "$test_name")_stdout.txt)

    actual_stderr=$(echo -e "$actual_stderr")
    actual_stdout=$(echo -e "$actual_stdout")
    local result=0;
    if [ "$actual_stdout" != "$expected_stdout" ]; then
        echo "$test_name: NOT OK (stdout):"
        diff <(echo "$expected_stdout") <(echo "$actual_stdout")
        result=1;
    fi
    if [ "$actual_stderr" != "$expected_stderr" ]; then
        echo "$test_name: NOT OK (stderr):"
        diff <(echo "$expected_stderr") <(echo "$actual_stderr")
    fi
    if [ $result == 0 ]; then
        echo "$test_name: OK"
    fi
}

main()
{
    mkdir test_outdir 2>/dev/null
    echo -e "username = testuser\npassword = testpwd" > test_credentials
    # No hostname
    run-test "bad-commandline-1" # No arguments
    run-test "bad-commandline-2" # No output directory
    run-test "bad-commandline-3" # No auth file
    run-test "bad-commandline-4" # Port number too large
    run-test "bad-commandline-5" # Output directory does not exist
    run-test "interactive-1" # Nonexistent command
}
# Interactive

#./imapcl $HOST -o $OUTDIR -a $CREDSOK -i <(echo "NOT AN ACTUAL COMMAND") # Invalid command

# Unexpected data from server

#./imapcl $HOST -o $OUTDIR -a $CREDSOK -T -p 143 # Trying to connect to an IMAP port with TLS

#./imapcl $HOST -o $OUTDIR -a $CREDSOK -p 993 # Trying to connect to an IMAPS port without TLS

main
