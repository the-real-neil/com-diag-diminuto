#!/bin/bash
# Copyright 2018 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto

# This is built for a specific test fixture I breadboarded up.

. $(readlink -e $(dirname ${0})/../bin)/setup

PGM=$(basename $0)
PIN=${1:-16}
SEC=${2:-3}
DUTY=0;
PCT=100;
DIR="tmp"

mkdir -p ${DIR}

while [[ ${DUTY} -le ${PCT} ]]; do
	strace -o ${DIR}/${PGM}-${DUTY} -f pwmthread ${PIN} ${DUTY} &
	PID=$!
	trap "kill ${PID}; exit" 1 2 3 15
	echo ${PGM}: ${PIN} ${SEC} ${DUTY} ${PID} 1>&2
	sleep ${SEC}
	kill -15 ${PID}
	wait ${PID}
	DUTY=$((${DUTY} + 1))
done

pintool -p ${PIN} -n

exit 0
