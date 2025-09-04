# Компилятор и флаги
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11

# Директории для тестов
TEST_RESULTS_DIR = test_results
LEAKS_RESULT_DIR = leak_test_results
TEST_SCRIPT = integration_test.sh
LEAKS_SCRIPT = leaks_test.sh

# Директория для объектных файлов
OBJ_DIR = bin

# Название исполняемого файла
TARGET = s21_grep

# Исходные файлы и объектные файлы
SRC = s21_grep.c s21_vector.c
OBJ = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRC))

# Цели
all: $(TARGET)

# Создание исполняемого файла
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

# Сборка объектных файлов
$(OBJ_DIR)/%.o: %.c %.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Создание директории для объектных файлов
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Очистка
clean:
	@echo "Cleaning up..."
	@rm -rf $(TARGET) $(OBJ_DIR) $(TEST_RESULTS_DIR) $(LEAKS_RESULT_DIR)
	@echo "Clean completed."

# Тесты
tests: $(TARGET)
	@echo "Running integration tests..."
	@mkdir -p $(TEST_RESULTS_DIR)
	@bash $(TEST_SCRIPT)
	@echo "Tests completed. Results saved in $(TEST_RESULTS_DIR)."

leaks: $(TARGET)
	@mkdir -p $(LEAKS_RESULT_DIR)
	@bash $(LEAKS_SCRIPT)

# Пересборка проекта
rebuild: clean all

# Упрощённое правило для тестирования Makefile
.PHONY: all clean rebuild
