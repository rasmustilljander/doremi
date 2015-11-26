#! /bin/bash
cd $SSP_CODE_HOME
find . -name "*.cpp" -o -name "*.hpp" | xargs tools/bin/clang-format -i