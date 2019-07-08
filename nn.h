#pragma once
#include "matrix.h"
typedef struct nn {
  int inputSize, hiddenSize, outSize;
  Matrix* input ;
  Matrix* inputLayer ;
  Matrix* hiddenLayer ;
  Matrix* inputBias;
  Matrix* hiddenBias;
  float* output;
  float* vision_input;

} NN;


NN* makeNN(int input, int hidden, int out);
void destroyNN(NN* n);
void set_input(NN* n, float* in, int inSize);
int predict(float* input, NN* nn);
