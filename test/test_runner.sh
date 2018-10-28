#!/bin/bash
set -u

realpath() {
    [[ $1 = /* ]] && echo "$1" || echo "$PWD/${1#./}"
}

echo_yellow () {
  printf "\033[1;33m%s\033[0m\n" "$1"
}

echo_green () {
  printf "\033[1;32m%s\033[0m\n" "$1"
}

echo_red () {
  printf "\033[1;31m%s\033[0m\n" "$1"
}

test_dir=$(dirname "$(realpath "$0")")
base_dir="${test_dir}/.."

num_total=0
num_passed=0
num_failed=0

echo "Running tests..."
for test_file in $(find $test_dir -mindepth 2 -name "test_*.sh"); do
  test_masterfile=${test_file/.sh/.png}
  test_outfile=${test_file/.sh/.actual.png}

  echo -n " + ${test_file/${test_dir}\//} "
  num_total=$[ $num_total + 1 ]

  (
      cd "$(dirname "${test_file}")" &&
      PATH="${base_dir}" \
      TEST_OUTFILE="${test_outfile}" \
      ${test_file}
  )

  pass="$?"

  if [[ $pass -eq 0 && ! -e "${test_masterfile}" ]]; then
    cp "${test_outfile}" "${test_masterfile}"
  fi

  # FIXME image diff
  if !(diff ${test_outfile} ${test_masterfile} &>/dev/null); then
    pass=1
  fi

  if [[ ${pass} -eq 0 ]]; then
    echo_green "PASS"
    num_passed=$[ $num_passed + 1 ]
  else
    echo_red "FAIL"
    num_failed=$[ $num_failed + 1 ]
  fi
done

echo
echo -n "Test Summary: "
if [[ ${num_passed} -eq ${num_total} && ${num_total} -gt 0 ]]; then
  echo_green "PASS (${num_passed}/${num_total})"
  exit 0
else
  echo_red "FAIL (${num_failed}/${num_total})"
  exit 1
fi
