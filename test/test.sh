#!/bin/bash

runAdess() {
	./build/adess $1 | grep -F "[DEBUG]" | tr -d '[:space:]' | tr -d "[DEBUG]" 
}

testAdess() {
	input=$(runAdess "$1")

	if [ "$input" = "$4" ]; then
		echo "passed $1 $2 $3"
	else
		echo "failed $1 $2 $3"
		echo ""
		echo "Output:"
		./build/adess $1
		echo ""
		echo "For input: $1 $2 $3"
		echo ""
		echo "Expected: $4"
		echo "Recieved: $input"
	fi
}

# Nothing at all
testAdess "" "" "" "command:helpargument:(null)options:(null)"

# Help command
testAdess "help" "" "" "command:helpargument:(null)options:(null)"

# Help command
