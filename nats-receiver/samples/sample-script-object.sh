#!/bin/bash

# get all arguments
if [[ "$#" -ne 2 ]]; then
   echo "Exactly 2 arguments are needed (time, filename)"
   exit 1
fi

TIME=$1
FILENAME=$2

# Log only incoming data
echo "Time: ${TIME}" >> Script_Output
echo "Filename: ${FILENAME}" >> Script_Output
echo >> Script_Output
