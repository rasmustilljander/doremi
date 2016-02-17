#! /bin/bash

cd $SSP_CODE_HOME
./tools/bin/cppcheck -q -I . --enable=all --inconclusive . 2> .staticTestCache
resTmp=`mktemp`
cat .staticTestCache > $resTmp
`cat $resTmp | tr '\\' '/' > .staticTestCache 2>/dev/null`
unlink $resTmp