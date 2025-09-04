# Имя исполняемого файла
TARGET = s21_cat

# Компилятор и флаги
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11

# Исходные файлы
SRCS = s21_cat.c
OBJS = $(SRCS:s21_cat.c=s21_cat.o)

# Папки и файлы для тестов
TEST_SCRIPT = integration_test.sh
TEST_RESULTS_DIR = test_results
DEPS = s21_cat.h

.PHONY: all clean tests

# Сборка всего проекта
all: $(TARGET)

# Линковка исполняемого файла
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Компиляция объектов
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Тесты
tests: $(TARGET)
	@echo "Running integration tests..."
	@mkdir -p $(TEST_RESULTS_DIR)
	bash $(TEST_SCRIPT)
	@echo "Tests completed. Results saved in $(TEST_RESULTS_DIR)."

# Очистка
clean:
	@echo "Cleaning up..."
	@rm -rf $(TARGET) $(OBJS) $(TEST_RESULTS_DIR)
	@echo "Clean completed."
