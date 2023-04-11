#include <stdio.h>
#include <math.h>
#include "layers_alexnet.h"
#include "ap_int.h"

int convolution1_hls(float bottom[3][227][227], float g_weights[96][3][11][11],
		float g_bias[96], float top[96][31][31], int output_offset, ap_int<64> *dummy_port, ap_int<64> *dummy_port1, ap_int<64> *dummy_port2)
{

#pragma HLS INTERFACE m_axi depth=2187 port=bottom offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi depth=34848 port=g_weights offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi depth=96 port=g_bias offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi depth=69984 port=top offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi port=dummy_port offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi port=dummy_port1 offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi port=dummy_port2 offset=slave bundle=DATA
#pragma HLS INTERFACE s_axilite register port=output_offset bundle=CTRL
#pragma HLS INTERFACE s_axilite register port=return bundle=CTRL

	//float bias[96];
	//float bias;

	float out[55][55];
//	#pragma HLS ARRAY_PARTITION variable=out cyclic factor=3 dim=1
	//#pragma HLS ARRAY_PARTITION variable=out cyclic factor=1 dim=2

	float temp[11];
	#pragma HLS ARRAY_PARTITION variable=temp complete dim=1

	float partial_sum[11];
	#pragma HLS ARRAY_PARTITION variable=partial_sum complete dim=1

	float weights[3][11][11];
	#pragma HLS ARRAY_PARTITION variable=weights complete dim=3

	float sum;
	float sum0, sum1, sum2, sum3, sum4, sum5, sum6, sum7, sum8, sum9, sum10, sum11;

	output_loop:for(int co=0;co<(conv1_output/conv1_slots);co++)
	{
		//bias = g_bias[co];

		w_i:for(int i=0; i<3; i++)
			w_j:for(int j=0; j<11; j++)
				w_k:for(int k=0; k<11; k++)
				{
					#pragma HLS PIPELINE
					weights[i][j][k] = g_weights[co+output_offset][i][j][k];
				}
		
		height_loop:for(int h=0;h<55;h++)
		{
			width_loop:for(int w=0;w<55;w++)
			{
				//#pragma HLS PIPELINE
				int ch = h*4;
				int cw = w*4;
				sum = 0.00f;

				input_channel_loop:for (int ci = 0; ci < 3; ci++)
				{
					kernel_channel_loop:for(int i=0; i<11; i++)
					{
						#pragma HLS PIPELINE

						for(int j=0; j<11; j++)
						{
							#pragma HLS PIPELINE
							temp[j]=bottom[ci][ch+i][cw+j];
						}

						partial_sum[0] = (float)weights[ci][i][0] * (float)temp[0];
						partial_sum[1] = (float)weights[ci][i][1] * (float)temp[1];
						partial_sum[2] = (float)weights[ci][i][2] * (float)temp[2];
						partial_sum[3] = (float)weights[ci][i][3] * (float)temp[3];
						partial_sum[4] = (float)weights[ci][i][4] * (float)temp[4];
						partial_sum[5] = (float)weights[ci][i][5] * (float)temp[5];
						partial_sum[6] = (float)weights[ci][i][6] * (float)temp[6];
						partial_sum[7] = (float)weights[ci][i][7] * (float)temp[7];
						partial_sum[8] = (float)weights[ci][i][8] * (float)temp[8];
						partial_sum[9] = (float)weights[ci][i][9] * (float)temp[9];
						partial_sum[10] = (float)weights[ci][i][10] * (float)temp[10];

						sum0 = partial_sum[0] + partial_sum[5];
						sum1 = partial_sum[1] + partial_sum[6];
						sum2 = partial_sum[2] + partial_sum[7];
						sum3 = partial_sum[3] + partial_sum[8];
						sum4 = partial_sum[4] + partial_sum[9];

						sum5 = sum0 + sum1;
						sum6 = sum2 + sum3;
						sum7 = sum4 + partial_sum[10];
						//sum8 = sum7 + partial_sum[10];
						sum += sum5 + sum6 + sum7;
					}
				}
				
				//top[co+output_offset][h][w] = std::max(sum + g_bias[co+output_offset], 0.0f);
				out[h][w] = std::max(sum + g_bias[co+output_offset], 0.0f);
			}
		}
		
		for(int h=0;h < 27;h++)
		{
			for(int w =0; w< 27; w++)
			{
				#pragma HLS PIPELINE
				int h_start=h*2;
				int w_start=w*2;
				// type is "MAX"
				float max_value=-1000000000000.0;
				for(int i=0;i<3;i++)
				for(int j=0;j<3;j++)
				{
					//#pragma HLS PIPELINE
					max_value=std::max(max_value,out[h_start+i][w_start+j]);
				}
				top[co+output_offset][h+2][w+2]=max_value;
			}
		}

	}
	return 0;
}


