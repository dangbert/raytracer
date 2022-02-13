#!/bin/bash

set -euC

find rendering -iname '*.cpp' -o -iname '*.h' | xargs clang-format --style=file --fallback-style=none -i