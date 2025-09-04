#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STEP 4

// Определение vector_t
typedef struct {
  char **data;    // Указатель на массив строк
  size_t length;  // Текущее количество элементов
  size_t capacity;  // Максимальное количество элементов
  size_t step;  // Шаг увеличения capacity
} vector_t;

// Реализация strdup()
char *strdup(const char *str);

// Функция для инициализации вектора
void vector_init(vector_t *vec);

// Функция для добавления строки в вектор
void vector_push_back(vector_t *vec, const char *str);

// Функция для освобождения памяти вектора
void vector_free(vector_t *vec);
