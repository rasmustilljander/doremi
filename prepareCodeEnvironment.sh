#! /bin/bash

echo -e "\nSetting environment variables..."
echo "\$SSP_CODE_HOME = $PWD"

export SSP_CODE_HOME=$PWD

echo -e "\nCopying git hooks..."
cp tools/githook/* .git/hooks/

echo -e "\nPreparing code environment complete"
read -rsp $'Press any key to exit...\n' -n 1 key