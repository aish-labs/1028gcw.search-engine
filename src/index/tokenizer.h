#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stddef.h>

typedef struct Tokens {
    size_t total_words;
    size_t unique_words; // Distinct terms within this document or query.
    char *text_copy;
    char *words[]; // Exactly total_words pointers to slices of text_copy, in input order.
} Tokens;

// Keep the original Tokens allocation and text_copy in place. Do not copy
// Tokens or realloc text_copy while words points into it.
// Set *out to NULL before the first call. On success, replace any existing result.
// On error, leave *out intact. Use free_tokens to release the result.
// Returns 0 on success, -1 for invalid arguments, -2 for allocation failure,
// and -3 if the file cannot be opened or read.
int tokenize(const char *path, Tokens **out);

// Apply the same rules to a NUL-terminated query.
int tokenize_text(const char *text, Tokens **out);

void free_tokens(Tokens *tokens);

#endif
