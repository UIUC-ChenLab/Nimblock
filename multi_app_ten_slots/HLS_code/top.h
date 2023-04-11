#ifndef __TOP_H__
#define __TOP_H__

#include "lenet.h"

int layer_conv1_pool2(volatile DTYPE input[32][32], volatile DTYPE weights[6][5][5],
		volatile DTYPE bias[6], volatile DTYPE output[6][14][14], ap_int<64> *dummy_port_1);

int layer_conv3_pool4(volatile DTYPE input[6][14][14], volatile DTYPE weights[16][6][5][5],
		volatile DTYPE bias[16], volatile DTYPE output[16][5][5]);

int layer_conv5_fc6(volatile DTYPE input[16][5][5], volatile DTYPE conv5_weights[120][16][5][5],
		volatile DTYPE conv5_bias[120], volatile DTYPE fc6_weights[10][120], volatile DTYPE fc6_bias[10],
		volatile DTYPE output[10]);

//int layer_conv3_pool4_divided(volatile DTYPE input[6][14][14], volatile DTYPE weights[6][5][5],
//		volatile DTYPE bias[1], volatile DTYPE output[5][5]);
//
//int layer_conv5_divided(volatile DTYPE input[16][5][5], volatile DTYPE weights[16][5][5], volatile DTYPE bias[1], volatile DTYPE output[1]);
//
//int layer_conv1(volatile DTYPE input[32][32], volatile DTYPE weights[6][5][5],
//		volatile DTYPE bias[6], volatile DTYPE output[6][28][28]);
//
//int layer_pool2(volatile DTYPE input[6][28][28], volatile DTYPE output[6][14][14]);
//
//int layer_conv3(volatile DTYPE input[6][14][14], volatile DTYPE weights[16][6][5][5], volatile DTYPE bias[16],
//		volatile DTYPE output[16][10][10]);
//
//int layer_pool4(volatile DTYPE input[16][10][10], volatile DTYPE output[16][5][5]);
//
//int layer_conv5(volatile DTYPE input[16][5][5], volatile DTYPE weights[120][16][5][5], volatile DTYPE bias[120],
//		volatile DTYPE output[120]);
//
//int layer_fc6(volatile DTYPE input[120], volatile DTYPE weights[10][120], volatile DTYPE bias[10],
//		volatile DTYPE output[10]);



#endif
