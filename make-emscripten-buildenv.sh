#!/bin/bash

IMAGE=amarillion/alleg5-emscripten:5.2.10.1
SRCDIR=$(dirname $(readlink -f $0))
docker run \
	-u $(id -u):$(id -g) \
	-v $SRCDIR:/data \
	$IMAGE \
	make TARGET=EMSCRIPTEN BUILD=DEBUG "$@"
