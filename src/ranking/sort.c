#include "sort.h"
#include <stdio.h>

int sort_by_score(RankingEntry results[], int count)
{
    int i, j;
    RankingEntry tmp;

    // Check parameters
    if (count < 0)
    {
        return -1; // Count cannot be a negative number
    }

    if (count == 0)
    {
        return 0; // There are no results to sort
    }

    if (results == NULL)
    {
        return -1; // The result array cannot be NULL
    }

    // Swap if the next score is larger.
    for (i = 0; i < count - 1; i++)
    {
        for (j = 0; j < count - 1 - i; j++)
        {
            if (results[j].score < results[j + 1].score)
            {
                // Swap the whole entry: document ID and score
                tmp = results[j];
                results[j] = results[j + 1];
                results[j + 1] = tmp;
            }
        }
    }

    return 0;
}


//Sum all elements in array to get total score.
int calculate_score(int times[], int word_count)
{
    int total = 0;
    int i;

    for (i = 0; i < word_count; i++)
    {
        total = total + times[i];
    }

    return total;
}


int rank_word(
    const Index *index,
    const char *word,
    RankingEntry results[],
    int capacity,
    int *result_count)
{
    Posting *p = NULL;
    int error;
    int n = 0;

    // Check parameters.
    if (result_count == NULL)
        return -1;

    *result_count = 0;

    if (index == NULL || word == NULL || capacity < 0)
        return -1;

    if (capacity > 0 && results == NULL)
        return -1;

    // Look up one normalized word.
    error = index_lookup(index, word, &p);

    if (error != 0)
        return error;

    // Copy each matching document and its score.
    while (p != NULL)
    {
        if (n >= capacity)
            return -2; // Not enough space for all matches.

        results[n].doc_id = p->doc_id;
        results[n].score = p->count;

        n++;
        p = p->next;
    }

    // Sort the collected results.
    error = sort_by_score(results, n);

    if (error != 0)
        return error;

    *result_count = n;
    return 0;
}