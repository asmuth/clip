#!/bin/bash

plotfx img new --out ${TEST_OUTFILE} --width 800 --height 400 --clear '#fff'
plotfx plot axes --in ${TEST_OUTFILE} --out ${TEST_OUTFILE} --left
