#include "tokenizer.h"

#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static int is_word_char(unsigned char ch)
{
    return ch <= 127 && isalnum(ch);
}

static size_t measure_text(const char text[], size_t *length)
{
    size_t i = 0;
    size_t count = 0;
    int inside_word = 0;

    while (text[i] != '\0') {
        int is_letter_or_digit = is_word_char((unsigned char)text[i]);
        if (is_letter_or_digit && !inside_word) {
            ++count;
        }
        inside_word = is_letter_or_digit;
        ++i;
    }
    *length = i;
    return count;
}

static int is_stop_word(const char word[])
{
    static const char *const stop_words[] = {
        "a", "an", "and", "are", "as", "at", "be", "but", "by", "for", "from",
        "has", "have", "he", "in", "is", "it", "of", "on", "or", "that", "the",
        "this", "to", "was", "were", "will", "with"
    };

    for (size_t i = 0; i < sizeof stop_words / sizeof stop_words[0]; ++i) {
        if (strcmp(word, stop_words[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

static int already_seen(const Tokens *tokens, const char word[])
{
    for (size_t i = 0; i < tokens->total_words; ++i) {
        if (strcmp(word, tokens->words[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int tokenize(const char text[], Tokens *out)
{
    if (text == NULL || out == NULL) {
        return -1;
    }

    size_t length;
    size_t capacity = measure_text(text, &length);
    if (capacity == 0) {
        free_tokens(out);
        return 0;
    }
    if (capacity > SIZE_MAX / sizeof(char *)) {
        return -2;
    }

    Tokens result = {0};
    result.text_copy = malloc(length + 1);
    result.words = malloc(capacity * sizeof *result.words);
    if (result.text_copy == NULL || result.words == NULL) {
        free_tokens(&result);
        return -2;
    }
    char *word = NULL;
    // Include the final NUL so it also finishes the last word.
    for (size_t i = 0; i <= length; ++i) {
        unsigned char ch = (unsigned char)text[i];
        result.text_copy[i] = text[i];

        if (is_word_char(ch)) {
            if (word == NULL) {
                word = &result.text_copy[i];
            }
            result.text_copy[i] = (char)tolower(ch);
        } else if (word != NULL) {
            result.text_copy[i] = '\0';
            if (!is_stop_word(word)) {
                if (!already_seen(&result, word)) {
                    ++result.unique_words;
                }
                result.words[result.total_words] = word;
                ++result.total_words;
            }
            word = NULL;
        }
    }

    if (result.total_words == 0) {
        free_tokens(&result);
    }
    free_tokens(out);
    *out = result;
    return 0;
}

void free_tokens(Tokens *tokens)
{
    if (tokens != NULL) {
        free(tokens->words);
        free(tokens->text_copy);
        tokens->words = NULL;
        tokens->text_copy = NULL;
        tokens->total_words = 0;
        tokens->unique_words = 0;
    }
}
