#! /bin/bash
cd $SSP_CODE_HOME

for entry in $(cat .formatCache)
do
	file=$(echo $entry | cut -d"," -f1)
	startLine=$(echo $entry | cut -d"," -f2)
	endLine=$(echo $entry | cut -d"," -f3)
	./tools/bin/clang-format -lines=$startLine:$endLine -i $file
done