int convolution2_hls(float bottom[96][31][31], float g_weights[256][48][5][5], float g_bias[256], float top[256][15][15], int output_offset,
					  int input_offset, ap_int<64> *dummy_port, ap_int<64> *dummy_port1, ap_int<64> *dummy_port2)
{
#pragma HLS INTERFACE m_axi depth=92256 port=bottom offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi depth=307200 port=g_weights offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi depth=256 port=g_bias offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi depth=57600 port=top offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi port=dummy_port offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi port=dummy_port1 offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi port=dummy_port2 offset=slave bundle=DATA
#pragma HLS INTERFACE s_axilite register port=input_offset bundle=CTRL
#pragma HLS INTERFACE s_axilite register port=output_offset bundle=CTRL
#pragma HLS INTERFACE s_axilite register port=return bundle=CTRL

	float out[1][27][27];
	float weights[1][48][5][5];

	float sum[2];
	#pragma HLS ARRAY_PARTITION variable=sum complete dim=1

	float temp0[5];//, temp1[5];
	#pragma HLS ARRAY_PARTITION variable=temp0 complete dim=1
	//#pragma HLS ARRAY_PARTITION variable=temp1 complete dim=1

	float psum0[10];// psum1[10];
	//#pragma HLS ARRAY_PARTITION variable=psum0 complete dim=1
	//#pragma HLS ARRAY_PARTITION variable=psum1 complete dim=1
	
	for(int co=0;co<(conv2_output/conv2_slots);co++)
	{	
		for (int ci = 0; ci < 48; ci++)
		for(int i=0; i<5; i++)	
		for(int j=0; j<5; j++)
		{
			#pragma HLS PIPELINE
			weights[0][ci][i][j] = g_weights[co+output_offset][ci][i][j];
			//weights[1][ci][i][j] = g_weights[co+128+output_offset][ci][i][j];
		}
		
		for(int h=0;h<27;h++)
		{
			for(int w=0;w<27;w++)
			{
				sum[0] = 0.00f;
				sum[1] = 0.00f;
				
				for (int ci = 0; ci < 48; ci++)
				{
					for(int i=0; i<5; i++)
					{
						#pragma HLS PIPELINE
						int ch=h+i;
						for(int j=0; j<5; j++)
						{
							#pragma HLS PIPELINE
							temp0[j]=bottom[ci+input_offset][ch][w+j];
						}

						psum0[0] = (float)weights[0][ci][i][0] * (float)temp0[0];
						psum0[1] = (float)weights[0][ci][i][1] * (float)temp0[1];
						psum0[2] = (float)weights[0][ci][i][2] * (float)temp0[2];
						psum0[3] = (float)weights[0][ci][i][3] * (float)temp0[3];
						psum0[4] = (float)weights[0][ci][i][4] * (float)temp0[4];
						psum0[5] = psum0[0] + psum0[1] + psum0[2] + psum0[3] + psum0[4] + sum[0];
						sum[0] = psum0[5];

					}
				}
				out[0][h][w] = std::max(sum[0] + g_bias[co+output_offset], 0.0f);
			}
		}

		pool_h:for(int h=0;h < 13;h++)
		{
			pool_w:for(int w =0; w< 13; w++)
			{

                int h_start=h*2;
                int w_start=w*2;
                // type is "MAX"
                float max_value0=-1000000000000.0;
				float max_value1=-1000000000000.0;
				pool_i:for(int i=0;i<3;i++)
				pool_j:for(int j=0;j<3;j++)
                {
#pragma HLS PIPELINE
                    max_value0=std::max(max_value0,out[0][h_start+i][w_start+j]);
                }
                top[co+output_offset][h+1][w+1]=max_value0;
            }
        }	
    }
	return 0;
 }
 
 int convolution3_hls(float bottom[256][15][15], float g_weights[384][256][3][3], float g_bias[384], float top[384][15][15], int output_offset,
		              ap_int<64> *dummy_port, ap_int<64> *dummy_port1, ap_int<64> *dummy_port2)
{
#pragma HLS INTERFACE m_axi depth=57600 port=bottom offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi depth=884736 port=g_weights offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi depth=384 port=g_bias offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi depth=64896 port=top offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi port=dummy_port offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi port=dummy_port1 offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi port=dummy_port2 offset=slave bundle=DATA
#pragma HLS INTERFACE s_axilite register port=output_offset bundle=CTRL
#pragma HLS INTERFACE s_axilite register port=return bundle=CTRL

	float bias;
    //float bottom_padded[15][15];
	float weights[256][3][3];
	#pragma HLS ARRAY_PARTITION variable=weights complete dim=2
	#pragma HLS ARRAY_PARTITION variable=weights complete dim=3
	
	float sum = 0.00f;
	float temp[3][3];
	#pragma HLS ARRAY_PARTITION variable=temp complete dim=1
	#pragma HLS ARRAY_PARTITION variable=temp complete dim=2
	
	float pproduct[9];
	#pragma HLS ARRAY_PARTITION variable=pproduct complete dim=1
	
	float psum[9];
	#pragma HLS ARRAY_PARTITION variable=psum complete dim=1

	for(int co=0;co<(conv3_output/conv3_slots);co++)
	{
		//#pragma HLS UNROLL FACTOR=4
		bias = g_bias[co];
		for (int ci = 0; ci < 256; ci++)
		for(int i=0; i<3; i++)
		for(int j=0; j<3; j++)
		{
			#pragma HLS PIPELINE
			weights[ci][i][j] = g_weights[co+output_offset][ci][i][j];
		}

		for(int h=0;h<13;h++)
		{

			for(int w=0;w<13;w++)
			{
				sum = 0.00f;
		
				for (int ci = 0; ci < 256; ci++)
				{
					#pragma HLS PIPELINE
					for(int i=0; i<3; i++)
					{
						#pragma HLS PIPELINE
						temp[0][i] = bottom[ci][h+0][w+i];
					}
					for(int i=0; i<3; i++)
					{
						#pragma HLS PIPELINE
						temp[1][i] = bottom[ci][h+1][w+i];
					}
					for(int i=0; i<3; i++)
					{
						#pragma HLS PIPELINE
						temp[2][i] = bottom[ci][h+2][w+i];
					}
					
					
					pproduct[0] = temp[0][0]*weights[ci][0][0];
					pproduct[1] = temp[0][1]*weights[ci][0][1];
					pproduct[2] = temp[0][2]*weights[ci][0][2];
					pproduct[3] = temp[1][0]*weights[ci][1][0];
					pproduct[4] = temp[1][1]*weights[ci][1][1];
					pproduct[5] = temp[1][2]*weights[ci][1][2];
					pproduct[6] = temp[2][0]*weights[ci][2][0];
					pproduct[7] = temp[2][1]*weights[ci][2][1];
					pproduct[8] = temp[2][2]*weights[ci][2][2];
					
					psum[0] = pproduct[0] + pproduct[1];
					psum[1] = pproduct[2] + pproduct[3];
					psum[2] = pproduct[4] + pproduct[5];
					psum[3] = pproduct[6] + pproduct[7];
					psum[4] = pproduct[8];
					
					psum[5] = psum[0] +  psum[1];
					psum[6] = psum[2] +  psum[3];
					psum[7] = psum[5] +  psum[6];
					psum[8] = psum[7] +  psum[4];
					sum += psum[8];
//					psum[7] = psum[4];
//					sum += psum[1];
//					sum += psum[2];
//					sum += psum[3];
//					sum += psum[4];
					
				}
				top[co+output_offset][h+1][w+1] = std::max(sum + bias, 0.0f);
			}
		}
	}
	return 0;
}
 
