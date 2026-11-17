#ifndef SEARCH_INDEX_H
#define SEARCH_INDEX_H
#include <stddef.h>

typedef struct Posting {
    int doc_id;
    size_t frequency;
    struct Posting *next;
} Posting;

typedef struct Term {
    char *word;
    Posting *head;
    struct Term *next;
} Term;

typedef struct Index {
    Term *first;
    size_t words;
    size_t postings;
} Index;

/* Start with: Index index = {0}; */
/* Add one occurrence of a normalized word. 0: OK, -1: bad input, -2: no memory. */
int index_add_term(Index *index, const char *word, int doc_id);

/* 0: completed (NULL if absent), -1: bad input. Borrowed list: read only; do not free. */
int index_lookup(const Index *index, const char *word, Posting **out);

void index_free(Index *index);

#endif

