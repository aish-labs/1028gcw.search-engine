#include "tokenizer.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int read_document(const char *path, char **text, size_t *length)
{
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        return -3;
    }

    char *buffer = NULL;
    size_t used = 0;
    size_t capacity = 0;
    int status = 0;

    for (;;) {
        if (capacity - used < 2) {
            if (capacity > SIZE_MAX / 2) {
                status = -2;
                break;
            }
            size_t next_capacity = capacity == 0 ? 4096 : capacity * 2;
            char *next = realloc(buffer, next_capacity);
            if (next == NULL) {
                status = -2;
                break;
            }
            buffer = next;
            capacity = next_capacity;
        }

        size_t space = capacity - used - 1; // Leave room for a final NUL.
        size_t bytes = fread(buffer + used, 1, space, file);
        used += bytes;
        if (bytes < space) {
            if (ferror(file)) {
                status = -3;
            }
            break;
        }
    }

    if (fclose(file) != 0 && status == 0) {
        status = -3;
    }
    if (status != 0) {
        free(buffer);
        return status;
    }
    buffer[used] = '\0';
    *text = buffer;
    *length = used;
    return 0;
}

static size_t word_char_size(const char *text, size_t remaining)
{
    if (remaining == 0) {
        return 0;
    }
    unsigned char first = (unsigned char)text[0];
    if (first < 128) {
        return isalnum(first) != 0;
    }
    // UTF-8 letters U+00C0-U+00FF, except multiplication and division signs.
    if (first == 0xC3 && remaining >= 2) {
        unsigned char second = (unsigned char)text[1];
        if (second >= 0x80 && second <= 0xBF &&
            second != 0x97 && second != 0xB7) {
            return 2;
        }
    }
    return 0;
}

static void copy_lowercase(char *dest, const char *src, size_t width)
{
    if (width == 1) {
        dest[0] = (char)tolower((unsigned char)src[0]);
    } else {
        unsigned char second = (unsigned char)src[1];
        if (second >= 0x80 && second <= 0x9E) {
            second += 0x20;
        }
        dest[0] = src[0];
        dest[1] = (char)second;
    }
}

static size_t apostrophe_size(const char *text, size_t remaining)
{
    if (remaining > 0 && text[0] == '\'') {
        return 1;
    }
    if (remaining >= 3 && memcmp(text, "\xE2\x80\x99", 3) == 0) {
        return 3; // UTF-8 right apostrophe.
    }
    return 0;
}

static int is_stop_word(const char *word)
{
    static const char *const stop_words[] = {
        "a", "an", "and", "are", "as", "at", "be", "been", "being", "but", "by",
        "for", "from", "had", "has", "have", "he", "her", "hers", "him", "his",
        "i", "i'm", "i've", "in", "is", "it", "it's", "its", "me", "my", "of",
        "on", "or", "our", "ours", "she", "that", "the", "their", "theirs",
        "them", "they", "they're", "this", "to", "us", "was", "we", "we're",
        "we've", "were", "will", "with", "you", "you're", "your", "yours"
    };

    for (size_t i = 0; i < sizeof stop_words / sizeof stop_words[0]; ++i) {
        if (strcmp(word, stop_words[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Pack retained words at the front of text, each followed by a NUL.
static size_t split_words(char *text, size_t length)
{
    size_t read = 0;
    size_t write = 0;
    size_t count = 0;

    while (read < length) {
        size_t width = word_char_size(text + read, length - read);
        if (width == 0) {
            ++read;
            continue;
        }

        size_t start = write;
        do {
            copy_lowercase(text + write, text + read, width);
            write += width;
            read += width;

            size_t quote = apostrophe_size(text + read, length - read);
            if (quote != 0 && word_char_size(text + read + quote, length - read - quote)) {
                text[write++] = '\'';
                read += quote;
            }
            width = word_char_size(text + read, length - read);
        } while (width != 0);

        // The next byte is a separator or the end of input. Reserve it
        // before writing the terminator in case read and write coincide.
        if (read < length) {
            ++read;
        }
        text[write++] = '\0';
        if (is_stop_word(text + start)) {
            write = start;
        } else {
            ++count;
        }
    }
    return count;
}

static int compare_words(const void *left, const void *right)
{
    char *const *a = left;
    char *const *b = right;
    return strcmp(*a, *b);
}

static int count_unique_words(Tokens *tokens)
{
    if (tokens->total_words < 2) {
        tokens->unique_words = tokens->total_words;
        return 0;
    }

    // Sort a temporary array so the public word array stays in input order.
    size_t bytes = tokens->total_words * sizeof tokens->words[0];
    char **sorted = malloc(bytes);
    if (sorted == NULL) {
        return -2;
    }
    memcpy(sorted, tokens->words, bytes);
    qsort(sorted, tokens->total_words, sizeof sorted[0], compare_words);

    tokens->unique_words = 1;
    for (size_t i = 1; i < tokens->total_words; ++i) {
        if (strcmp(sorted[i - 1], sorted[i]) != 0) {
            ++tokens->unique_words;
        }
    }
    free(sorted);
    return 0;
}

static int make_tokens(char *text, size_t length, Tokens **out)
{
    size_t count = split_words(text, length);
    if (count > (SIZE_MAX - sizeof(Tokens)) / sizeof(char *)) {
        free(text);
        return -2;
    }
    Tokens *tokens = malloc(sizeof *tokens + count * sizeof tokens->words[0]);
    if (tokens == NULL) {
        free(text);
        return -2;
    }

    tokens->total_words = count;
    tokens->text_copy = text;
    char *word = text;
    for (size_t i = 0; i < count; ++i) {
        tokens->words[i] = word;
        word += strlen(word) + 1;
    }
    if (count == 0) {
        free(tokens->text_copy);
        tokens->text_copy = NULL;
    }
    if (count_unique_words(tokens) != 0) {
        free_tokens(tokens);
        return -2;
    }

    free_tokens(*out);
    *out = tokens;
    return 0;
}

int tokenize(const char *path, Tokens **out)
{
    if (path == NULL || out == NULL) {
        return -1;
    }
    char *text;
    size_t length;
    int status = read_document(path, &text, &length);
    if (status != 0) {
        return status;
    }
    return make_tokens(text, length, out);
}

int tokenize_text(const char *text, Tokens **out)
{
    if (text == NULL || out == NULL) {
        return -1;
    }
    size_t length = strlen(text);
    char *copy = malloc(length + 1);
    if (copy == NULL) {
        return -2;
    }
    memcpy(copy, text, length + 1);
    return make_tokens(copy, length, out);
}

void free_tokens(Tokens *tokens)
{
    if (tokens != NULL) {
        free(tokens->text_copy);
        free(tokens);
    }
}