int convolution4_hls(float bottom[384][15][15], float g_weights[384][192][3][3], float g_bias[384], float top[384][15][15], int output_offset, int input_offset,
					 ap_int<64> *dummy_port, ap_int<64> *dummy_port1, ap_int<64> *dummy_port2)
{

#pragma HLS INTERFACE m_axi depth=86400 port=bottom offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi depth=663552 port=g_weights offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi depth=384 port=g_bias offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi depth=32448 port=top offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi port=dummy_port offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi port=dummy_port1 offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi port=dummy_port2 offset=slave bundle=DATA
#pragma HLS INTERFACE s_axilite register port=output_offset bundle=CTRL
#pragma HLS INTERFACE s_axilite register port=input_offset bundle=CTRL
#pragma HLS INTERFACE s_axilite register port=return bundle=CTRL

	float bias;
    //float bottom_padded[15][15];
	float weights[192][3][3];
	//#pragma HLS ARRAY_PARTITION variable=weights cyclic factor=2 dim=1
	#pragma HLS ARRAY_PARTITION variable=weights complete dim=2
	#pragma HLS ARRAY_PARTITION variable=weights complete dim=3
	
	float sum = 0.00f;
	float temp[3][3];
	#pragma HLS ARRAY_PARTITION variable=temp complete dim=1
	#pragma HLS ARRAY_PARTITION variable=temp complete dim=2
	
	float pproduct[9];
	#pragma HLS ARRAY_PARTITION variable=pproduct complete dim=1
	
	float psum[9];
	#pragma HLS ARRAY_PARTITION variable=psum complete dim=1	

	for(int co=0;co<(conv4_output/conv4_slots);co++)
	{
		bias = g_bias[co+output_offset+input_offset];
		
		for (int ci = 0; ci < 192; ci++)
		for(int i=0; i<3; i++)
		for(int j=0; j<3; j++)
		{
			#pragma HLS PIPELINE
			weights[ci][i][j] = g_weights[co+output_offset+input_offset][ci][i][j];
		}
		
		for(int h=0;h<13;h++)
		{
			for(int w=0;w<13;w++)
			{
				//#pragma HLS PIPELINE
				sum = 0.00f;
				
				for (int ci = 0; ci < 192; ci++)
				{
					#pragma HLS PIPELINE
					
					for(int i=0; i<3; i++)
					{
						#pragma HLS PIPELINE
						temp[0][i] = bottom[ci+input_offset][h+0][w+i];
					}
					for(int i=0; i<3; i++)
					{
						#pragma HLS PIPELINE
						temp[1][i] = bottom[ci+input_offset][h+1][w+i];
					}
					for(int i=0; i<3; i++)
					{
						#pragma HLS PIPELINE
						temp[2][i] = bottom[ci+input_offset][h+2][w+i];
					}
					
					pproduct[0] = temp[0][0]*weights[ci][0][0];
					pproduct[1] = temp[0][1]*weights[ci][0][1];
					pproduct[2] = temp[0][2]*weights[ci][0][2];
					pproduct[3] = temp[1][0]*weights[ci][1][0];
					pproduct[4] = temp[1][1]*weights[ci][1][1];
					pproduct[5] = temp[1][2]*weights[ci][1][2];
					pproduct[6] = temp[2][0]*weights[ci][2][0];
					pproduct[7] = temp[2][1]*weights[ci][2][1];
					pproduct[8] = temp[2][2]*weights[ci][2][2];
					
					psum[0] = pproduct[0] + pproduct[1];
					psum[1] = pproduct[2] + pproduct[3];
					psum[2] = pproduct[4] + pproduct[5];
					psum[3] = pproduct[6] + pproduct[7];
					psum[4] = pproduct[8];
					
					psum[5] = psum[0] +  psum[1];
					psum[6] = psum[2] +  psum[3];
					psum[7] = psum[5] +  psum[6];
					psum[8] = psum[7] +  psum[4];
					sum += psum[8];
					
				}
				
				top[co+output_offset+input_offset][h+1][w+1] = std::max(sum + bias, 0.0f);
			}
		}
	}
	return 0;
}

