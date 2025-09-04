#include "s21_vector.h"

char *strdup(const char *str) {
  if (str == NULL) return NULL;
  size_t len = strlen(str) + 1;
  char *copy = malloc(len);
  if (copy) {
    memcpy(copy, str, len);
  }
  return copy;
}

void vector_init(vector_t *vec) {
  vec->data = NULL;
  vec->length = 0;
  vec->capacity = 0;
  vec->step = STEP;
}

void vector_push_back(vector_t *vec, const char *str) {
  if (vec->length == vec->capacity) {
    vec->capacity += vec->step;
    vec->data = realloc(vec->data, vec->capacity * sizeof(char *));
    if (!vec->data) {
      perror("Failed to reallocate memory");
    }
  }

  vec->data[vec->length] = strdup(str);
  if (!vec->data[vec->length]) {
    perror("Failed to allocate memory for string");
  }
  vec->length++;
}

void vector_free(vector_t *vec) {
  for (size_t i = 0; i < vec->length; i++) {
    free(vec->data[i]);
  }
  free(vec->data);
  vec->data = NULL;
  vec->length = 0;
  vec->capacity = 0;
}
