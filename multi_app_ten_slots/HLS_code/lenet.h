#ifndef __LENET_H__
#define __LENET_H__

#include "ap_int.h"
//#include <ap_fixed.h>
#include <stdint.h>

//typedef int16_t DTYPE;
//typedef int8_t DTYPE;
typedef float DTYPE;
//typedef ap_int<16> DTYPE;
//typedef float DTYPE;

void convolution1(volatile DTYPE input[32][32], volatile DTYPE weights[6][5][5], volatile DTYPE bias[6], volatile DTYPE output[6][28][28]);

void max_pooling2(volatile DTYPE input[6][28][28], volatile DTYPE output[6][14][14]);

void convolution3(volatile DTYPE input[6][14][14], volatile DTYPE weights[16][6][5][5], volatile DTYPE bias[16], volatile DTYPE output[16][10][10]);

void conv3_divided(volatile DTYPE input[6][14][14], volatile DTYPE weights[6][5][5], volatile DTYPE bias[1], volatile DTYPE output[10][10]);

void max_pooling4(volatile DTYPE input[16][10][10], volatile DTYPE output[16][5][5]);

void pool4_divided(volatile DTYPE input[10][10], volatile DTYPE output[5][5]);

void convolution5(volatile DTYPE input[16][5][5], volatile DTYPE weights[120][16][5][5], volatile DTYPE bias[120], volatile DTYPE output[120]);

void conv5_divided(volatile DTYPE input[16][5][5], volatile DTYPE weights[16][5][5], volatile DTYPE bias[1], volatile DTYPE output[1]);

void fc6(volatile DTYPE input[120], volatile DTYPE weights[10][120], volatile DTYPE bias[10], volatile DTYPE output[10]);

int layer_conv1pool2(volatile DTYPE input[32][32], volatile DTYPE weights[6][5][5], volatile DTYPE bias[6], volatile DTYPE output[6][14][14]);
int layer_conv3pool4(volatile DTYPE input[6][14][14], volatile DTYPE weights[16][6][5][5], volatile DTYPE bias[16], volatile DTYPE output[16][5][5]);

#endif