//int convolution5_hls(float bottom[192][15][15], float g_weights[128][192][3][3], float g_bias[128], float top[128][13][13], int output_offset, int input_offset)
int convolution5_hls(float bottom[384][15][15], float g_weights[256][192][3][3], float g_bias[256], float top[256][6][6], int output_offset, int input_offset,
					 ap_int<64> *dummy_port, ap_int<64> *dummy_port1, ap_int<64> *dummy_port2)
{

#pragma HLS INTERFACE m_axi depth=86400 port=bottom offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi depth=442368 port=g_weights offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi depth=256 port=g_bias offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi depth=9216 port=top offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi port=dummy_port offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi port=dummy_port1 offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi port=dummy_port2 offset=slave bundle=DATA
#pragma HLS INTERFACE s_axilite register port=output_offset bundle=CTRL
#pragma HLS INTERFACE s_axilite register port=input_offset bundle=CTRL
#pragma HLS INTERFACE s_axilite register port=return bundle=CTRL

	float out[13][13];
	
	float weights[192][3][3];
	#pragma HLS ARRAY_PARTITION variable=weights complete dim=2
	#pragma HLS ARRAY_PARTITION variable=weights complete dim=3
	
	float sum=0.00f;
	float bias; 
	
	float temp[3][3];
	#pragma HLS ARRAY_PARTITION variable=temp complete dim=1
	#pragma HLS ARRAY_PARTITION variable=temp complete dim=2
	
	float pproduct[9];
	#pragma HLS ARRAY_PARTITION variable=pproduct complete dim=1
	
	float psum[9];
	#pragma HLS ARRAY_PARTITION variable=psum complete dim=1	
	
	for(int co=0; co<(conv5_output/conv5_slots); co++)
	{
		bias = g_bias[co+output_offset];
		
		for (int ci = 0; ci < 192; ci++)
		for(int i=0; i<3; i++)
		for(int j=0; j<3; j++)
		{
			#pragma HLS PIPELINE
			weights[ci][i][j] = g_weights[co+output_offset][ci][i][j];
		}
			
		
		for(int h=0;h<13;h++)
		{
			for(int w=0;w<13;w++)
			{
				sum = 0.00f;
				
				for (int ci = 0; ci < 192; ci++)
				{
					#pragma HLS PIPELINE
					for(int i=0; i<3; i++)
					{
						#pragma HLS PIPELINE
						temp[0][i]=bottom[ci+input_offset][h+0][w+i];
					}
					for(int i=0; i<3; i++)
					{
						#pragma HLS PIPELINE
						temp[1][i]=bottom[ci+input_offset][h+1][w+i];
					}
					for(int i=0; i<3; i++)
					{
						#pragma HLS PIPELINE
						temp[2][i]=bottom[ci+input_offset][h+2][w+i];
					}
					
					pproduct[0] = temp[0][0]*weights[ci][0][0];
					pproduct[1] = temp[0][1]*weights[ci][0][1];
					pproduct[2] = temp[0][2]*weights[ci][0][2];
					pproduct[3] = temp[1][0]*weights[ci][1][0];
					pproduct[4] = temp[1][1]*weights[ci][1][1];
					pproduct[5] = temp[1][2]*weights[ci][1][2];
					pproduct[6] = temp[2][0]*weights[ci][2][0];
					pproduct[7] = temp[2][1]*weights[ci][2][1];
					pproduct[8] = temp[2][2]*weights[ci][2][2];
					
					psum[0] = pproduct[0] + pproduct[1];
					psum[1] = pproduct[2] + pproduct[3];
					psum[2] = pproduct[4] + pproduct[5];
					psum[3] = pproduct[6] + pproduct[7];
					psum[4] = pproduct[8];
					
					psum[5] = psum[0] +  psum[1];
					psum[6] = psum[2] +  psum[3];
					psum[7] = psum[5] +  psum[6];
					psum[8] = psum[7] +  psum[4];
					sum += psum[8];
					
				}
				
				out[h][w] = std::max(sum + bias, 0.00f);
				
			}
		}
		
		for(int h=0;h < 6;h++)
		{
            for(int w =0; w< 6; w++)
			{
				#pragma HLS PIPELINE
                int h_start=h*2;
                int w_start=w*2;
                // type is "MAX"
                float max_value=-1000000000000.0;
                for(int i=0;i<3;i++)
				for(int j=0;j<3;j++)
                {
                    max_value=std::max(max_value,out[h_start+i][w_start+j]);
                }
                //top[c][h][w]=max_value;
				top[co+output_offset][h][w] =max_value;
            }
        }
	}
	return 0;
}

