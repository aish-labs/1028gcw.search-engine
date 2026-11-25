#ifndef SEARCH_INDEX_H
#define SEARCH_INDEX_H

#include <stddef.h>
#include <stdint.h>

typedef struct Posting {
    uint64_t doc_id;
    size_t count;
    struct Posting *next;
} Posting;

typedef struct Term {
    Posting *head;
    char *word;
    struct Term *next;
} Term;

typedef struct Index {
    Term *head;
    size_t word_count;
    size_t posting_count;
} Index;

// Add one word occurrence to the index.
//
// word: normalized word, copied when first added.
// doc_id: FileEntry.hash.
//
// Returns:
//    0: success
//   -1: invalid input
//   -2: allocation failed
int index_add_term(Index *index, const char *word, uint64_t doc_id);

// Find the documents containing a normalized word.
//
// out: receives the posting list, or NULL if no match.
// The index owns the list. Do not modify or free it.
//
// Returns:
//    0: success
//   -1: invalid input
int index_lookup(const Index *index, const char *word, Posting **out);

// Free and reset the index.
void free_index(Index *index);

#endif
