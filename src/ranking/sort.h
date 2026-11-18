#ifndef SORT_H
#define SORT_H
#include "../index/index.h"

typedef struct RankingEntry{
    uint64_t doc_id;
    size_t score;
} RankingEntry;

int search_find(  
    const Index *index,
    const char *word,
    RankingEntry **results,
    int *result_count
);
// searches one normalized words, scores matche, and sorts results(by decending score)

void free_search_results(RankingEntry *results);

#endif
