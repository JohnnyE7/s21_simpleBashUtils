#ifndef PROGRAM_H
#define PROGRAM_H

#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Функции для обработки символов.
void print_nonprinting_char(int c);

void handle_character(int c, int show_tabs, int show_nonprinting);

// Функция для обработки строк.
int handle_newline(int c, int *prev_char, int *blank_line,
                   int *first_char_in_line, int *empty_line, int squeeze_blank,
                   int show_ends, int *line_number, int number,
                   int number_nonblank);

// Функция для обработки файла с учётом переданных флагов.
void process_file(FILE *file, int number_nonblank, int number,
                  int squeeze_blank, int show_ends, int show_tabs,
                  int show_nonprinting);

// Функция для разбора аргументов командной строки.
int parse_arguments(int argc, char *argv[], int *number_nonblank, int *number,
                    int *squeeze_blank, int *show_ends, int *show_tabs,
                    int *show_nonprinting);

#endif  // PROGRAM_H
