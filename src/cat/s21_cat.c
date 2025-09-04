#include "s21_cat.h"

int main(int argc, char *argv[]) {
  int number_nonblank = 0, number = 0, squeeze_blank = 0;
  int show_ends = 0, show_tabs = 0, show_nonprinting = 0;

  if (parse_arguments(argc, argv, &number_nonblank, &number, &squeeze_blank,
                      &show_ends, &show_tabs, &show_nonprinting))
    return 0;

  if (optind == argc) {
    process_file(stdin, number_nonblank, number, squeeze_blank, show_ends,
                 show_tabs, show_nonprinting);
  } else {
    for (int i = optind; i < argc; i++) {
      FILE *file = fopen(argv[i], "r");
      if (!file) {
        perror(argv[i]);
      } else {
        number = !number_nonblank && number;

        process_file(file, number_nonblank, number, squeeze_blank, show_ends,
                     show_tabs, show_nonprinting);
        fclose(file);
      }
    }
  }

  return 0;
}

void print_nonprinting_char(int c) {
  if (c == 127) {
    printf("^?");
  } else if (c < 32) {
    printf("^%c", c + 64);
  } else if (c == 255) {
    printf("M-^?");
  } else if (c < 160) {
    printf("M-^%c", c - 64);
  } else {
    printf("M-%c", c - 128);
  }
}

void handle_character(int c, int show_tabs, int show_nonprinting) {
  if (c == '\t' && show_tabs) {
    printf("^I");
  } else if (show_nonprinting && !isprint((unsigned char)c) && c != '\n' &&
             c != '\t') {
    print_nonprinting_char(c);
  } else {
    putchar(c);
  }
}

int handle_newline(int c, int *prev_char, int *blank_line,
                   int *first_char_in_line, int *empty_line, int squeeze_blank,
                   int show_ends, int *line_number, int number,
                   int number_nonblank) {
  *blank_line = (squeeze_blank && *prev_char == '\n') ? (*blank_line + 1) : 0;

  if (squeeze_blank && *blank_line > 1) {
    return 0;
  }

  if (*first_char_in_line && (number || (number_nonblank && !*empty_line))) {
    printf("%6d\t", (*line_number)++);
  }

  if (show_ends) {
    putchar('$');
  }
  putchar('\n');

  *first_char_in_line = 1;
  *empty_line = 1;
  *prev_char = c;

  return 1;
}

void process_file(FILE *file, int number_nonblank, int number,
                  int squeeze_blank, int show_ends, int show_tabs,
                  int show_nonprinting) {
  int line_number = 1;
  int first_char_in_line = 1;
  int empty_line = 1;
  int prev_char = '\n';
  int blank_line = 0;
  int c = 0;

  while ((c = fgetc(file)) != EOF) {
    if (c == '\n') {
      int should_continue = handle_newline(
          c, &prev_char, &blank_line, &first_char_in_line, &empty_line,
          squeeze_blank, show_ends, &line_number, number, number_nonblank);

      if (should_continue == 0) {
        empty_line = 1;
        blank_line = 1;
        first_char_in_line = 1;
        prev_char = '\n';
      }
    } else {
      if (!isspace(c) || c == '\t' || c == ' ' || c == 11 || c == 12 ||
          c == '\r') {
        empty_line = 0;
      }

      blank_line = 0;

      if (first_char_in_line) {
        if (number || (number_nonblank && !empty_line)) {
          printf("%6d\t", line_number++);
        }
        first_char_in_line = 0;
      }

      handle_character(c, show_tabs, show_nonprinting);
      prev_char = c;
    }
  }
}

int parse_arguments(int argc, char *argv[], int *number_nonblank, int *number,
                    int *squeeze_blank, int *show_ends, int *show_tabs,
                    int *show_nonprinting) {
  int opt = 0;
  int error = 0;
  struct option long_options[] = {{"number-nonblank", no_argument, 0, 'b'},
                                  {"number", no_argument, 0, 'n'},
                                  {"squeeze-blank", no_argument, 0, 's'},
                                  {"show-ends", no_argument, 0, 'E'},
                                  {"show-tabs", no_argument, 0, 'T'},
                                  {"show-nonprinting", no_argument, 0, 'v'},
                                  {"e", no_argument, 0, 'e'},
                                  {"t", no_argument, 0, 't'},
                                  {0, 0, 0, 0}};

  while ((opt = getopt_long(argc, argv, "bnsETvet", long_options, NULL)) !=
         -1) {
    switch (opt) {
      case 'b':
        *number_nonblank = 1;
        break;
      case 'n':
        *number = 1;
        break;
      case 's':
        *squeeze_blank = 1;
        break;
      case 'E':
        *show_ends = 1;
        break;
      case 'T':
        *show_tabs = 1;
        break;
      case 'v':
        *show_nonprinting = 1;
        break;
      case 'e':
        *show_ends = 1;
        *show_nonprinting = 1;
        break;
      case 't':
        *show_tabs = 1;
        *show_nonprinting = 1;
        break;
      default:
        fprintf(stderr, "Usage: %s [-bnsETvet] [file...]\n", argv[0]);
        error = 1;
    }
  }

  return error;
}
