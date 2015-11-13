#! /bin/bash
cd $SSP_CODE_HOME
git diff --cached --name-only | grep ".cpp\|.hpp" | xargs External/clang-format/clang-format