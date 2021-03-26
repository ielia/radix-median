#!/bin/bash
reset
make
# export LOGLEVEL=LOGDEBUG
export LOGLEVEL=LOGINFO
# export LOGLEVEL=LOGTEST
(echo "704"; seq 500000 -5 400000; seq 100000 -5 -100000; seq 200005 5 399995; seq 100005 5 200000) | ./median