int fullconnection6_hls(const float bottom[256][6][6], const float g_weights[256][256][6][6], const float g_bias[256], float top[256], int output_offset,
					    ap_int<64> *dummy_port, ap_int<64> *dummy_port1, ap_int<64> *dummy_port2)
{

#pragma HLS INTERFACE m_axi depth=9216 port=bottom offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi depth=2359296 port=g_weights offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi depth=256 port=g_bias offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi depth=256 port=top offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi port=dummy_port offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi port=dummy_port1 offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi port=dummy_port2 offset=slave bundle=DATA
#pragma HLS INTERFACE s_axilite register port=output_offset bundle=CTRL
#pragma HLS INTERFACE s_axilite register port=return bundle=CTRL

	float bias;
	float sum=0.00f;
	float temp[6];
	float temp2[6];
	float weights[256][6][6];

	float pproduct[12];
	#pragma HLS ARRAY_PARTITION variable=pproduct complete dim=1

	float psum[12];
	#pragma HLS ARRAY_PARTITION variable=psum complete dim=1


	for(int n=0;n<(conv6_output/conv6_slots);n++)
	{
//#pragma HLS unroll factor=2
		sum = 0.00f;
		bias = g_bias[n+output_offset];

		for(int c=0;c<256;c++)
		for(int i=0;i<6;i++)
		for(int j=0;j<6;j++)
		{
			#pragma HLS PIPELINE
			weights[c][i][j] = g_weights[n+output_offset][c][i][j];
		}

		for(int c=0;c<128;c++)
		{
			for(int h=0;h<6;h++)
			{
				#pragma HLS PIPELINE
				for(int w=0;w<6;w++)
				{
					#pragma HLS PIPELINE
					temp[w] = bottom[2*c][h][w];
				}
				for(int w=0;w<6;w++)
				{
					#pragma HLS PIPELINE
					temp2[w] = bottom[2*c+1][h][w];
				}

				pproduct[0] = temp[0] * weights[2*c][h][0];
				pproduct[1] = temp[1] * weights[2*c][h][1];
				pproduct[2] = temp[2] * weights[2*c][h][2];
				pproduct[3] = temp[3] * weights[2*c][h][3];
				pproduct[4] = temp[4] * weights[2*c][h][4];
				pproduct[5] = temp[5] * weights[2*c][h][5];

				pproduct[6] = temp2[0] * weights[2*c+1][h][0];
				pproduct[7] = temp2[1] * weights[2*c+1][h][1];
				pproduct[8] = temp2[2] * weights[2*c+1][h][2];
				pproduct[9] = temp2[3] * weights[2*c+1][h][3];
				pproduct[10] = temp2[4] * weights[2*c+1][h][4];
				pproduct[11] = temp2[5] * weights[2*c+1][h][5];

				psum[0] = pproduct[0] + pproduct[6];
				psum[1] = pproduct[1] + pproduct[7];
				psum[2] = pproduct[2] + pproduct[8];
				psum[3] = pproduct[3] + pproduct[9];
				psum[4] = pproduct[4] + pproduct[10];
				psum[5] = pproduct[5] + pproduct[11];

				psum[6] = psum[0] + psum[3];
				psum[7] = psum[1] + psum[4];
				psum[8] = psum[2] + psum[5];

				psum[9] = psum[6] + psum[7];
				psum[10] = psum[8] + psum[9];

				sum += psum[10];
			}
		}
		top[n+output_offset] = std::max(sum + bias, 0.00f);
	}
	return 0;
}

