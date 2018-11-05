#!/bin/bash
set -ue

print_error () {
  printf "\033[1;31m"
  echo -e "$1"
  printf "\033[0m\n"
}

test_script="$1"
test_outfile="$1.png"
test_masterfile="$2"

${test_script} || exit 1

if [[ ! -e ${test_masterfile} ]]; then
  cp ${test_outfile} ${test_masterfile}
fi

# FIXME image diff
if !(diff ${test_outfile} ${test_masterfile} &>/dev/null); then
  echo
  print_error "ERROR: output files do not match:\n    - actual: ${test_outfile}\n    - master: ${test_masterfile}"
  echo
  echo -e "If the modification is intended, fix by running:\n    cp ${test_outfile} ${test_masterfile}"
  exit 1
fi
