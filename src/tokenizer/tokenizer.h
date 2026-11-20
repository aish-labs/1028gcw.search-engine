#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stddef.h>

typedef struct {
    char **words;       // Kept words in input order, including repetitions.
    size_t total_words;
    size_t unique_words;
    char *text_copy;
} Tokens;

// Initialize the result with Tokens tokens = {0} before the first call.
// On success, replace the result and return 0. On error, keep the old result.
// Return -1 for invalid arguments or -2 if memory cannot be allocated.
// Read the words until the result is replaced or released with free_tokens.
int tokenize(const char text[], Tokens *out);

void free_tokens(Tokens *tokens);

#endif
