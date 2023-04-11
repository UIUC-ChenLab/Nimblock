#include "lenet.h"
#include "top.h"

int layer_conv1_pool2(volatile DTYPE input[32][32], volatile DTYPE weights[6][5][5],
		volatile DTYPE bias[6], volatile DTYPE output[6][14][14]
		, ap_int<64> *dummy_port_1, ap_int<64> *dummy_port_2
		, ap_int<64> *dummy_port_3, ap_int<64> *dummy_port_4){
#pragma HLS INTERFACE s_axilite register port=return bundle=ctrl
#pragma HLS INTERFACE m_axi depth=1024 port=input offset=slave bundle=data
#pragma HLS INTERFACE m_axi depth=150 port=weights offset=slave bundle=data
#pragma HLS INTERFACE m_axi depth=6 port=bias offset=slave bundle=data
#pragma HLS INTERFACE m_axi depth=1176 port=output offset=slave bundle=data
#pragma HLS INTERFACE m_axi depth=1 port=dummy_port_1 offset=slave bundle=data
#pragma HLS INTERFACE m_axi depth=1 port=dummy_port_2 offset=slave bundle=data
#pragma HLS INTERFACE m_axi depth=1 port=dummy_port_3 offset=slave bundle=data
#pragma HLS INTERFACE m_axi depth=1 port=dummy_port_4 offset=slave bundle=data
	DTYPE conv1_output[6][28][28];
	convolution1(input, weights, bias, conv1_output);
	max_pooling2(conv1_output, output);
	return 0;
}

int layer_conv3_pool4(volatile DTYPE input[6][14][14], volatile DTYPE weights[16][6][5][5],
		volatile DTYPE bias[16], volatile DTYPE output[16][5][5]
		, ap_int<64> *dummy_port_1, ap_int<64> *dummy_port_2
		, ap_int<64> *dummy_port_3, ap_int<64> *dummy_port_4)
{
#pragma HLS INTERFACE s_axilite register port=return bundle=ctrl
#pragma HLS INTERFACE m_axi depth=1176 port=input offset=slave bundle=data
#pragma HLS INTERFACE m_axi depth=2400 port=weights offset=slave bundle=data
#pragma HLS INTERFACE m_axi depth=16 port=bias offset=slave bundle=data
#pragma HLS INTERFACE m_axi depth=400 port=output offset=slave bundle=data
#pragma HLS INTERFACE m_axi depth=1 port=dummy_port_1 offset=slave bundle=data
#pragma HLS INTERFACE m_axi depth=1 port=dummy_port_2 offset=slave bundle=data
#pragma HLS INTERFACE m_axi depth=1 port=dummy_port_3 offset=slave bundle=data
#pragma HLS INTERFACE m_axi depth=1 port=dummy_port_4 offset=slave bundle=data
	DTYPE conv3_output[16][10][10];
	convolution3(input, weights, bias, conv3_output);
	max_pooling4(conv3_output, output);
	return 0;
}

//int layer_conv3_pool4_divided(volatile DTYPE input[6][14][14], volatile DTYPE weights[6][5][5],
//		volatile DTYPE bias[1], volatile DTYPE output[5][5])
//{
//#pragma HLS INTERFACE s_axilite register port=return
//#pragma HLS INTERFACE m_axi depth=1176 port=input offset=slave
//#pragma HLS INTERFACE m_axi depth=150 port=weights offset=slave
//#pragma HLS INTERFACE m_axi depth=1 port=bias offset=slave
//#pragma HLS INTERFACE m_axi depth=25 port=output offset=slave
//	DTYPE conv3_output[10][10];
//	conv3_divided(input, weights, bias, conv3_output);
//	pool4_divided(conv3_output, output);
//	return 0;
//}
//
//int layer_conv5_divided(volatile DTYPE input[16][5][5], volatile DTYPE weights[16][5][5], volatile DTYPE bias[1], volatile DTYPE output[1])
//{
//#pragma HLS INTERFACE s_axilite register port=return
//#pragma HLS INTERFACE m_axi depth=400 port=input offset=slave
//#pragma HLS INTERFACE m_axi depth=400 port=weights offset=slave
//#pragma HLS INTERFACE m_axi depth=1 port=bias offset=slave
//#pragma HLS INTERFACE m_axi depth=1 port=output offset=slave
//	conv5_divided(input, weights, bias, output);
//	return 0;
//}

