#define _POSIX_C_SOURCE 200809L

#include "s21_grep.h"

int main(int argc, char *argv[]) {
  grep_options_t flags = {0};
  vector_t patterns;
  vector_init(&patterns);
  vector_t file_names;
  vector_init(&file_names);

  parse_flags(argc, argv, &flags, &patterns, &file_names);

  if (patterns.length == 0 || file_names.length == 0) {
    fprintf(stderr, "s21_grep: no pattern specified\n");
    vector_free(&file_names);
    vector_free(&patterns);
    return EXIT_FAILURE;
  }

  for (size_t i = 0; i < file_names.length; i++) {
    process_file(file_names.data[i], &patterns, &flags, &file_names);
  }

  vector_free(&file_names);
  vector_free(&patterns);
  return EXIT_SUCCESS;
}

void parse_flags(int argc, char *argv[], grep_options_t *flags,
                 vector_t *patterns, vector_t *file_names) {
  int opt;

  while ((opt = getopt(argc, argv, "e:ivclnhsf:o")) != -1) {
    switch (opt) {
      case 'e':
        flags->use_patterns = 1;
        vector_push_back(patterns, optarg);
        break;
      case 'i':
        flags->ignore_case = 1;
        break;
      case 'v':
        flags->invert_match = 1;
        break;
      case 'c':
        flags->count = 1;
        break;
      case 'l':
        flags->list_files = 1;
        break;
      case 'n':
        flags->line_number = 1;
        break;
      case 'h':
        flags->no_filename = 1;
        break;
      case 's':
        flags->suppress_errors = 1;
        break;
      case 'o':
        flags->only_matching = 1;
        break;
      case 'f': {
        vector_t patterns_from_file = load_patterns_from_file(optarg);
        for (size_t i = 0; i < patterns_from_file.length; i++) {
          vector_push_back(patterns, patterns_from_file.data[i]);
        }
        vector_free(&patterns_from_file);
        break;
      }
      default:
        fprintf(stderr, "Usage: s21_grep [options] pattern [file...]\n");
        return;
    }
  }

  flags->count = !flags->list_files && flags->count;

  if (patterns->length == 0 && optind < argc) {
    vector_push_back(patterns, argv[optind++]);
  }

  while (optind < argc) {
    vector_push_back(file_names, argv[optind++]);
  }
}

vector_t load_patterns_from_file(const char *filename) {
  vector_t patterns;
  vector_init(&patterns);
  FILE *file = fopen(filename, "r");

  if (!file) {
    fprintf(stderr, "s21_grep: %s: No such file or directory\n", filename);
    return patterns;
  }

  char *line = NULL;
  size_t len = 0;

  while (getline(&line, &len, file) != -1) {
    line[strcspn(line, "\n")] = '\0';
    vector_push_back(&patterns, line);
  }

  free(line);
  fclose(file);

  return patterns;
}

int match_line(const char *line, vector_t *patterns,
               const grep_options_t *flags) {
  regex_t regex;
  int regex_flags = 0 | (flags->ignore_case * REG_ICASE) | REG_NEWLINE;
  int result = 0;

  for (size_t i = 0; i < patterns->length; i++) {
    if (regcomp(&regex, patterns->data[i], regex_flags) != 0) {
      fprintf(stderr, "s21_grep: Invalid regular expression\n");
      return 0;
    }

    int match = regexec(&regex, line, 0, NULL, 0);
    regfree(&regex);

    if ((match == 0 && !flags->invert_match) ||
        (match != 0 && flags->invert_match)) {
      result = 1;
      break;
    }
  }

  return result;
}

int find_closest_match(const char *line, size_t offset, regex_t *regexes,
                       size_t regex_count, match_t *closest_match) {
  regmatch_t match;
  int found = 0;
  size_t closest_start = (size_t)-1;

  for (size_t i = 0; i < regex_count; i++) {
    if (regexec(&regexes[i], line + offset, 1, &match, 0) == 0) {
      size_t start = match.rm_so + offset;
      size_t end = match.rm_eo + offset;

      if (start < closest_start) {
        closest_start = start;
        closest_match->start = start;
        closest_match->end = end;
        closest_match->pattern_index = i;
        found = 1;
      }
    }
  }

  return found;
}

void process_only_matching(const char *line, vector_t *patterns,
                           const grep_options_t *flags) {
  regex_t regexes[patterns->length];
  int regex_flags = 0 | (flags->ignore_case * REG_ICASE) | REG_NEWLINE;

  for (size_t i = 0; i < patterns->length; i++) {
    if (regcomp(&regexes[i], patterns->data[i], regex_flags) != 0) {
      fprintf(stderr, "s21_grep: Invalid regular expression: %s\n",
              (char *)patterns->data[i]);
      return;
    }
  }

  size_t offset = 0;
  match_t closest_match;

  while (find_closest_match(line, offset, regexes, patterns->length,
                            &closest_match)) {
    printf("%.*s\n", (int)(closest_match.end - closest_match.start),
           line + closest_match.start);

    offset = closest_match.end;
  }

  for (size_t i = 0; i < patterns->length; i++) {
    regfree(&regexes[i]);
  }
}

void process_file(const char *file_name, vector_t *patterns,
                  const grep_options_t *flags, const vector_t *file_names) {
  FILE *file = fopen(file_name, "r");
  if (!file) {
    handle_file_error(file_name, flags);
    return;
  }

  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  unsigned long line_number = 0;
  unsigned long match_count = 0;

  int print_file_name = should_print_file_name(flags, file_names);

  while ((read = getline(&line, &len, file)) != -1) {
    line_number++;
    if (process_line(line, patterns, flags, file_name, line_number,
                     &match_count, print_file_name)) {
      if (flags->list_files) {
        printf("%s\n", file_name);
        break;
      }
    }
  }

  if (flags->count) {
    printf("%lu\n", match_count);
  }

  free(line);
  fclose(file);
}

int process_line(const char *line, vector_t *patterns,
                 const grep_options_t *flags, const char *file_name,
                 unsigned long line_number, unsigned long *match_count,
                 int print_file_name) {
  if (!line || !patterns || !flags || !file_name || !match_count) {
    return 0;
  }

  int result = 0;

  if (flags->only_matching) {
    process_only_matching(line, patterns, flags);
    (*match_count)++;
    result = 1;
  } else if (match_line(line, patterns, flags)) {
    (*match_count)++;
    result = 1;

    if (!(flags->count || flags->list_files)) {
      if (flags->line_number) {
        printf("%lu:", line_number);
      }

      if (flags->no_filename) {
        printf("%s", line);
      } else {
        if (print_file_name) {
          printf("%s:", file_name);
        }
        printf("%s", line);
      }

      if (line[strlen(line) - 1] != '\n') {
        printf("\n");
      }
    }
  }

  return result;
}

void handle_file_error(const char *file_name, const grep_options_t *flags) {
  if (!flags->suppress_errors) {
    fprintf(stderr, "s21_grep: %s: No such file or directory\n", file_name);
  }
}

int should_print_file_name(const grep_options_t *flags,
                           const vector_t *file_names) {
  return (flags->no_filename || file_names->length > 1);
}
