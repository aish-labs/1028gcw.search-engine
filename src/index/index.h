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

// Add one occurrence of a word to the index.
//
// index: initialized index to update.
// word: normalized word. The index copies new words.
// doc_id: FileEntry.hash of the document.
//
// Returns:
//   0 => success;
//  -1 => index or word is NULL, or word is empty;
//  -2 => memory allocation failed.
int index_add_term(Index *index, const char *word, uint64_t doc_id);

// Find the documents containing a word.
//
// index: initialized index to search.
// word: normalized word to look up.
// out: address of the result pointer. *out is NULL if there is no match.
//      The index owns the returned nodes. Read them without freeing them.
//
// Returns:
//   0 => lookup completed, including when there is no match;
//  -1 => index, word or out is NULL, or word is empty.
int index_lookup(const Index *index, const char *word, Posting **out);

// Free the index contents and reset its counts.
//
// index: index to clear. NULL is allowed.
//        Previous lookup results become invalid.
//
// Returns no value.
void free_index(Index *index);

#endif
