#!/bin/bash
set -e

binfile="$1"
testname="$2"
srcdir="$3"
tmpdir="$4"
format="svg"

infile="${srcdir}/${testname}.fvz"
reffile="${srcdir}/${testname}.${format}"
errfile="${srcdir}/${testname}.err"
outfile="${tmpdir}/${testname}.${format}"
logfile="${tmpdir}/${testname}.log"

# clean up old files
rm -rf "${outfile}" "${logfile}"

# run fviz
echo "-------------------------------------------------------------------------------"
echo "${binfile}" --in "${infile}" --out "${outfile}"
echo "-------------------------------------------------------------------------------"

result=""
if "${binfile}" \
      --font-defaults off \
      --font-load /usr/share/fonts/msttcore/arial.ttf \
      --in "${infile}" \
      --out "${outfile}" \
      2> "${logfile}"; then
  result="ok"
else
  result="fail"
fi

# check error messages
if [[ -e "${errfile}" ]]; then
  if [[ ${result} != "fail" ]]; then
    echo "ERROR: expected failure but got success"
    cat ${logfile}
    exit 1
  fi

  if (diff ${logfile} ${errfile} &>/dev/null); then
    exit 0
  else
    echo "ERROR: error messages do not match"
    diff ${logfile} ${errfile} || true
    exit 1
  fi
fi

# check result
if [[ ${result} != "ok" ]]; then
  echo "ERROR: execution failed"
  cat ${logfile}
  exit 1
fi

if [[ ! -e ${reffile} || ! -z "${FVIZ_TEST_FORCE}" ]]; then
  cp ${outfile} ${reffile}
fi

if (diff ${outfile} ${reffile} &>/dev/null); then
  cat ${logfile}
  exit 0
else
  echo "ERROR: output files do not match"
  echo "OUTPUT:" ${outfile}
  echo "EXPECT:" ${reffile}
  echo "-------------------------------------------------------------------------------"
  #cat ${logfile}
  #diff ${outfile} ${reffile} || true
  exit 1
fi
