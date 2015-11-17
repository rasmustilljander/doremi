# Zlib Copyright 2014 Robograde Development team https://github.com/Robograde/Robograde
# Modified by Rasmus Tilljander to fit our team

#! /bin/bash
if [ -z "$2" ]; then
	grep --include=*.cpp --include=*.hpp -Hnr $1 $SSP_CODE_HOME
else
	grep --include=*.cpp --include=*.hpp -Hnr $1 $SSP_CODE_HOME > $2
fi