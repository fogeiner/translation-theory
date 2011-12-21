#!/bin/bash 

APP=./main

SUCCESS=0
FAIL=0

make clean && make

if [ ! -x ${APP} ] ; then
	echo "${APP} not found";
	exit 1;
fi

for i in tests/*.sc ; do
	echo '========== Running test ' "$i" ==========
	${APP} "$i"
	if [ "X$?" = "X0" ] ; then
		echo "Ok";
		let SUCCESS=$(($SUCCESS+1))
	else 
		echo "Failed";
		let FAIL=$(($FAIL+1))
	fi
done

echo 'Total tests ' $(($SUCCESS + $FAIL))
echo 'Success ' $SUCCESS
echo 'Fail ' $FAIL
