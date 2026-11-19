#include "sort.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

static int sort_by_score(RankingEntry results[], int count)
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


int search(
    const Index *index,
    const char *word,
    RankingEntry *results[],
    int *result_count)
{
    Posting *head = NULL;
    Posting *p;
    RankingEntry *array;
    int error;
    int count = 0;
    int i = 0;

    // Check parameters.
    if (result_count == NULL || results == NULL)
        return -1;

    *results = NULL;
    *result_count = 0;

    if (index == NULL || word == NULL)
        return -1;

    // Look up one normalized word.
    error = index_lookup(index, word, &head);

    if (error != 0)
        return error;

    // Copy each matching document and its score
    p = head;
    while (p != NULL)
    {
        if (count == INT_MAX)
            return -2; // Not enough space for all matches

        count++;
        p = p->next;
    }

     if (count == 0)
        return 0; // No matches


    // Check the allocation size.
    if ((size_t)count > SIZE_MAX / sizeof(RankingEntry))
        return -2;

    // Allocate space for all results.
    array = malloc((size_t)count * sizeof(RankingEntry));

    if (array == NULL)
        return -2;


    // Copy document IDs and use word counts as scores
    p = head;
    while (p != NULL)
    {
        array[i].doc_id = p->doc_id;
        array[i].score = p->count;

        i++;
        p = p->next;
    }

    // Sort by descending score.
    error = sort_by_score(array, count);

    if (error != 0)
    {
        free(array);
        return error;
    }

    // Return the allocated array and its length
    *results = array;
    *result_count = count;

    return 0;
}

// Search ends here
void free_search_results(RankingEntry *results)
{
    free(results);
}