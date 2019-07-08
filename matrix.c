
#include "matrix.h"

#define LEDG GREEN_LED
#define LEDR RED_LED
#define e 2.71

float sigmoidd(float x) {
  float temp = 1 / (1 + exp(-x));
  return temp;
}

Matrix* makeMatrix(int r, int c) {
  Matrix* temp = (Matrix*)malloc(sizeof(Matrix));
  temp->row = r;
  temp->col = c;
  temp->data = (float**)malloc(sizeof(float*)*temp->row);
  for (int i = 0; i < temp->row; i++) {
    temp->data[i] = (float*)calloc(temp->col, sizeof(float));
  }
  return temp;
}

void destroyMatrix(Matrix* m) {
  for (int i = 0; i < m->row; i++) {
    free(m->data[i]);
  }
  free(m->data);
  free(m);
}

Matrix* setData(Matrix* m, float** data, int r, int c) {
  printf("setting data...\n");
  if (r == m->row && c==m->col)
    for (int i = 0; i < r; i++) {
      for (int j = 0; j < c; j++) {
        m->data[i][j] = data[i][j];
      }
    }
  else{
    printf("erro setting matriz data %d x %d \n",r,c);
  }
    printf("data setted...\n");

  return m;
}
Matrix* mat_mult(Matrix* a , Matrix* b) {
  //digitalWrite(LEDR, HIGH);
  //digitalWrite(LEDG, LOW);
  float temp = 0;
  if (a->col == b->row) {
    int newRow, newCol;
    newRow = a->row;
    newCol = b->col;
    Matrix* result = makeMatrix(newRow, newCol);
    float acum = 0;
    for (int i = 0; i < newRow; i++) {
      for (int j = 0; j < newCol; j++) {
        temp = 0;
        for (int k = 0; k < a->col; k++) {
          temp += a->data[i][k] * b->data[k][j];
        }
        result->data[i][j] = temp;
      }
    }
    //digitalWrite(LEDG, HIGH);
    //digitalWrite(LEDR, LOW);
    return result;
  }
  else {
    //Serial.println("Erro, matrizes n sao multiplicaveis!");
    return 0;
  }

}

void show_data(Matrix* m) {
  for (int i = 0; i < m->row; i++) {
    for (int j = 0; j < m->col; j++) {
      //Serial.print((float)m->data[i][j]);
      printf("%0.2f",(float)m->data[i][j]);
      printf(",");
    }
    //Serial.println();
    printf("\n");
  }
  //Serial.println("##########DONE##############");
  printf("########################done\n");
}
//set all elements
Matrix* setData_all(Matrix* m, float n) {
  for (int i = 0; i < m->row; i++) {
    for (int j = 0; j < m->col; j++) {
      m->data[i][j] = n;
    }
  }
  return m;
}

Matrix* add_matrix(Matrix* a , Matrix* b) {
  if (a->row == b->row && a->col == b->col) {
    Matrix* m = makeMatrix(a->row, a->col);
    for (int i = 0; i < m->row; i++) {
      for (int j = 0; j < m->col; j++) {
        //Monitor->println(a->data[i][j]);
        m->data[i][j] = a->data[i][j] + b->data[i][j];
      }
    }
    return m;
  }
  else {
    //Monitor->print("Erro add Matrix, matrix incompatible!");
    return 0;
  }
}
void activate(Matrix* m) {
  if (m->row == 1) {
    for (int i = 0; i < m->col; i++) {
      float temp = sigmoidd(m->data[0][i]);
      //Monitor->print("sigmoid:");
      m->data[0][i] = temp;
    }
  }
  else {
    //Monitor->println("ERRO Activate, row != 1");
  }
}

