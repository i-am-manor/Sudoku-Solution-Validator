#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 9

int sudoku[SIZE][SIZE] = {
    {5,3,4,6,7,8,9,1,2},
    {6,7,2,1,9,5,3,4,8},
    {1,9,8,3,4,2,5,6,7},
    {8,5,9,7,6,1,4,2,3},
    {4,2,6,8,5,3,7,9,1},
    {7,1,3,9,2,4,8,5,6},
    {9,6,1,5,3,7,2,8,4},
    {2,8,7,4,1,9,6,3,5},
    {3,4,5,2,8,6,1,7,9}
};

typedef struct {
    int start_row;
    int start_col;
    int index;
} parameters;

// Results: 1 = valid, 0 = invalid
int rows_valid = 0;
int cols_valid = 0;
int subgrids_valid[9] = {0};

// Check all rows
void* check_rows(void* arg) {
    for (int i = 0; i < SIZE; i++) {
        int nums[SIZE+1] = {0};
        for (int j = 0; j < SIZE; j++) {
            int num = sudoku[i][j];
            if (num < 1 || num > 9 || nums[num] == 1) {
                pthread_exit(NULL);
            }
            nums[num] = 1;
        }
    }
    rows_valid = 1;
    pthread_exit(NULL);
}

// Check all columns
void* check_cols(void* arg) {
    for (int j = 0; j < SIZE; j++) {
        int nums[SIZE+1] = {0};
        for (int i = 0; i < SIZE; i++) {
            int num = sudoku[i][j];
            if (num < 1 || num > 9 || nums[num] == 1) {
                pthread_exit(NULL);
            }
            nums[num] = 1;
        }
    }
    cols_valid = 1;
    pthread_exit(NULL);
}

// Check a 3x3 subgrid
void* check_subgrid(void* param) {
    parameters* p = (parameters*) param;
    int start_row = p->start_row;
    int start_col = p->start_col;
    int index = p->index;

    int nums[SIZE+1] = {0};
    for (int i = start_row; i < start_row + 3; i++) {
        for (int j = start_col; j < start_col + 3; j++) {
            int num = sudoku[i][j];
            if (num < 1 || num > 9 || nums[num] == 1) {
                pthread_exit(NULL);
            }
            nums[num] = 1;
        }
    }
    subgrids_valid[index] = 1;
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[11];
    parameters* data[9];

    // Create thread to check rows
    pthread_create(&threads[0], NULL, check_rows, NULL);

    // Create thread to check columns
    pthread_create(&threads[1], NULL, check_cols, NULL);

    // Create 9 threads to check each subgrid
    int thread_index = 2;
    int subgrid_index = 0;
    for (int i = 0; i < SIZE; i += 3) {
        for (int j = 0; j < SIZE; j += 3) {
            data[subgrid_index] = (parameters*) malloc(sizeof(parameters));
            data[subgrid_index]->start_row = i;
            data[subgrid_index]->start_col = j;
            data[subgrid_index]->index = subgrid_index;
            pthread_create(&threads[thread_index++], NULL, check_subgrid, data[subgrid_index]);
            subgrid_index++;
        }
    }

    for (int i = 0; i < 11; i++) {
        pthread_join(threads[i], NULL);
    }


    int valid = rows_valid && cols_valid;
    for (int i = 0; i < 9; i++) {
        valid = valid && subgrids_valid[i];
    }

    if (valid) {
        printf("The Sudoku puzzle is valid.\n");
    } else {
        printf("The Sudoku puzzle is invalid!\n");
    }

    for (int i = 0; i < 9; i++) {
        free(data[i]);
    }

    return 0;
}
