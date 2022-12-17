#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "lab2.h"


int** read_board_from_file(char* filename){
    FILE *fp = NULL;
    int** board = (int**)malloc(sizeof(int*)*ROW_SIZE);
    for(int row = 0; row < ROW_SIZE; row++){
	    board[row] = (int*)malloc(sizeof(int)*COL_SIZE);
    }

    fp = fopen(filename, "r");

    for(int row = 0; row < ROW_SIZE; row++){
        for(int col =0; col < COL_SIZE; col++){
            fscanf(fp, "%d,", &board[row][col]);
        }
    }
    fclose(fp);

    return board;
}

void* rowValidate(void* param){
    param_struct * here = (param_struct*)param;
    int check = 0;
    for (int b = 0; b< ROW_SIZE; b++){
        check = check + sudoku_board[here->starting_row][b];
    }
    /*printf("%d\n",check);*/
    if (check > 45){
        return (void *) 0;
    }
    else if (check < 45){
        return (void *) 0;
    }
    else{
        return (void *) 1;
    }
}

void* colValidate(void* param){
    param_struct * here = (param_struct*)param;
    int check = 0;
    for (int b = 0; b< ROW_SIZE; b++){
        check = check + sudoku_board[b][here->starting_col];
    }
    if (check > 45){
        return (void *) 0;
    }
    else if (check < 45){
        return (void *) 0;
    }
    else if (check == 45){
        return (void *) 1;
    }
}

void* process_sub_grid(void* param){

    param_struct * here = (param_struct*)param;
    int check = 0;
    unsigned int r, c;
    int start_row = here->starting_row;
    int start_col = here->starting_col;

    for (r = start_row; r < start_row + 3; r ++) {
        for (c = start_col; c < start_col + 3; c++) {
            check = check + sudoku_board[r][c];
        }
    }
    /*printf("%dG ",check);*/
    if (check > 45){
        return (void *) 0;
    }
    else if (check < 45){
        return (void *) 0;
    }
    else if (check == 45){
        return (void *) 1;
    }
}

int is_board_valid(){
    pthread_t* tid = (pthread_t*)malloc(sizeof(int*)*NUM_OF_THREADS);  /* the thread identifiers */
    pthread_attr_t attr;
    param_struct* params = (param_struct*)malloc(sizeof(param_struct)*NUM_OF_THREADS);
    worker_validation = (int *)(malloc((sizeof(int *)*NUM_OF_THREADS)));
    
    for (int x = 0; x < 9; x++){

        params[x].id = x; // id of first thread
        params[x].starting_row = x;
        pthread_create(&(tid[x]), NULL, rowValidate, &(params[x]));
        
    }
    for (int h = 0; h < 9; h++ ){
        params[h+9].id = h+9; 
        params[h+9].starting_col = h;
        pthread_create(&(tid[h+9]),NULL, colValidate, &(params[h+9]));
    }
    int l = 17;
    for (int start_row = 0; start_row < 9; start_row += 3) {
        for (int start_col = 0; start_col < 9; start_col += 3) {
            l++;
            params[l].id = l;
            params[l].starting_col = start_col;
            params[l].starting_row = start_row;
            pthread_create(&(tid[l]),NULL, process_sub_grid, &(params[l]));
        }
    }
    int pain = 0;
    for (int z = 0; z < 27; z++){
        pthread_join(tid[z],(void **)&worker_validation[z]);
        if ((int)worker_validation[z] == 1){
            pain++;
        }
    }

    for (int p = 0; p < NUM_OF_THREADS; p++){
        if ((int)worker_validation[p] < 1){
            free(tid);
            free(params);
            free(worker_validation);
            return 0;
        }
    }
    free(tid);
    free(params);
    free(worker_validation);
    return 1;
}
