#! /bin/bash
cd $SSP_CODE_HOME
find . -name "*.cpp" -o -name "*.hpp" | xargs External/clang-format/clang-format -i