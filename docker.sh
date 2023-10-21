#!/bin/bash

IMAGENAME="ft_ping"
SCRIPTPATH="$( cd "$(dirname "$0")"  >/dev/null 2>&1; pwd -P )"

if [[ "$(docker images -q $IMAGENAME 2> /dev/null)" == "" ]]; then
	docker build -t "$IMAGENAME" "$SCRIPTPATH"
	if [ $? -ne 0 ]; then
        exit 1
    fi
fi

docker run --security-opt "apparmor=unconfined" --security-opt "seccomp=unconfined" \
    -it --mount type=bind,source="$SCRIPTPATH",target=/root/$IMAGENAME $IMAGENAME
    