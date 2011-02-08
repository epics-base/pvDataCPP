#!/bin/sh

SELF="`basename "$0"`"
TEST="${SELF%.sh}"

if [ -z "$EPICS_HOST_ARCH" ]; then
    echo "Please set EPICS_HOST_ARCH and run again"
    exit 1
fi

rm -f $TEST ${TEST}Diff ${TEST}Aux

if ../bin/${EPICS_HOST_ARCH}/$TEST $TEST ${TEST}Aux 2>&1 >/dev/null
then
    printf "" # OK
else
    printf "$TEST Failed to complete\n"
fi

if [ -e $TEST -a -e ${TEST}Gold ]
then
    diff -u ${TEST}Gold $TEST >> ${TEST}Diff
    if [ -s "${TEST}Diff" ]
    then
        printf "$TEST Does not match expectations\n"
        cat ${TEST}Diff
        printf "\n\n"
    else
        printf "$TEST OK\n"
    fi
else
    printf "$TEST OK\n"
fi