int fullconnection7_hls(const float g_bottom[256], const float g_weights[256][256], const float g_bias[256], float top[256], int output_offset,
	    ap_int<64> *dummy_port, ap_int<64> *dummy_port1, ap_int<64> *dummy_port2)
{
#pragma HLS INTERFACE m_axi depth=256 port=g_bottom offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi depth=65536 port=g_weights offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi depth=256 port=g_bias offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi depth=256 port=top offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi port=dummy_port offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi port=dummy_port1 offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi port=dummy_port2 offset=slave bundle=DATA
#pragma HLS INTERFACE s_axilite register port=output_offset bundle=CTRL
#pragma HLS INTERFACE s_axilite register port=return bundle=CTRL

	float sum;
	float bias;
	float temp;
	float pproduct;
	float bottom[256];
	float weights[256];
	
	for(int n=0;n<256;n++)
	{
		#pragma HLS PIPELINE
		bottom[n] = g_bottom[n];
	}
	
	for(int n=0;n<(conv7_output/conv7_slots);n++)
	{
		//#pragma HLS PIPELINE
		sum = 0.00f;
		bias = g_bias[n+output_offset];
		
		for(int c=0;c<256;c++)
		{
			#pragma HLS PIPELINE
			weights[c] = g_weights[n+output_offset][c];
		}
		
		for(int c=0;c<256 ;c++)
		{
			#pragma HLS PIPELINE
			temp=bottom[c];		
			sum += weights[c]*temp;
			
		}
		
		top[n+output_offset] = std::max(sum + bias, 0.00f);
	}
	return 0;
}

