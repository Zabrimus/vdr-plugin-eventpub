#!/bin/bash

# get all arguments
if [[ "$#" -ne 4 ]]; then
   echo "Exactly 4 arguments are needed (time, subject, length, message)"
   exit 1
fi

TIME=$1
SUBJECT=$2
MSGLEN=$3
MESSAGE=$4

# Log only incoming data
echo "Time: ${TIME}" >> Script_Output
echo "Subject: ${SUBJECT}" >> Script_Output
echo "Len: ${MSGLEN}" >> Script_Output
echo "MESSAGE: ${MESSAGE}" >> Script_Output
echo >> Script_Output
