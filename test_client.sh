#!/bin/bash
echo -n "$1" | nc -u -w1 localhost 9000
echo "Sent IMSI: $1, response: $?"



