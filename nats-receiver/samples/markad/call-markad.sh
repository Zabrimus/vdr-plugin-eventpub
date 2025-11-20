#!/bin/bash

# sample call: /usr/local/bin/nats-receiver --server=nats://127.0.0.1:4222 --stream=vdrevents --subject="vdr.recording.stop.*" --script="/usr/local/bin/call-markad.sh" --durable=markad
#
# or using nats
# nats publish -J "vdr.recording.stop.vdr-client-1" "/video/Mein_Film/2025-10-15.20.09.10-0.rec"

if [[ "$#" -ne 4 ]]; then
   logger "[call-markad] Exactly 4 arguments are needed (time, subject, length, message)"
   exit 1
fi

# get all arguments
TIME=$1
SUBJECT=$2
MSGLEN=$3
MESSAGE=$4

# check if this is a video stop message
if [[ ! "${SUBJECT}" =~ ^vdr.recording.stop.*$ ]]; then
    logger "[call-markad] Subject ${SUBJECT} is not allowed. Exit."
    exit 0
fi

# get VDR name
subjectIN=(${SUBJECT//./ })
VDR=${subjectIN[-1]}

# transform video path to real path
# This is definitly system dependant and needs to be changed - if necessary
if [ "${VDR}" = "vdr-client-1" ]; then
    VIDEOPATH=$(echo ${MESSAGE} | sed -e "s#/video/#/video/client1/#")
elif [ "${VDR}" = "vdr-client-2" ]; then
    VIDEOPATH=$(echo ${MESSAGE} | sed -e "s#/video/#/video/client2/#")
fi

# check if video exists
if [ ! -d ${VIDEOPATH} ]; then
    logger "[call-markad] Recording ${VIDEOPATH} does not exists. Exit."
    exit 0
fi

# check if marks already exists
if [ -f ${VIDEOPATH}/marks ]; then
    logger "[call-markad] Marks for recording ${VIDEOPATH} already exists. Exit."
    exit 0
fi

# Call markad
logger "[call-markad] /usr/local/bin/markad --hwaccel=vaapi --log2rec - ${VIDEOPATH}"
/usr/local/bin/markad --hwaccel=vaapi --log2rec - ${VIDEOPATH}
logger "[call-markad] Finished ${VIDEOPATH}"
