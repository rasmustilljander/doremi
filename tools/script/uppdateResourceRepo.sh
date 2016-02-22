#! /bin/bash
currentPath=`pwd`
cd $SSP_RESOURCE_HOME
git pull --rebase origin master
cd $pwd