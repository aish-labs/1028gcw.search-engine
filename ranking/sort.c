#include "sort.h"
#include <stdio.h>

int sort_by_score(int file_number[], int file_score[], int count){

    int tmp, i, j;  

// checking parameter
    if(count < 0){
        return -1; // Count cannot be a negative number.
    }
    if(count==0){
        return 0; // These are no files to sort.
    }
    if (file_number == NULL || file_score == NULL || file_number == file_score){
        return -1;
    }

//Swap if the next score is larger
    for (i = 0; i < count - 1; i++)
{
    for (j = 0; j < count - 1 - i; j++)
    {
        if (file_score[j] < file_score[j + 1])
        {
            tmp = file_score[j];
            file_score[j] = file_score[j + 1];
            file_score[j + 1] = tmp;
//Swap the file_number along with file_score
            tmp = file_number[j];
            file_number[j] = file_number[j + 1];
            file_number[j + 1] = tmp;
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
