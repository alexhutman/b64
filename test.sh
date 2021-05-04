#!/bin/bash

err_and_exit() {
	echo "$1" 1>&2
	exit 1
}

if ! gcc b64.c -o b64; then
	err_and_exit "Compilation failed."
fi

TEST_STR="Hello there, this is a test :)"
INTENDED="$(printf $TEST_STR | base64)" 

RES="$(printf $TEST_STR | ./b64)" 
if [ "$INTENDED" = "$RES" ]; then
	echo "Stdin test succeeded!"
else
	err_and_exit "Stdin test failed! Received '$RES' instead of '$INTENDED'"
fi

RES="$(./b64 <(printf $TEST_STR))" 
if [ "$INTENDED" = "$RES" ]; then
	echo "File test succeeded!"
else
	err_and_exit "File test failed! Received '$RES' instead of '$INTENDED'"
fi

echo "All tests successful!"