int layer_conv5_fc6(volatile DTYPE input[16][5][5], volatile DTYPE conv5_weights[120][16][5][5],
		volatile DTYPE conv5_bias[120], volatile DTYPE fc6_weights[10][120], volatile DTYPE fc6_bias[10],
		volatile DTYPE output[10]
		, ap_int<64> *dummy_port_1, ap_int<64> *dummy_port_2
		, ap_int<64> *dummy_port_3, ap_int<64> *dummy_port_4)
{
#pragma HLS INTERFACE s_axilite register port=return bundle=ctrl
#pragma HLS INTERFACE m_axi depth=400 port=input offset=slave bundle=data
#pragma HLS INTERFACE m_axi depth=48000 port=conv5_weights offset=slave bundle=data
#pragma HLS INTERFACE m_axi depth=120 port=conv5_bias offset=slave bundle=data
#pragma HLS INTERFACE m_axi depth=1200 port=fc6_weights offset=slave bundle=data
#pragma HLS INTERFACE m_axi depth=10 port=fc6_bias offset=slave bundle=data
#pragma HLS INTERFACE m_axi depth=120 port=output offset=slave bundle=data
#pragma HLS INTERFACE m_axi depth=1 port=dummy_port_1 offset=slave bundle=data
#pragma HLS INTERFACE m_axi depth=1 port=dummy_port_2 offset=slave bundle=data
#pragma HLS INTERFACE m_axi depth=1 port=dummy_port_3 offset=slave bundle=data
#pragma HLS INTERFACE m_axi depth=1 port=dummy_port_4 offset=slave bundle=data
	DTYPE conv5_output[120];
	convolution5(input, conv5_weights, conv5_bias, conv5_output);
	fc6(conv5_output, fc6_weights, fc6_bias, output);
	return 0;
}

