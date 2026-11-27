#include "index.h"
#include <stdlib.h>
#include <string.h>

static Term *find_term(const Index *index, const char *word)
{
    Term *term = index->head;

    while (term != NULL) {
        if (strcmp(term->word, word) == 0) {
            return term;
        }
        term = term->next;
    }

    return NULL;
}

int index_add_term(Index *index, const char *word, uint64_t doc_id)
{
    Term *term;
    Posting *p;

    if (index == NULL || word == NULL || word[0] == '\0') {
        return -1;
    }

    term = find_term(index, word);

    if (term != NULL) {
        p = term->head;

        while (p != NULL) {
            if (p->doc_id == doc_id) {
                p->count++;
                return 0;
            }
            p = p->next;
        }
    }

    p = malloc(sizeof *p);
    if (p == NULL) {
        return -1;
    }

    if (term == NULL) {
        term = malloc(sizeof *term);
        if (term == NULL) {
            free(p);
            return -1;
        }

        term->word = malloc(strlen(word) + 1);
        if (term->word == NULL) {
            free(term);
            free(p);
            return -1;
        }

        strcpy(term->word, word);
        term->head = NULL;
        term->next = index->head;
        index->head = term;
    }

    p->doc_id = doc_id;
    p->count = 1;
    p->next = term->head;
    term->head = p;

    return 0;
}

int index_lookup(const Index *index, const char *word, Posting **out)
{
    Term *term;

    if (out == NULL) {
        return -1;
    }

    *out = NULL;

    if (index == NULL || word == NULL || word[0] == '\0') {
        return -1;
    }

    term = find_term(index, word);
    if (term != NULL) {
        *out = term->head;
    }

    return 0;
}

void free_index(Index *index)
{
    if (index == NULL) {
        return;
    }

    while (index->head != NULL) {
        Term *term = index->head;
        index->head = term->next;

        while (term->head != NULL) {
            Posting *p = term->head;
            term->head = p->next;
            free(p);
        }

        free(term->word);
        free(term);
    }

    index->word_count = 0;
    index->posting_count = 0;
}
