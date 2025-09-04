#ifndef S21_GREP_H
#define S21_GREP_H

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "s21_vector.h"

// Структура для хранения флагов командной строки
typedef struct grep_options {
  int ignore_case;  // Флаг -i: игнорировать регистр при сопоставлении
  int invert_match;  // Флаг -v: инвертировать совпадения (выводить строки,
                     // которые НЕ совпадают с шаблоном)
  int count;  // Флаг -c: выводить только количество совпадений для каждого
              // файла
  int list_files;  // Флаг -l: выводить только имена файлов, содержащих
                   // совпадения
  int line_number;  // Флаг -n: выводить номера строк, содержащих совпадения
  int no_filename;  // Флаг -h: подавлять вывод имен файлов в результатах
  int suppress_errors;  // Флаг -s: подавлять ошибки при открытии файлов
                        // (например, если файл не найден)
  int only_matching;  // Флаг -o: выводить только совпавшие части строки, а не
                      // всю строку целиком
  int use_patterns;  // Флаг -e: использовать указанные шаблоны для поиска
                     // (может быть указан несколько раз)
} grep_options_t;

// Структура для хранения информации о совпадении
typedef struct match {
  size_t start;          // Начало совпадения
  size_t end;            // Конец совпадения
  size_t pattern_index;  // Индекс паттерна
} match_t;

// Прототипы функций

// Основная логика программы
int main(int argc, char *argv[]);

// Парсинг флагов и аргументов командной строки
void parse_flags(int argc, char *argv[], grep_options_t *flags,
                 vector_t *patterns, vector_t *file_names);

// Загрузка шаблонов из файла
vector_t load_patterns_from_file(const char *filename);

// Проверка совпадения строки с шаблонами
int match_line(const char *line, vector_t *patterns,
               const grep_options_t *flags);

// Находит ближайшее совпадение
int find_closest_match(const char *line, size_t offset, regex_t *regexes,
                       size_t regex_count, match_t *closest_match);

// Обработка флага -o
void process_only_matching(const char *line, vector_t *patterns,
                           const grep_options_t *flags);

// Обработка каждого файла
void process_file(const char *file_name, vector_t *patterns,
                  const grep_options_t *flags, const vector_t *file_names);

// Обработка строки из файла
int process_line(const char *line, vector_t *patterns,
                 const grep_options_t *flags, const char *file_name,
                 unsigned long line_number, unsigned long *match_count,
                 int print_file_name);

// Обработка ошибок при открытии файла
void handle_file_error(const char *file_name, const grep_options_t *flags);

// Проверка, нужно ли выводить имя файла
int should_print_file_name(const grep_options_t *flags,
                           const vector_t *file_names);

#endif  // S21_GREP_H
