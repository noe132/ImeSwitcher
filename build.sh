#!/usr/bin/env bash
cd "$(dirname "$0")"
rm ./build -rf
rm .qmake.stash
qmake && make
