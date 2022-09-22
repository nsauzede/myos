#!/bin/sh

if type git >/dev/null 2>&1 ; then
    t=`git describe --tags 2> /dev/null` && echo "-DMYOS_TAG=$t"
fi
