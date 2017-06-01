#!/bin/bash

ssh-add ./ucla-aiaa-uas-raspi

mkdir captures
REMOTE="pi@192.168.1.2"
CAPTURE_DIRECTORY="~/vision/captures/segments"
LATEST_DIRECTORY=$(ssh $REMOTE ls $CAPTURE_DIRECTORY | tail -1)
LATEST_DIRECTORY="$CAPTURE_DIRECTORY/$LATEST_DIRECTORY"
LATEST_DIRECTORY="$LATEST_DIRECTORY"
echo "Downloading from $REMOTE:$LATEST_DIRECTORY"

while [ 1 ]
do
    rsync -avz --partial $REMOTE:$LATEST_DIRECTORY ./captures
    if [ "$?" = "0" ] ; then
        echo "Directory up-to-date."
    else
        echo "Rsync failure. Backing off and retrying..."
    fi
    sleep 1
done
