#!/usr/bin/bash

set -eu

# go to the git root dir
cd "$(git rev-parse --show-toplevel)"

clang-format -i src/src/*.cpp src/include/vnepogodin/*.hpp \
    modules/settings/src/*.{cpp,hpp}
