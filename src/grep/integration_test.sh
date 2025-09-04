#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'
BLUE='\033[0;34m'

RESULTS_DIR="test_results"
TOTAL_TESTS=0
PASSED_TESTS=0

# Убедимся, что директория для результатов существует
mkdir -p "$RESULTS_DIR"

run_test() {
    local test_name="$1"
    local pattern="$2"
    local flags="$3"
    local files="$4"
    ((TOTAL_TESTS++))

    # Подготовка директории для текущего теста
    local sanitized_name=$(echo "$test_name")
    local test_dir="$RESULTS_DIR/$sanitized_name"
    mkdir -p "$test_dir"
    
    echo -e "${BLUE}Running test: ${test_name}${NC}"

    # Выполнение команды
    if [ -z "$flags" ]; then
        ./s21_grep "$pattern" $files > "$test_dir/s21_output.txt"
        grep "$pattern" $files > "$test_dir/original_output.txt"
        echo "./s21_grep $pattern $figrep > $test_dir/s21_output.txt"
        echo "grep $pattern $files > $test_dir/original_output.txt"
    else
        ./s21_grep $flags "$pattern" $files > "$test_dir/s21_output.txt"
        grep $flags "$pattern" $files > "$test_dir/original_output.txt"
        echo "./s21_grep $flags $pattern $files > $test_dir/s21_output.txt"
        echo "grep $flags $pattern $files > $test_dir/original_output.txt"
    fi

    # Сравнение результатов
    if diff "$test_dir/s21_output.txt" "$test_dir/original_output.txt" > "$test_dir/diff.txt" 2>&1; then
        echo -e "${GREEN}Test passed: ${test_name}${NC}"
        ((PASSED_TESTS++))
    else
        echo -e "${RED}Test failed: ${test_name}${NC}"
        echo "Differences found. See $test_dir/diff.txt for details."
    fi
    echo "----------------------------------------"
}

run_test_new() {
    local test_name="$1"
    local pattern="$2"
    local flags="$3"
    local files="$4"
    ((TOTAL_TESTS++))

    # Подготовка директории для текущего теста
    local sanitized_name=$(echo "$test_name")
    local test_dir="$RESULTS_DIR/$sanitized_name"
    mkdir -p "$test_dir"
    
    echo -e "${BLUE}Running test: ${test_name}${NC}"

    # Подготовка аргументов
    local s21_args=("./s21_grep")
    local grep_args=("grep")
    if [ -n "$flags" ]; then
        s21_args+=($flags)
        grep_args+=($flags)
    fi

    if [ -n "$pattern" ]; then
        # Разбиваем паттерн на отдельные слова и сохраняем в массив
        read -r -a patterns <<< "$pattern"
        for p in "${patterns[@]}"; do
            s21_args+=("-e" "$p")
            grep_args+=("-e" "$p")
        done
    fi

    s21_args+=($files)
    grep_args+=($files)

    # Выполнение команды
    "${s21_args[@]}" > "$test_dir/s21_output.txt" 2>/dev/null
    echo "${s21_args[@]}"
    "${grep_args[@]}" > "$test_dir/original_output.txt" 2>/dev/null
    echo "${grep_args[@]}"
    
    # Сравнение результатов
    if diff "$test_dir/s21_output.txt" "$test_dir/original_output.txt" > "$test_dir/diff.txt" 2>&1; then
        echo -e "${GREEN}Test passed: ${test_name}${NC}"
        ((PASSED_TESTS++))
    else
        echo -e "${RED}Test failed: ${test_name}${NC}"
        echo "Differences found. See $test_dir/diff.txt for details."
    fi
    echo "----------------------------------------"
}

# Запуск тестов
run_test "Basic pattern match" "test" "" "test_files/test.txt"
run_test "Case sensitive" "TEST" "" "test_files/test.txt"
run_test "Multiple files" "test" "" "test_files/test.txt test_files/test.txt"

# Тесты с флагами
run_test "Flag -e (template)" "test" "-e [^hello$]" "test_files/test.txt"
run_test "Flag -i (ignore case)" "test" "-i" "test_files/test.txt"
run_test "Flag -v (invert match)" "test" "-v" "test_files/test.txt"
run_test "Flag -c (count)" "test" "-c" "test_files/test.txt"
run_test "Flag -l (files with matches)" "test" "-l" "test_files/test.txt test_files/test.txt"
run_test "Flag -n (line number)" "test" "-n" "test_files/test.txt"
run_test "Flag -h (no filename)" "test" "-h" "test_files/test.txt test_files/test.txt"
run_test "Flag -s (suppress errors)" "test" "-s" "test_files/test.txt test_filesnonexistent.txt"
run_test "Flag -f (patterns from file)" "" "-f test_files/patterns.txt" "test_files/test.txt"
run_test "Flag -o (only matching)" "test" "-o" "test_files/test.txt"

# Запуск тестов
run_test_new "Basic pattern match" "test" "" "test_files/test.txt"
run_test_new "Case sensitive" "TEST" "" "test_files/test.txt"
run_test_new "Multiple files" "test" "" "test_files/test.txt test_files/test2.txt"

# Тесты с флагами
run_test_new "Flag -e" "[^hello$]" "" "test_files/test.txt"
run_test_new "Flag -i -e" "test" "-i" "test_files/test.txt"
run_test_new "Flag -v -e" "test" "-v" "test_files/test.txt"
run_test_new "Flag -c -e" "test" "-c" "test_files/test.txt"
run_test_new "Flag -l -e" "test" "-l" "test_files/test.txt test_files/test2.txt"
run_test_new "Flag -n -e" "test" "-n" "test_files/test.txt"
run_test_new "Flag -h -e" "test" "-h" "test_files/test.txt test_files/test2.txt"
run_test_new "Flag -s -e" "test" "-s" "test_files/test.txt test_files/nonexistent.txt"
run_test_new "Flag -f" "" "-f test_files/patterns.txt" "test_files/test.txt"
run_test_new "Flag -o -e" "test" "-o" "test_files/test.txt"

# Скомбинированные флаги
run_test_new "Flags -iv -e" "test" "-iv" "test_files/test.txt"
run_test_new "Flags -in -e" "test" "-in" "test_files/test.txt"
run_test_new "Flags -ic -e" "test" "-ic" "test_files/test.txt"
run_test_new "Flags -il -e" "test" "-il" "test_files/test.txt test_files/test2.txt"
run_test_new "Flags -nh -e" "test" "-nh" "test_files/test.txt test_files/test2.txt"
run_test_new "Flags -cl -e" "test" "-cl" "test_files/test.txt test_files/test2.txt"

# Разные паттерны
run_test_new "Special chars pattern" "\\." "" "test_files/test.txt"
run_test_new "Number pattern" "123" "" "test_files/test.txt"
run_test_new "Multiple word pattern" "test patterns" "" "test_files/test.txt"

# Новый тест для команды grep -e num -o -e out ./tests/files/1.file
run_test_new "Multiple -e and -o flags out num" "out num" "-o" "test_files/1.file"
run_test_new "Multiple -e and -o flags num out" "num out" "-o" "test_files/1.file"

# Вывод итогов
echo -e "\n${BLUE}Test Summary:${NC}"
echo -e "Total tests: ${TOTAL_TESTS}"
echo -e "${GREEN}Passed tests: ${PASSED_TESTS}${NC}"
if [ $PASSED_TESTS -eq $TOTAL_TESTS ]; then
    echo -e "${GREEN}All tests passed successfully!${NC}"
else
    echo -e "${RED}Failed tests: $(($TOTAL_TESTS - $PASSED_TESTS))${NC}"
fi