int fullconnection8_hls(const float g_bottom[256], const float g_weights[1000][256], const float g_bias[1000], float top[1000], int output_offset,
	    ap_int<64> *dummy_port, ap_int<64> *dummy_port1, ap_int<64> *dummy_port2)
{
#pragma HLS INTERFACE m_axi depth=256 port=g_bottom offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi depth=256000 port=g_weights offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi depth=1000 port=g_bias offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi depth=1000 port=top offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi port=dummy_port offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi port=dummy_port1 offset=slave bundle=DATA
#pragma HLS INTERFACE m_axi port=dummy_port2 offset=slave bundle=DATA
#pragma HLS INTERFACE s_axilite register port=output_offset bundle=CTRL
#pragma HLS INTERFACE s_axilite register port=return bundle=CTRL

	float sum;
	float bias;
	float temp;
	float pproduct;
	float bottom[256];
	float weights[256];
	
	for(int n=0;n<256;n++)
	{
		#pragma HLS PIPELINE
		bottom[n] = g_bottom[n];
	}

	for(int n=0;n<(conv8_output/conv8_slots) ;n++)
	{
		sum = 0;
		bias = g_bias[n+output_offset];
		
		for(int c=0;c<256;c++)
		{
			#pragma HLS PIPELINE
			weights[c] = g_weights[n+output_offset][c];
		}
		
		for(int c=0;c<256 ;c++)
		{
			#pragma HLS PIPELINE
			temp=bottom[c];
			sum+=weights[c]*temp;
		}
		
		top[n+output_offset] = sum + bias;
	}
	return 0;
}


 
 