//int layer(volatile DTYPE input[6][28][28], volatile DTYPE output[6][14][14])
//{
//#pragma HLS INTERFACE s_axilite register port=return
//#pragma HLS INTERFACE m_axi depth=4704 port=input offset=slave
//#pragma HLS INTERFACE m_axi depth=1176 port=output offset=slave
//
//	max_pooling2(input, output);
//
//	return 1;
//}
//
//int layer_conv1(volatile DTYPE input[32][32], volatile DTYPE weights[6][5][5],
//		volatile DTYPE bias[6], volatile DTYPE output[6][28][28])
//{
//#pragma HLS INTERFACE s_axilite register port=return
//#pragma HLS INTERFACE m_axi depth=1024 port=input offset=slave
//#pragma HLS INTERFACE m_axi depth=150 port=weights offset=slave
//#pragma HLS INTERFACE m_axi depth=6 port=bias offset=slave
//#pragma HLS INTERFACE m_axi depth=4704 port=output offset=slave
//
//	convolution1(input, weights, bias, output);
//
//	return 1;
//}
//
//int layer_pool2(volatile DTYPE input[6][28][28], volatile DTYPE output[6][14][14])
//{
//#pragma HLS INTERFACE s_axilite register port=return
//#pragma HLS INTERFACE m_axi depth=4704 port=input offset=slave
//#pragma HLS INTERFACE m_axi depth=1176 port=output offset=slave
//
//	max_pooling2(input, output);
//
//	return 1;
//}
//
//int layer_conv3(volatile DTYPE input[6][14][14], volatile DTYPE weights[16][6][5][5], volatile DTYPE bias[16], volatile DTYPE output[16][10][10])
//{
//#pragma HLS INTERFACE s_axilite register port=return
//#pragma HLS INTERFACE m_axi depth=1176 port=input offset=slave
//#pragma HLS INTERFACE m_axi depth=2400 port=weights offset=slave
//#pragma HLS INTERFACE m_axi depth=16 port=bias offset=slave
//#pragma HLS INTERFACE m_axi depth=1600 port=output offset=slave
//
//	//DTYPE weights_buff[16][6][5][5];
////#pragma HLS ARRAY_PARTITION variable=weights_buff cyclic factor=2 dim=2
////#pragma HLS ARRAY_PARTITION variable=weights_buff cyclic factor=5 dim=3
////#pragma HLS ARRAY_PARTITION variable=weights_buff cyclic factor=5 dim=4
//
//	//DTYPE bias_buff[16];
//	//#pragma HLS ARRAY_PARTITION variable=bias_buff complete dim=1
//	//DTYPE inputb[6][14][14];
//
////	for(int i=0; i<16; i++)
////	{
////		for(int j=0; j<6; j++)
////		for(int k=0; k<5; k++)
////		for(int l=0; l<5; l++)
////		{
//////#pragma HLS PIPELINE
////			weights_buff[i][j][k][l] = weights[i][j][k][l];
////		}
////
////		bias_buff[i] = bias[i];
////	}
//
//	convolution3(input, weights, bias, output);
//
//	return 1;
//}
//
//int layer_pool4(volatile DTYPE input[16][10][10], volatile DTYPE output[16][5][5])
//{
//#pragma HLS INTERFACE s_axilite register port=return
//#pragma HLS INTERFACE m_axi depth=1600 port=input offset=slave
//#pragma HLS INTERFACE m_axi depth=400 port=output offset=slave
//
//	max_pooling4(input, output);
//
//	return 1;
//}
//
//int layer_conv5(volatile DTYPE input[16][5][5], volatile DTYPE weights[120][16][5][5], volatile DTYPE bias[120], volatile DTYPE output[120])
//{
//#pragma HLS INTERFACE s_axilite register port=return
//#pragma HLS INTERFACE m_axi depth=400 port=input offset=slave
//#pragma HLS INTERFACE m_axi depth=48000 port=weights offset=slave
//#pragma HLS INTERFACE m_axi depth=120 port=bias offset=slave
//#pragma HLS INTERFACE m_axi depth=120 port=output offset=slave
//
//	convolution5(input, weights, bias, output);
//
//	return 1;
//}
//
//int layer_fc6(volatile DTYPE input[120], volatile DTYPE weights[10][120], volatile DTYPE bias[10], volatile DTYPE output[10])
//{
//#pragma HLS INTERFACE s_axilite register port=return
//#pragma HLS INTERFACE m_axi depth=120 port=input offset=slave
//#pragma HLS INTERFACE m_axi depth=1200 port=weights offset=slave
//#pragma HLS INTERFACE m_axi depth=10 port=bias offset=slave
//#pragma HLS INTERFACE m_axi depth=10 port=output offset=slave
//
//	fc6(input, weights, bias, output);
//
//	return 1;
//}
//
//int layer_conv1pool2(volatile DTYPE input[32][32], volatile DTYPE weights[6][5][5],
//		volatile DTYPE bias[6], volatile DTYPE output[6][14][14])
//{
//#pragma HLS INTERFACE s_axilite register port=return
//#pragma HLS INTERFACE m_axi depth=1024 port=input offset=slave
//#pragma HLS INTERFACE m_axi depth=150 port=weights offset=slave
//#pragma HLS INTERFACE m_axi depth=6 port=bias offset=slave
//#pragma HLS INTERFACE m_axi depth=1176 port=output offset=slave
//
//	//#pragma HLS DATAFLOW
//	DTYPE conv1_output[6][28][28];
//	#pragma HLS ARRAY_PARTITION variable=conv1_output cyclic factor=4 dim=2
//	#pragma HLS ARRAY_PARTITION variable=conv1_output cyclic factor=4 dim=3
//
//	convolution1(input, weights, bias, conv1_output);
//	max_pooling2(conv1_output, output);
//	return 1;
//}
//
//int layer_conv3pool4(volatile DTYPE input[6][14][14], volatile DTYPE weights[16][6][5][5], volatile DTYPE bias[16], volatile DTYPE output[16][5][5])
//{
//#pragma HLS INTERFACE s_axilite register port=return
//#pragma HLS INTERFACE m_axi depth=1176 port=input offset=slave
//#pragma HLS INTERFACE m_axi depth=2400 port=weights offset=slave
//#pragma HLS INTERFACE m_axi depth=16 port=bias offset=slave
//#pragma HLS INTERFACE m_axi depth=400 port=output offset=slave
//
//	//#pragma HLS DATAFLOW
//	DTYPE conv3_output[16][10][10];
//	#pragma HLS ARRAY_PARTITION variable=conv3_output cyclic factor=4 dim=2
//	#pragma HLS ARRAY_PARTITION variable=conv3_output cyclic factor=4 dim=3
//
//   convolution3(input, weights, bias, conv3_output);
//   max_pooling4(conv3_output, output);
//
//	return 1;
//}
