#!/bin/bash
set -ue

print_error () {
  printf "\033[1;31m"
  echo -e "$1"
  printf "\033[0m\n"
}

binfile="$1"
specfile="$2"
outfile="$3"
masterfile="$4"

rm -rf ${outfile}
${binfile} --in ${specfile} --out ${outfile} || exit 1

if [[ ! -e ${masterfile} ]]; then
  cp ${outfile} ${masterfile}
fi

# FIXME image diff
if !(diff ${outfile} ${masterfile} &>/dev/null); then
  echo
  print_error "ERROR: output files do not match"
  echo -e "If the modification is intended, fix by running:\n    cp ${outfile} ${masterfile}"
  echo
  echo -e "To view the diff, run:\n    diff ${outfile} ${masterfile}"
  exit 1
fi
