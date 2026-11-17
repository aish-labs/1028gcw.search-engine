#ifndef SEARCH_INDEX_H
#define SEARCH_INDEX_H

#include <stddef.h>
#include <stdint.h>

/* One (term, file hash) relationship in a term's posting list. */
typedef struct Posting {
    uint64_t doc_id;       /* FileEntry.hash identifying the document. */
    size_t count;         /* Occurrences of this term in this document. */
    struct Posting *next; /* Next posting, or NULL at the end. */
} Posting;

/* One normalized term and the documents containing it. */
typedef struct Term {
    Posting *head;     /* First posting, or NULL for an empty list. */
    char *word;        /* Null-terminated copy owned by the index. */
    struct Term *next; /* Next term, or NULL at the end. */
} Term;

/* Linked-list index and its statistics. Initialize with Index index = {0}. */
typedef struct Index {
    Term *head;           /* First term, or NULL for an empty index. */
    size_t word_count;    /* Number of distinct terms, not total occurrences. */
    size_t posting_count; /* Number of distinct (term, file hash) relationships. */
} Index;

/*
 * Add one occurrence of a normalized word, using FileEntry.hash as doc_id.
 * Copy a new word into index-owned memory; the caller may reuse its buffer.
 * Increase count for an existing (word, doc_id), otherwise add a posting.
 * Equal hashes share a document identity in this interface.
 * Return 0 on success, -1 for NULL index/word or an empty word,
 * or -2 if memory allocation fails (the existing index remains valid).
 */
int index_add_term(Index *index, const char *word, uint64_t doc_id);

/*
 * Look up one normalized word and write its first posting to *out.
 * Return 0 when complete; *out is NULL if the word is absent.
 * Return -1 for NULL index/word/out or an empty word.
 * When out is valid, clear *out before checking the other arguments.
 * The caller only reads the borrowed nodes and must not free them.
 * Follow next to read all matches; no ranking or sorting is performed.
 * Existing nodes stay valid until index_free; additions may change counts.
 */
int index_lookup(const Index *index, const char *word, Posting **out);

/*
 * Free all postings, word copies and terms owned by the index.
 * Reset head and both counts to zero; the Index object itself is not freed.
 * Accept NULL and an already cleared index. Borrowed nodes then become invalid.
 */
void index_free(Index *index);

#endif
