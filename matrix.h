#pragma once

#include <stdio.h>
#include <math.h>
#include <stdlib.h>  



typedef struct {
  int col, row;
  float** data;
} Matrix;
/*######---- MATRIX FUNCTIONS-----#########*/
Matrix* makeMatrix(int r, int c);
void destroyMatrix(Matrix* m);
Matrix* setData(Matrix* m, float** data, int r, int c);
Matrix* setData_all(Matrix* m, float n);
Matrix* mat_mult(Matrix* a , Matrix* b);
void show_data(Matrix* m);
Matrix* add_matrix(Matrix* a , Matrix* b);
void activate(Matrix* m);
float sigmoidd(float x) ;