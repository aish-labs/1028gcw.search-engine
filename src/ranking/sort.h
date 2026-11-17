#ifndef SORT_H
#define SORT_H
#include "../index/index.h"

typedef struct {
    uint64_t doc_id;
    size_t score;
} RankingEntry;

int sort_by_score(RankingEntry results[], int count);
int calculate_score(int times[], int word_count);
int rank_word(
    const Index *index,
    const char *word,
    RankingEntry results[],
    int capacity,
    int *result_count
);
#endif
