#!/bin/bash
set -e

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

if [[ ! -e ${masterfile} || ! -z "${PLOTFX_TEST_FORCE}" ]]; then
  cp ${outfile} ${masterfile}
fi

echo

# FIXME image diff
if !(diff ${outfile} ${masterfile} &>/dev/null); then
  echo
  print_error "ERROR: output files do not match"
  echo "Diff:"
  diff ${outfile} ${masterfile} || true
  exit 1
fi
