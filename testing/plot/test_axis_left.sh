#!/bin/bash

signaltk img new --out ${TEST_OUTFILE} --width 800 --height 400 --clear '#fff'
signaltk plot axes --in ${TEST_OUTFILE} --out ${TEST_OUTFILE} --left
