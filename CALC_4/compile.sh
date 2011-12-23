#!/bin/sh -x

if [ -z "$1" -o -z "$2" ] ; then
	echo "Usage $0 sc-file out-file"
	exit 1
fi

COMPILER=./main
TMP_PATH=/tmp/src.s

if [ ! -x ${COMPILER} ] ; then
	echo "${COMPILER} not found!";
	exit 1;
fi

${COMPILER} "$1" > ${TMP_PATH}
if [ "X$?" != "X0" ] ; then
	echo 'Translation failed'
fi
gcc -m32 -o "$2" ${TMP_PATH}

if [ "X$?" != "X0" ] ; then
	echo 'Compilation failed'
fi
