#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#define SIZE 9

typedef struct 
{
    int row;
    int column;
    int (*sudoku)[SIZE];
} parameters;

void *check_board(void *param)
{
    parameters *miniboard = (parameters *)param;
    int row = miniboard->row;
    int column = miniboard->column;
    int (*sudoku)[SIZE] = miniboard->sudoku;
    bool valid[SIZE] = {false};
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

    //create parameters for each board.
    parameters *param[9];
    int cnt = 0;
    for (int i = 0; i < 9; i+=3)
    {
        for (int j = 0; j < 9; j+=3)
        {
            param[cnt] = (parameters *)malloc(sizeof(parameters));
            param[cnt]->row = i;
            param[cnt]->column = j;
            param[cnt]->sudoku = sudoku;
            cnt++;
        }
    }
    //create thread for each board.
    pthread_t threadboard[9];
    for (int i = 0; i < 9; i++)
    {
        if (pthread_create(&threadboard[i], NULL, check_board, param[i]) != 0)
        {
            perror("Failed to create thread");
            return 1;
        }
    }

    void *result[9];

    for (int i = 0; i < 9; i++)
    {
        if (pthread_join(threadboard[i], &result[i]) != 0)
        {
            perror("Failed to join thread");
            return 1;
        }
    }

    for (int i = 0; i < 9; i++)
    {
        if((int *)result[i] == 0)
        {
            printf("Sudoku is invalid\n");
            return 1;
        }
    }
    printf("Sudoku is valid\n");
    return 0;
}