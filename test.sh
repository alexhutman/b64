#!/bin/sh

err_and_exit() {
	echo "$1" 1>&2
	exit 1
}
if ! gcc b64.c -o b64; then
	err_and_exit "Compilation failed."
fi

TEST_STR="hi!"

INTENDED="$(printf $TEST_STR | base64)" 
RES="$(./b64 test.sh)" 

if [ "$INTENDED" != "$RES" ]; then
	err_and_exit "Test failed! Received '$RES' instead of '$INTENDED'"
fi

echo "Test successful!"
