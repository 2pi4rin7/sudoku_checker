#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#define SIZE 9

//parameters for each subgrid.
typedef struct 
{
    int row;
    int column;
    int (*sudoku)[SIZE];
} parameters;

//void* means it returns a pointer to void, which is a generic pointer type.
void *check_subgrid(void *param)
{
    parameters *miniboard = (parameters *)param;
    int row = miniboard->row;
    int column = miniboard->column;
    int (*sudoku)[SIZE] = miniboard->sudoku;
    bool valid[SIZE] = {false}; //bitmask to check subgrid.
    for (int i = row; i < row + 3; i++)
    {
        for (int j = column; j < column + 3; j++)
        {
            if (valid[sudoku[i][j] - 1])
            {
                return (void *) 0;
            }
            else valid[sudoku[i][j] - 1] = true;
        }
    }
    return (void *) 1;
}

void *check_cols(void *param)
{
    int (*sudoku)[SIZE] = (int (*)[SIZE])param;
    for (int col = 0; col < SIZE; col++)
    {
        bool valid[SIZE] = {false};//bitmask to check columns.
        for (int row = 0; row < SIZE; row++)
        {
            if (valid[sudoku[row][col] - 1])
            {
                return (void *)0;
            }
            else
                valid[sudoku[row][col] - 1] = true;
        }
    }
    return (void *)1;
}

void *check_rows(void *param)
{
    int (*sudoku)[SIZE] = (int (*)[SIZE])param;
    for (int row = 0; row < SIZE; row++)
    {
        bool valid[SIZE] = {false};//bitmask to check rows.
        for (int col = 0; col < SIZE; col++)
        {
            if (valid[sudoku[row][col] - 1])
            {
                return (void *)0;
            }
            else
                valid[sudoku[row][col] - 1] = true;
        }
    }
    return (void *)1;
}


int main()
{   
    int sudoku[9][9] = {{6, 2, 4, 5, 3, 9, 1, 8, 7},
                        {5, 1, 9, 7, 2, 8, 6, 3, 4},
                        {8, 3, 7, 6, 1, 4, 2, 9, 5},
                        {1, 4, 3, 8, 6, 5, 7, 2, 9},
                        {9, 5, 8, 2, 4, 7, 3, 6, 1},
                        {7, 6, 2, 3, 9, 1, 4, 5, 8},
                        {3, 7, 1, 9, 5, 6, 8, 4, 2},
                        {4, 9, 6, 1, 8, 2, 5, 7, 3},
                        {2, 8, 5, 4, 7, 3, 9, 1, 6}};

    //create parameters for each subgrid.
    parameters *param[9];
    int cnt = 0;
    for (int i = 0; i < 9; i+=3)
    {
        for (int j = 0; j < 9; j+=3)
        {
            param[cnt] = (parameters *)malloc(sizeof(parameters));
            param[cnt]->row = i; //starting row of subgrid.
            param[cnt]->column = j;//starting column of subgrid.
            param[cnt]->sudoku = sudoku;
            cnt++;
        }
    }
    //create thread for each subgrid.
    pthread_t thread_subgrid[9];
    for (int i = 0; i < 9; i++)
    {
        if (pthread_create(&thread_subgrid[i], NULL, check_subgrid, param[i]) != 0)
        {
            perror("Failed to create thread");
            return 1;
        }
    }

    //two threads for checking rows and columns.
    pthread_t thread_row, thread_col;

    if (pthread_create(&thread_row, NULL, check_rows, sudoku) != 0)
    {
        perror("Failed to create row thread");
        return 1;
    }

    if (pthread_create(&thread_col, NULL, check_cols, sudoku) != 0)
    {
        perror("Failed to create column thread");
        return 1;
    }

    // store the results of 11 threads
    void *result_subgrid[9], *result_row, *result_col;

    for (int i = 0; i < 9; i++)
    {
        if (pthread_join(thread_subgrid[i], &result_subgrid[i]) != 0)
        {
            perror("Failed to join board thread");
            return 1;
        }
        
        //check subgrid bitmask
        if ((intptr_t)result_subgrid[i] == 0)
        {
            printf("Sudoku is invalid\n");
            return 1;
        }
    }

    if (pthread_join(thread_row, &result_row) != 0)
    {
        perror("Failed to join row thread");
        return 1;
    }

    if (pthread_join(thread_col, &result_col) != 0)
    {
        perror("Failed to join column thread");
        return 1;
    }

    //check row and column 
    if ((intptr_t)result_row == 0 || (intptr_t)result_col == 0)
    {
        printf("Sudoku is invalid\n");
        return 1;
    }

    printf("Sudoku is valid\n");
    return 0;
}