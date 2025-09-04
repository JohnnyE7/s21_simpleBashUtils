#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'
BLUE='\033[0;34m'

RESULTS_DIR="test_results"
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

FAILED_TESTS_LIST=()

# Создаем папку для результатов тестов
mkdir -p "$RESULTS_DIR"

run_test() {
    local test_name="$1"
    local flags="$2"
    shift 2
    local input_files=("$@")
    local test_dir="$RESULTS_DIR/$test_name"

    mkdir -p "$test_dir"
    ((TOTAL_TESTS++))

    echo -e "${BLUE}Running test: ${test_name}${NC}"

    if [ -z "$flags" ]; then
        ./s21_cat "${input_files[@]}" > "$test_dir/s21_output.txt"
        cat "${input_files[@]}" > "$test_dir/original_output.txt"
    else
        ./s21_cat $flags "${input_files[@]}" > "$test_dir/s21_output.txt"
        cat $flags "${input_files[@]}" > "$test_dir/original_output.txt"
    fi

    if diff "$test_dir/s21_output.txt" "$test_dir/original_output.txt" >/dev/null 2>&1; then
        echo -e "${GREEN}Test passed: ${test_name}${NC}"
        ((PASSED_TESTS++))
    else
        echo -e "${RED}Test failed: ${test_name}${NC}"
        echo "Differences found:"
        diff "$test_dir/s21_output.txt" "$test_dir/original_output.txt"
        ((FAILED_TESTS++))
        FAILED_TESTS_LIST+=("${test_name}")
    fi
    echo "----------------------------------------"
}

# Тесты
run_test "No flags" "" "tests/test_cases_cat.txt"
run_test "Flag -b" "-b" "tests/test_cases_cat.txt"
run_test "Flag -e" "-e" "tests/test_cases_cat.txt"
run_test "Flag -n" "-n" "tests/test_cases_cat.txt"
run_test "Flag -s" "-s" "tests/test_cases_cat.txt"
run_test "Flag -t" "-t" "tests/test_cases_cat.txt"
run_test "Flag -v" "-v" "tests/test_cases_cat.txt"
run_test "Flags -b -e" "-b -e" "tests/test_cases_cat.txt"
run_test "Flags -b -n" "-b -n" "tests/test_cases_cat.txt"
run_test "Flags -b -s" "-b -s" "tests/test_cases_cat.txt"
run_test "Flags -b -t" "-b -t" "tests/test_cases_cat.txt"
run_test "Flags -e -n" "-e -n" "tests/test_cases_cat.txt"
run_test "Flags -e -s" "-e -s" "tests/test_cases_cat.txt"
run_test "Flags -e -t" "-e -t" "tests/test_cases_cat.txt"
run_test "Flags -n -s" "-n -s" "tests/test_cases_cat.txt"
run_test "Flags -n -t" "-n -t" "tests/test_cases_cat.txt"
run_test "Flags -s -t" "-s -t" "tests/test_cases_cat.txt"
run_test "Flags -b -e -n" "-b -e -n" "tests/test_cases_cat.txt"
run_test "Flags -b -e -s" "-b -e -s" "tests/test_cases_cat.txt"
run_test "Flags -b -e -t" "-b -e -t" "tests/test_cases_cat.txt"
run_test "Flags -b -n -s" "-b -n -s" "tests/test_cases_cat.txt"
run_test "Flags -e -n -s" "-e -n -s" "tests/test_cases_cat.txt"
run_test "Flags -n -s -t" "-n -s -t" "tests/test_cases_cat.txt"
run_test "All flags" "-b -e -n -s -t" "tests/test_cases_cat.txt"

run_test "Flag -E" "-E" "tests/test_cases_cat.txt"
run_test "Flag -T" "-T" "tests/test_cases_cat.txt"

run_test "Flag --number" "--number" "tests/test_cases_cat.txt"
run_test "Flag --squeeze-blank" "--squeeze-blank" "tests/test_cases_cat.txt"
run_test "Flag --number-nonblank" "--number-nonblank" "tests/test_cases_cat.txt"

run_test "Empty file" "" "tests/test_cases_cat_empty.txt"
run_test "Nonexistent file" "" "test_cases_cat_NAN.txt"
run_test "Multiple files" "" "tests/test_cases_cat.txt" "tests/test_cases_cat2.txt"
run_test "Invalid flag" "-z" "tests/test_cases_cat.txt"

echo -e "\n${BLUE}Test Summary:${NC}"
echo -e "Total tests: ${TOTAL_TESTS}"
echo -e "${GREEN}Passed tests: ${PASSED_TESTS}${NC}"

if [ $PASSED_TESTS -eq $TOTAL_TESTS ]; then
    echo -e "${GREEN}All tests passed successfully!${NC}"
else
    echo -e "${RED}Failed tests: ${FAILED_TESTS}${NC}"
    echo -e "Failed tests details: ${RED}"
    for test in "${FAILED_TESTS_LIST[@]}"; do
        echo -e "- $test"
    done
    exit 1
fi
