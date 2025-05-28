#include "task4.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void dijkstra(const char* ref_line, const char* compare_line, FILE *fout) {
   unsigned int ref_no = strlen(ref_line);
    unsigned int compare_no = strlen(compare_line);
    int j;

    // allocate the cost arrays.
    int *previous = malloc((compare_no + 1) * sizeof(int));
    int *current = malloc((compare_no + 1) * sizeof(int));

    //arrays to track counts for replacements, deletions, insertions for both current and previous.
    int *previous_replacement = malloc((compare_no + 1) * sizeof(int));
    int *previous_deletion = malloc((compare_no + 1) * sizeof(int));
    int *previous_insertion = malloc((compare_no + 1) * sizeof(int));

    int *current_replacement = malloc((compare_no + 1) * sizeof(int));
    int *current_deletion = malloc((compare_no + 1) * sizeof(int));
    int *current_insertion = malloc((compare_no + 1) * sizeof(int));

    if (!previous || !current || !previous_replacement || !previous_deletion || !previous_insertion || !current_replacement || !current_deletion || !current_insertion) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Initialize the first row.
    previous[0] = 0;
    previous_replacement[0] = previous_deletion[0] = previous_insertion[0] = 0;
    for (j = 1; j <= compare_no; j++) {
        previous[j] = j;                   // Cost: j insertions.
        previous_replacement[j] = 0;
        previous_deletion[j] = 0;
        previous_insertion[j] = j;          // All operations are insertions.
    }

    // Process each character of ref_line.
    for (int i = 1; i <= ref_no; i++) {
        current[0] = i;                   // Cost: i deletions.
        current_replacement[0] = 0;
        current_deletion[0] = i;          // All operations are deletions.
        current_insertion[0] = 0;

        for (j = 1; j <= compare_no; j++) {
            int cost = ref_line[i - 1] == compare_line[j - 1] ? 0 : 2;
            int diag = previous[j - 1] + cost;
            int deletion = previous[j] + 1;       // Deletion cost.
            int insertion = current[j - 1] + 1;     // Insertion cost.

            if (diag <= deletion && diag <= insertion) {
                current[j] = diag;
                current_replacement[j] = previous_replacement[j - 1] + ((cost == 2) ? 1 : 0);
                current_deletion[j] = previous_deletion[j - 1];
                current_insertion[j] = previous_insertion[j - 1];
            } else if (deletion <= insertion) {
                current[j] = deletion;
                current_replacement[j] = previous_replacement[j];
                current_deletion[j] = previous_deletion[j] + 1;
                current_insertion[j] = previous_insertion[j];
            } else {
                current[j] = insertion;
                current_replacement[j] = current_replacement[j - 1];
                current_deletion[j] = current_deletion[j - 1];
                current_insertion[j] = current_insertion[j - 1] + 1;
            }
        }

        memcpy(previous, current, (compare_no + 1) * sizeof(int));
        memcpy(previous_replacement, current_replacement, (compare_no + 1) * sizeof(int));
        memcpy(previous_deletion, current_deletion, (compare_no + 1) * sizeof(int));
        memcpy(previous_insertion, current_insertion, (compare_no + 1) * sizeof(int));
    }

    fprintf(stderr, "Minimum changes required: %d\n", previous[compare_no]);
    fprintf(stderr, "Replacements: %d, Deletions: %d, Insertions: %d\n\n", previous_replacement[compare_no], previous_deletion[compare_no], previous_insertion[compare_no]);
    fprintf(fout, "%dD %dI %dR \n", previous_deletion[compare_no], previous_insertion[compare_no], previous_replacement[compare_no]);

    free(previous);
    free(current);
    free(previous_replacement);
    free(previous_deletion);
    free(previous_insertion);
    free(current_replacement);
    free(current_deletion);
    free(current_insertion);
}