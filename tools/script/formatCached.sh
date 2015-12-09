#! /bin/bash
cd $SSP_CODE_HOME

for entry in $(git status | grep ".cpp\|.hpp" | grep "modified:\|new file:" | cut -d$'\t' -f2 | cut -d":" -f 2)
do
	echo $entry
	
	#./tools/bin/clang-format -i $entry
done
