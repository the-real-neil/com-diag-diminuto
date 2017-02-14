#!/bin/bash

ROOT=/home/pi
export COM_DIAG_DIMINUTO_ROOT=${ROOT}/src/com-diag-diminuto/Diminuto/out/host/bin
export PATH=$PATH:$COM_DIAG_DIMINUTO_ROOT/../sym:$COM_DIAG_DIMINUTO_ROOT/../bin:$COM_DIAG_DIMINUTO_ROOT/../tst

pintool -p 16 -n -x -o
pintool -p 20 -n -x -o
pintool -p 21 -n -x -o

pintool -p 16 -s
sleep 1
pintool -p 20 -s
sleep 1
pintool -p 21 -s
sleep 1
pintool -p 16 -c
sleep 1
pintool -p 20 -c
sleep 1
pintool -p 21 -c
sleep 1

pintool -p 22 -n -x -i -L
pintool -p 27 -n -x -i -H

VALUE=0
stdbuf -o0 pintool -p 27 -b 10000 | stdbuf -o0 tee /dev/tty | while read PIN27; do
	if [[ ${VALUE} -eq 1 ]]; then
		VALUE=$((${VALUE} + 1))
		TEMP=${VALUE}
		PIN16=$((${TEMP} % 2))
		TEMP=$((${TEMP} / 2))
		PIN20=$((${TEMP} % 2))
		TEMP=$((${TEMP} / 2))
		PIN21=$((${TEMP} % 2))
		pintool -p 16 -w ${PIN16}
		pintool -p 20 -w ${PIN20}
		pintool -p 21 -w ${PIN21}
		echo ${PIN27} ${VALUE} ${PIN16} ${PIN20} ${PIN21}
	fi
done
