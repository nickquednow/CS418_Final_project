#!/bin/bash

PYTHON_EXECUTABLE_DIRECTORY=/usr/bin/python3
TESTING_DIRECTORY=.


config_toml_head=$(head -n 1 "$TESTING_DIRECTORY/config.toml")

if test -f "$TESTING_DIRECTORY/fail.txt"; then
    rm "$TESTING_DIRECTORY/fail.txt"
fi

if [ "$config_toml_head" == "[fail]" ]
then
    echo "please read README, then try again"
    exit
fi

echo "executing the simple test command. makes sure the webserver is running correctly"
$PYTHON_EXECUTABLE_DIRECTORY "$TESTING_DIRECTORY/test_webserver.py"

if test -f "$TESTING_DIRECTORY/fail.txt"; then
    cat "$TESTING_DIRECTORY/fail.txt"
    exit
fi