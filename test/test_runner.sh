#!/bin/bash
set -ue

source_path="$(realpath "$(dirname "$0")/..")"
proc_path="$(realpath "./clip")"
test_path="${source_path}/test"
result_path="$(realpath ./test-results)"

mkdir -p "${result_path}"

run_all_tests() {
	log_init

	num_total=0
	num_pass=0
	num_fail=0

	test_cases=$(cd "${source_path}/test" && find . -name "*.clp" ! -path "./spec/*" | sort)

	for test_id in ${test_cases[@]}; do
		local test_id="$(echo "${test_id}" | sed -e 's/^.\///' -e 's/\.clp$//')"
		num_total=$[ $num_total + 1 ]

		if run_test "${test_id}" &>/dev/null; then
			echo "$(printf "\033[1;32m%s\033[0m" "[PASS]") ${test_id}"
		  num_pass=$[ $num_pass + 1 ]
		else
			echo "$(printf "\033[1;31m%s\033[0m" "[FAIL]") ${test_id}"
		  num_fail=$[ $num_fail + 1 ]
		fi
	done

	if [[ ${num_total} == ${num_pass} ]]; then
		echo
		echo "TEST RESULT: $(printf "\033[1;32m%s\033[0m" "PASS")"
		echo
		echo "${num_pass}/${num_total} test cases ok"
    exit 0
	else
		echo
		echo "Test Result: $(printf "\033[1;31m%s\033[0m" "FAIL")"
		echo
		echo "${num_fail}/${num_total} test cases failed"
    exit 1
	fi
}

run_test() {
	local test_id="$1"
	local format="svg"

	local infile="${test_path}/${test_id}.clp"
	local reffile="${test_path}/${test_id}.${format}"
	local errfile="${test_path}/${test_id}.err"
	local outfile="${result_path}/${test_id}.${format}"
	local logfile="${result_path}/${test_id}.log"

	# clean up old files
	rm -rf "${outfile}" "${logfile}"
	mkdir -p "$(dirname "${outfile}")"

	# run clip
	result=""
	if (cd ${source_path} && "${proc_path}" \
				--font-load "test/testdata/fonts/LiberationSans-Regular.ttf" \
				--in "${infile}" \
				--out "${outfile}" \
				2> "${logfile}"); then
		result="ok"
	else
		result="fail"
	fi

	# check error messages
	if [[ -e "${errfile}" ]]; then
		if [[ ${result} != "fail" ]]; then
			echo "ERROR: expected failure but got success" >> ${logfile}
			return 1
		fi

		if (diff ${logfile} ${errfile} &>/dev/null); then
			return 0
		else
			echo "ERROR: error messages do not match" >> ${logfile}
			return 1
		fi
	fi

	# check result
	if [[ "${result}" != "ok" ]]; then
		echo "ERROR: execution failed" >> ${logfile}
		cat ${logfile}
		return 1
	fi

	if [[ ! -e ${reffile} ]]; then
		cp ${outfile} ${reffile}
	fi

	if (diff ${outfile} ${reffile} &>/dev/null); then
		echo "OK" >&2
		cat ${logfile}
		return 0
	else
		echo "FAIL" >&2
		log_failure "${test_id}" "${outfile}" "${reffile}" "${logfile}"
		cat ${logfile}
		return 1
	fi
}

log_append() {
	echo "$1" >> "${result_path}/index.html"
}

log_init() {
	(cat > "${result_path}/index.html") <<-EOF
		<title>Test Results</title>
		<style type='text/css'>
		  body {
		    max-width: 1200px;
		    font-family: sans-serif;
		  }
		
		  th {
		    background: #eee;
		    padding: 2pt;
		  }
		
		  td, th {
		    text-align: left;
		  }
		
		  table {
		    border: 1px solid #000;
		  }
		
		  section {
		    margin-bottom: 2em;
		  }
		
		  img {
		    max-width: 100%;
		  }
		
		  h3 {
		    margin: 4pt 0;
		  }
		</style>
		
	EOF
}

log_failure() {
	local test_id="$1"
	local test_result_path="$2"
	local test_expectation_path="$3"
	local test_logfile_path="$4"

	log_append "<section>"
	log_append "  <table width='100%'>"
	log_append "    <tr>"
	log_append "      <th colspan=2>"
	log_append "        <h3>Test Failure: ${test_id}</h3>"
	log_append "      </th>"
	log_append "    </tr>"
	log_append "    <tr>"
	log_append "      <th>Result</th>"
	log_append "      <th>Expectation</th>"
	log_append "    </tr>"
	log_append "    <tr>"
	log_append "      <td><img src='${test_result_path}' /></td>"
	log_append "      <td><img src='${test_expectation_path}' /></td>"
	log_append "    </tr>"
	log_append "    <tr>"
	log_append "      <th colspan=2>Command</th>"
	log_append "    </tr>"
	log_append "    <tr>"
	log_append "      <td><code>$ ${proc_path} --in tests/spec/${test_id}.clp --out /tmp/t.svg</code></td>"
	log_append "    </tr>"
	log_append "    <tr>"
	log_append "      <th colspan=2>Reference File</th>"
	log_append "    </tr>"
	log_append "    <tr>"
	log_append "      <td><code>$(realpath "${test_expectation_path}")</code></td>"
	log_append "    </tr>"
	log_append "    <tr>"
	log_append "      <th colspan=2>Details</th>"
	log_append "    </tr>"
	log_append "    <tr>"
	log_append "      <pre>$(cat ${test_logfile_path})</pre>"
	log_append "    </tr>"
	log_append "    <tr>"
	log_append "      <td>"
	log_append "        <details>"
	log_append "          <summary>Logfile</summary>"
	log_append "          <pre>$(log_escape < ${test_logfile_path})</pre>"
	log_append "        </details>"
	log_append "      </td>"
	log_append "    </tr>"
	log_append "    <tr>"
	log_append "      <td>"
	log_append "        <details>"
	log_append "          <summary>SVG Diff</summary>"
	log_append "          <pre>$(diff ${test_result_path} ${test_expectation_path} | log_escape)</pre>"
	log_append "        </details>"
	log_append "      </td>"
	log_append "    </tr>"
	log_append "  </table>"
	log_append "</section>"
}

log_escape() {
	sed \
		-e 's/&/\&amp;/g' \
		-e 's/</\&lt;/g' \
		-e 's/>/\&gt;/g'
}

if [[ $# -eq 1 ]]; then
	run_test "$1"
else
	run_all_tests
fi
