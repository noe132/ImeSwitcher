#!/usr/bin/env bash
cd "$(dirname "$0")"
./build.sh
open -n ./build/release/ImeSwitcher.app
