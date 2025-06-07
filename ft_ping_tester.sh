#!/bin/bash

FT_PING="sudo ./ft_ping"
SYS_PING=$(command -v ping)
TIMEOUT_DURATION=5

test_cases=(
    "8.8.8.8"
    "192.0.2.123"
    "-v 192.0.2.123"
    "google.com"
    "notarealhost.tld"
    "-v notarealhost.tld"
    "-?"
    "--help"
    "-h"
    "-c 3 8.8.8.8"
    "-i 2 -c 3 8.8.8.8"
    "-s 64 -c 2 8.8.8.8"
    "-n 8.8.8.8"
    "-c 5 -i 1 -s 56 8.8.8.8"
    "-v -i 1 -c 4 google.com"
    "-? -h --help"
    "-c 0 8.8.8.8"
    "-i 0 -c 3 8.8.8.8"
    "-z 8.8.8.8"
    "8.8.8.8 extra_arg"
    "-c 3"
)

run_test() {
    local args="$1"
    echo "=== Testing: $args ==="

    echo "--- ft_ping ---"
    sudo timeout $TIMEOUT_DURATION $FT_PING $args 2>&1

    echo "--- system ping ---"
    timeout $TIMEOUT_DURATION $SYS_PING $args 2>&1

    echo "=============================="
    echo
}

for case in "${test_cases[@]}"; do
    run_test "$case"
done
