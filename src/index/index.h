#ifndef SEARCH_INDEX_H
#define SEARCH_INDEX_H

#include <stddef.h>
#include <stdint.h>

// One term-document pair.
typedef struct Posting {
    uint64_t doc_id;       // FileEntry.hash.
    size_t count;         // Term count.
    struct Posting *next;
} Posting;

// Term and posting list.
typedef struct Term {
    Posting *head;
    char *word;           // Owned word copy.
    struct Term *next;
} Term;

// Start with: Index index = {0};
typedef struct Index {
    Term *head;
    size_t word_count;    // Distinct terms.
    size_t posting_count; // Term-document pairs.
} Index;

// Add one normalized word occurrence; doc_id is FileEntry.hash.
// 0: success; -1: invalid input; -2: malloc failed.
int index_add_term(Index *index, const char *word, uint64_t doc_id);

// Find a normalized word; write the posting list to *out.
// 0: success (NULL if absent); -1: invalid input.
// Returned nodes are read-only and owned by the index.
int index_lookup(const Index *index, const char *word, Posting **out);

// Free contents and reset the index; lookup results expire.
void free_index(Index *index);

#endif
