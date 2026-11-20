#ifndef SORT_H
#define SORT_H
#include "../index/index.h"

typedef struct RankingEntry{
    uint64_t doc_id;
    size_t score;
} RankingEntry;

// Searches one normalized words, scores matche
// and sorts results(by decending score).

int search(  
    const Index *index,
    const char *word,
    RankingEntry *results[],
    size_t *result_count
);

void free_search_results(RankingEntry results[]);

#endif
