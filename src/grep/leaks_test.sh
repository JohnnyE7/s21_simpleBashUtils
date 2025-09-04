#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

RESULTS_DIR="leak_test_results"
TOTAL_TESTS=0
FAILED_TESTS=0

# Убедимся, что директория для результатов существует
mkdir -p "$RESULTS_DIR"

run_leak_test() {
    local test_name="$1"
    local flags="$2"
    local files="$3"
    local extra_args="$4"
    ((TOTAL_TESTS++))

    # Подготовка директории для текущего теста
    local sanitized_name=$(echo "$test_name" | tr -d '[:space:]')
    local test_dir="$RESULTS_DIR/$sanitized_name"
    mkdir -p "$test_dir"
    
    echo -e "Running leak test: ${test_name}"

    # Формирование команды
    local s21_cmd="./s21_grep"
    if [ -n "$flags" ]; then
        s21_cmd+=" $flags"
    fi
    if [ -n "$extra_args" ]; then
        s21_cmd+=" $extra_args"
    fi
    s21_cmd+=" $files"

    # Проверка утечек Valgrind
    valgrind -s --trace-children=yes --track-fds=yes --track-origins=yes --leak-check=full --show-leak-kinds=all --log-file="$test_dir/valgrind.log" $s21_cmd > /dev/null 2>&1

    # Проверка результата Valgrind
    if grep -q "definitely lost: [1-9][0-9]* bytes" "$test_dir/valgrind.log"; then
        # Если найдены утечки
        local leaks=$(grep "definitely lost" "$test_dir/valgrind.log" | awk '{print $4}')
        echo -e "${RED}Valgrind: $leaks leaks for ${test_name}${NC}"
        ((FAILED_TESTS++))
    elif grep -q "All heap blocks were freed -- no leaks are possible" "$test_dir/valgrind.log"; then
        # Если утечек нет
        echo -e "${GREEN}Valgrind: No leaks for ${test_name}${NC}"
    else
        # Если отчет Valgrind не содержит ожидаемую информацию
        echo -e "${RED}Valgrind output format unexpected for ${test_name}. Check log for details.${NC}"
        echo -e "${BLUE}Valgrind log:${NC}"
        cat "$test_dir/valgrind.log"
        ((FAILED_TESTS++))
    fi

    echo "----------------------------------------"
}

# Пример тестов
run_leak_test "Flag -e (template)" "-e test" "test_files/test.txt"
run_leak_test "Flag -i (ignore case)" "-i" "test_files/test.txt"
run_leak_test "Flag -v (invert match)" "-v test" "test_files/test.txt"
run_leak_test "Flag -c (count)" "-c test" "test_files/test.txt"
run_leak_test "Flag -l (files with matches)" "-l test" "test_files/test.txt test_files/test2.txt"
run_leak_test "Flag -n (line number)" "-n test" "test_files/test.txt"
run_leak_test "Flag -h (no filename)" "-h test" "test_files/test.txt test_files/test2.txt"
run_leak_test "Flag -s (suppress errors)" "-s test" "test_files/test.txt test_files/nonexistent.txt"
run_leak_test "Flag -f (patterns from file)" "-f test_files/patterns.txt" "test_files/test.txt"
run_leak_test "Flag -o (only matching)" "-o test" "test_files/test.txt"

# Итоговый результат
echo -e "\nTest Summary:"
echo -e "Total tests: ${TOTAL_TESTS}"
if [ $FAILED_TESTS -eq 0 ]; then
    echo -e "${GREEN}All tests passed with 0 leaks!${NC}"
else
    echo -e "${RED}Failed tests: ${FAILED_TESTS}${NC}"
fi
