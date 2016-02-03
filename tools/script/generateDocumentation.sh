#! /bin/bash
mkdir -p docs
cd docs
command="$SSP_CODE_HOME/tools/bin/doxygen.exe $SSP_CODE_HOME/.doxygen"
         $SSP_CODE_HOME/tools/bin/doxygen.exe $SSP_CODE_HOME/.doxygen

eval $command