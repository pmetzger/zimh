#!/bin/sh
#
# Run generated simulator integration tests directly in the current
# terminal, then compare terminal mode before and after the run.
#
# This is a manual diagnostic for investigating tests that may leave a
# controlling terminal in raw, no-echo, or otherwise damaged mode. It is
# intentionally separate from the automated CTest path: use it from an
# interactive terminal when checking whether direct simulator runs corrupt
# the caller's tty state.
#
# The script expects a configured release build under build/release and
# replays the add_test() commands emitted into the generated CTest files.

set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
build_dir="${repo_root}/build/release"
before_stty_file=
after_stty_file=
before_stty_raw=

capture_tty_state() {
    if [ ! -t 0 ]; then
        return 1
    fi
    stty -a
}

print_tty_state() {
    printf '\n== %s ==\n' "$1"
    if capture_tty_state; then
        :
    else
        echo "stdin is not a tty"
    fi
}

cleanup() {
    status=$?

    print_tty_state "stty after direct integration run"

    if [ -t 0 ] && [ -n "${before_stty_file}" ] && [ -n "${after_stty_file}" ]; then
        stty -a >"${after_stty_file}"
        after_stty_raw=$(stty -g)
        printf '\n== stty comparison ==\n'
        if [ "${before_stty_raw}" = "${after_stty_raw}" ]; then
            echo "terminal mode unchanged"
        else
            echo "terminal mode changed"
            diff -u "${before_stty_file}" "${after_stty_file}" || true
        fi
    fi

    rm -f "${before_stty_file}" "${after_stty_file}"
    exit "$status"
}

trap cleanup EXIT INT TERM HUP

cd "$repo_root"

if [ -t 0 ]; then
    before_stty_file=$(mktemp "${TMPDIR:-/tmp}/run-integration-tty-before.XXXXXX")
    after_stty_file=$(mktemp "${TMPDIR:-/tmp}/run-integration-tty-after.XXXXXX")
    stty -a >"${before_stty_file}"
    before_stty_raw=$(stty -g)
fi

print_tty_state "stty before direct integration run"

run_one_test() {
    testfile=$1
    testname=$2
    add_test_line=$3
    workdir=$(dirname "$testfile")

    command_fields=$(
        printf '%s\n' "$add_test_line" |
        awk '
            {
                while (match($0, /"[^"]*"/)) {
                    print substr($0, RSTART + 1, RLENGTH - 2);
                    $0 = substr($0, RSTART + RLENGTH);
                }
            }'
    )

    set -- $command_fields
    if [ $# -eq 0 ]; then
        return 0
    fi

    props_line=$(grep "^set_tests_properties(${testname} " "$testfile" || true)
    env_string=
    if [ -n "$props_line" ]; then
        env_string=$(
            printf '%s\n' "$props_line" |
            sed -n 's/.*ENVIRONMENT "\([^"]*\)".*/\1/p'
        )
    fi

    printf '\n== %s (%s) ==\n' "$testname" "$testfile"
    printf 'cd %s\n' "$workdir"
    printf 'command:'
    for arg in "$@"; do
        printf ' %s' "$arg"
    done
    printf '\n'

    if [ -n "$env_string" ]; then
        printf 'env: %s\n' "$env_string"
        env_assignments=$(printf '%s' "$env_string" | tr ';' ' ')
        (
            cd "$workdir"
            # shellcheck disable=SC2086
            env $env_assignments "$@"
        )
    else
        (
            cd "$workdir"
            "$@"
        )
    fi
}

find "$build_dir/simulators" -name CTestTestfile.cmake | sort |
while IFS= read -r testfile; do
    grep '^add_test(' "$testfile" |
    while IFS= read -r add_test_line; do
        testname=$(
            printf '%s\n' "$add_test_line" |
            sed -n 's/^add_test(\([^ ]*\).*/\1/p'
        )
        run_one_test "$testfile" "$testname" "$add_test_line"
    done
done
