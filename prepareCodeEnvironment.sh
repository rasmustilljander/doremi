#! /bin/bash

echo -e "\nSetting environment variables..."
echo "\$SSP_CODE_HOME = $PWD"

setx.exe SSP_CODE_HOME $PWD

echo -e "\nCopying git hooks..."
cp tools/githook/* .git/hooks/

echo -e "\nSetting up .gitconfig"
gitConfig="git config --global alias.amend \"commit --amend --no-verify\""
echo $gitConfig
eval $gitConfig

echo -e "\nPreparing code environment complete"
read -rsp $'Press any key to exit...\n' -n 1 key