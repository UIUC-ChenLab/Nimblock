#include "lenet.h"

void convolution1(volatile DTYPE input[32][32], volatile DTYPE weights[6][5][5], volatile DTYPE bias[6],
 volatile DTYPE output[6][28][28])
{
	int co, h, w, i, j, m, n;
	DTYPE tmp, sum;

	for(co = 0; co < 6; co++)
	for(h = 0; h < 28; h++)
	for(w = 0; w < 28; w++)
	{
//#pragma HLS PIPELINE
		DTYPE sum = 0;
		//for(i = h, m = 0; i < (h + 5); i++, m++)
		for(i = 0; i < 5; i++)
		{
//#pragma HLS UNROLL
			//for(j = w, n = 0; j < (w + 5); j++, n++)
			for(j = 0; j < 5; j++)
			{
//#pragma HLS UNROLL
#pragma HLS PIPELINE
				sum += weights[co][i][j] * input[i+h][j+w];
			}
		}
//                output[co][h][w] = sum + bias[co];
		tmp = sum + bias[co];
		output[co][h][w] = (tmp > 0)? tmp:0.0;

	}
}

void conv1_perf(volatile DTYPE input[32][32], volatile DTYPE weights[6][5][5], volatile DTYPE bias[6],
 volatile DTYPE output[6][14][14])
{
#pragma HLS interface s_axilite port=return
#pragma HLS interface m_axi port=input offset=slave bundle=gmem
#pragma HLS interface m_axi port=weights offset=slave bundle=gmem
#pragma HLS interface m_axi port=bias offset=slave bundle=gmem
#pragma HLS interface m_axi port=output offset=slave bundle=gmem
	int co, h, w, i, j, m, n;
	DTYPE tmp;
	DTYPE product[5][5];
#pragma HLS array_partition variable=product complete
	DTYPE sum[5];
//#pragma HLS array_partition variable=sum complete
	DTYPE bias_buffer[6];
//#pragma HLS array_partition variable=bias_buffer complete
	DTYPE weight_buffer[6][5][5];
#pragma HLS array_partition variable=weight_buffer cyclic factor=5
	DTYPE input_buffer[5][5];
#pragma HLS array_partition variable=input_buffer cyclic factor=5
	// load weight and bias
	for (i = 0; i < 6; i++){
		for (h = 0; h < 5; h++){
			for (w = 0; w < 5; w++){
//#pragma HLS PIPELINE
				weight_buffer[i][h][w] = weights[i][h][w];
			}
		}
	}
	for (i = 0; i < 6; i++){
//#pragma HLS PIPELINE
		bias_buffer[i] = bias[i];
	}
	for(h = 0; h < 28; h++){
		for(w = 0; w < 28; w++){
			// buffer input
			for (i = 0; i < 5; i++){
				for (j = 0; j < 5; j++){
//#pragma HLS PIPELINE
					input_buffer[i][j] = input[i+h][j+w];
				}
			}
			for(co = 0; co < 6; co++)
			{
//#pragma HLS PIPELINE
				// multiply
				for(i = 0; i < 5; i++)
				{
//#pragma HLS UNROLL
					for(j = 0; j < 5; j++)
					{
//#pragma HLS UNROLL
//#pragma HLS PIPELINE
						product[i][j] = weight_buffer[co][i][j] * input_buffer[i][j];
					}
				}
				for (i = 0; i < 5; i++){
					sum[i] = product[i][0] + product[i][1] + product[i][2] + product[i][3] + product[i][4];
				}
				tmp = sum[0] + sum[1] + sum[2] + sum[3] + sum[4] + bias_buffer[co];
				output[co][h][w] = (tmp > 0)? tmp:0.0;
			}
		}
	}
}

void max_pooling2(volatile DTYPE input[6][28][28], volatile DTYPE output[6][14][14])
{
	int c, h, w, i, j;
	DTYPE data;
	bool cond;
    for(c = 0;c < 6; c++)
//#pragma HLS PIPELINE

        for(h = 0; h < 14; h++)
        {
//#pragma HLS PIPELINE
            for(w = 0; w < 14; w++)
            {
//#pragma HLS UNROLL
//#pragma HLS PIPELINE

            	DTYPE max_value=-10000000.0;
                //for(i = h*2; i < h*2+2; i++)
                for(i = 0; i < 2; i++)
                {
//#pragma HLS PIPELINE
                    //for(j = w*2;j < w*2+2; j++)
					for(j = 0;j < 2; j++)
					{
#pragma HLS PIPELINE
						data = input[c][(h*2)+i][(w*2)+j];
						cond = (max_value > input[c][(h*2)+i][(w*2)+j]);

						max_value = cond ? max_value : data;

                        //max_value =  ? max_value:input[c][(h*2)+i][(w*2)+j];
					}
                }
                output[c][h][w] = (max_value > 0)? max_value:0.0;
            }

        }
}

void convolution3(volatile DTYPE input[6][14][14], volatile DTYPE weights[16][6][5][5], volatile DTYPE bias[16], volatile DTYPE output[16][10][10])
{

	int co, h, w, i, j, k, m, n, ci;
	DTYPE tmp, sum;
	DTYPE sum0, sum1, sum2, sum3, sum4;
	//DTYPE sum5, sum6, sum7, sum8, sum9;
	//DTYPE sum;
	DTYPE conv3_partial_sum[150];
	//#pragma HLS ARRAY_PARTITION variable=conv3_partial_sum cyclic factor=5 dim=1

	//DTYPE output_buff[16][10][10];
	DTYPE weights_buff[6][5][5];
	DTYPE bias_local;

	l0:for(co = 0; co < 16; co++)
	{
		for(int a=0; a<6; a++)
		for(int b=0; b<5; b++)
		for(int c=0; c<5; c++)
		#pragma HLS PIPELINE
			weights_buff[a][b][c] = weights[co][a][b][c];

		bias_local = bias[co];

		l1:for(h = 0; h < 10; h++)
		{
			l2:for(w = 0; w < 10; w++)
			{
				//#pragma HLS PIPELINE

				sum = 0.00f;
				l3:for(ci = 0; ci < 6; ci++)
				//#pragma HLS PIPELINE
				//#pragma HLS UNROLL
				l4:for(i = 0; i < 5; i++)
				//#pragma HLS PIPELINE
				//#pragma HLS UNROLL
				l5:for(j = 0; j < 5; j++)
				{
					#pragma HLS PIPELINE
					//#pragma HLS UNROLL
					conv3_partial_sum[(ci*25)+(i*5)+j] = weights_buff[ci][i][j] * input[ci][i+h][j+w];
					//sum = sum + weights[co][ci][i][j] * input[ci][i+h][j+w];
				}

				sum0 = 0.00f;
				sum1 = 0.00f;
				sum2 = 0.00f;
				sum3 = 0.00f;
				sum4 = 0.00f;
	//			sum5 = 0.00f;
	//			sum6 = 0.00f;
	//			sum7 = 0.00f;
	//			sum8 = 0.00f;
	//			sum9 = 0.00f;

				l6:for(k = 0; k < 150; k+=5)
				{
					#pragma HLS pipeline
					//#pragma HLS UNROLL
					sum0 += conv3_partial_sum[k+0];
					sum1 += conv3_partial_sum[k+1];
					sum2 += conv3_partial_sum[k+2];
					sum3 += conv3_partial_sum[k+3];
					sum4 += conv3_partial_sum[k+4];
	//				sum5 += conv3_partial_sum[k+5];
	//				sum6 += conv3_partial_sum[k+6];
	//				sum7 += conv3_partial_sum[k+7];
	//				sum8 += conv3_partial_sum[k+8];
	//				sum9 += conv3_partial_sum[k+9];
				}

				tmp = sum0 + sum1 + sum2 + sum3 + sum4 + bias_local;
						//sum5 + sum6 + sum7 + sum8 + sum9 + bias[co];

				//tmp = sum + bias[co];
				if(tmp>0)
				{
					output[co][h][w] = tmp;
				}
				else
				{
					output[co][h][w] = 0.00f;
				}
				//output[co][h][w] = (tmp > 0)? tmp:0.00f;
			}
		}
	}

}

void conv3_divided(volatile DTYPE input[6][14][14], volatile DTYPE weights[6][5][5], volatile DTYPE bias[1], volatile DTYPE output[10][10])
{

	int co, h, w, i, j, k, m, n, ci;
	DTYPE tmp, sum;
	DTYPE sum0, sum1, sum2, sum3, sum4;
	//DTYPE sum5, sum6, sum7, sum8, sum9;
	//DTYPE sum;
	DTYPE conv3_partial_sum[150];
	//#pragma HLS ARRAY_PARTITION variable=conv3_partial_sum cyclic factor=5 dim=1

	//DTYPE output_buff[16][10][10];
	DTYPE weights_buff[6][5][5];
	DTYPE bias_local;

	// l0:for(co = 0; co < 4; co++)
	// {
		for(int a=0; a<6; a++)
		for(int b=0; b<5; b++)
		for(int c=0; c<5; c++)
		#pragma HLS PIPELINE
			weights_buff[a][b][c] = weights[a][b][c];

		bias_local = bias[0];

		l1:for(h = 0; h < 10; h++)
		{
			l2:for(w = 0; w < 10; w++)
			{
				//#pragma HLS PIPELINE

				sum = 0.00f;
				l3:for(ci = 0; ci < 6; ci++)
				#pragma HLS PIPELINE
				//#pragma HLS UNROLL
				l4:for(i = 0; i < 5; i++)
				#pragma HLS PIPELINE
				//#pragma HLS UNROLL
				l5:for(j = 0; j < 5; j++)
				{
					#pragma HLS PIPELINE
					//#pragma HLS UNROLL
					conv3_partial_sum[(ci*25)+(i*5)+j] = weights_buff[ci][i][j] * input[ci][i+h][j+w];
					//sum = sum + weights[co][ci][i][j] * input[ci][i+h][j+w];
				}

				sum0 = 0.00f;
				sum1 = 0.00f;
				sum2 = 0.00f;
				sum3 = 0.00f;
				sum4 = 0.00f;
	//			sum5 = 0.00f;
	//			sum6 = 0.00f;
	//			sum7 = 0.00f;
	//			sum8 = 0.00f;
	//			sum9 = 0.00f;

				l6:for(k = 0; k < 150; k+=5)
				{
					#pragma HLS UNROLL
					sum0 += conv3_partial_sum[k+0];
					sum1 += conv3_partial_sum[k+1];
					sum2 += conv3_partial_sum[k+2];
					sum3 += conv3_partial_sum[k+3];
					sum4 += conv3_partial_sum[k+4];
	//				sum5 += conv3_partial_sum[k+5];
	//				sum6 += conv3_partial_sum[k+6];
	//				sum7 += conv3_partial_sum[k+7];
	//				sum8 += conv3_partial_sum[k+8];
	//				sum9 += conv3_partial_sum[k+9];
				}

				tmp = sum0 + sum1 + sum2 + sum3 + sum4 + bias_local;
						//sum5 + sum6 + sum7 + sum8 + sum9 + bias[co];

				//tmp = sum + bias[co];
				if(tmp>0)
				{
					output[h][w] = tmp;
				}
				else
				{
					output[h][w] = 0.00f;
				}
				//output[co][h][w] = (tmp > 0)? tmp:0.00f;
			}
		}
	// }

}


void max_pooling4(volatile DTYPE input[16][10][10], volatile DTYPE output[16][5][5])
{
	DTYPE data;
	bool cond;
	int c, h, w, i, j;
    for(c = 0;c < 16; c++)
	for(h = 0; h < 5; h++)
	for(w = 0; w < 5; w++)
	{
#pragma HLS PIPELINE
		DTYPE max_value=-1000000000000.0;
		for(i = 0; i < 2; i++)
		{
#pragma HLS PIPELINE

			for(j = 0;j < 2; j++)
			{
#pragma HLS PIPELINE

				data = input[c][(h*2)+i][(w*2)+j];
				cond = max_value > input[c][(h*2)+i][(w*2)+j];
				max_value = (max_value > data) ? max_value:data;
			}
		}
		output[c][h][w] = (max_value > 0)? max_value:0.0;
	}
}

void pool4_divided(volatile DTYPE input[10][10], volatile DTYPE output[5][5])
{
	DTYPE data;
	bool cond;
	int c, h, w, i, j;
    // for(c = 0;c < 4; c++)
	for(h = 0; h < 5; h++)
	for(w = 0; w < 5; w++)
	{
#pragma HLS PIPELINE
		DTYPE max_value=-1000000000000.0;
		for(i = 0; i < 2; i++)
		{
#pragma HLS PIPELINE

			for(j = 0;j < 2; j++)
			{
#pragma HLS PIPELINE

				data = input[(h*2)+i][(w*2)+j];
				cond = max_value > input[(h*2)+i][(w*2)+j];
				max_value = (max_value > data) ? max_value:data;
			}
		}
		output[h][w] = (max_value > 0)? max_value:0.0;
	}
}


void convolution5(volatile DTYPE input[16][5][5], volatile DTYPE weights[120][16][5][5], volatile DTYPE bias[120], volatile DTYPE output[120])
{
	int co, i, j, k, m, n, ci;
	DTYPE tmp, sum;
	DTYPE sum0, sum1, sum2, sum3, sum4;
	DTYPE sum5, sum6, sum7, sum8, sum9;
	//DTYPE sum;
	//#pragma HLS ARRAY_PARTITION variable=conv3_partial_sum cyclic factor=5 dim=1

	//DTYPE output_buff[16][10][10];
	DTYPE weights_buff[16][5][5];
	DTYPE bias_local;
	DTYPE conv5_partial_sum[400];
	//#pragma HLS ARRAY_PARTITION variable=conv5_partial_sum cyclic factor=10 dim=1



    l0:for(co = 0; co < 120; co++)
    {
		for(int a=0; a<16; a++)
		for(int b=0; b<5; b++)
		for(int c=0; c<5; c++)
		//#pragma HLS PIPELINE
			weights_buff[a][b][c] = weights[co][a][b][c];

		bias_local = bias[0];
        sum = 0;

        l1:for (ci = 0; ci < 16; ci++)
        {
        	//#pragma HLS PIPELINE
			l2:for(i = 0; i < 5; i++)
			{
				//#pragma HLS PIPELINE
				l3:for(j = 0; j < 5; j++)
				{
					#pragma HLS PIPELINE
					conv5_partial_sum[(ci*25)+(i*5)+j]= weights_buff[ci][i][j] * input[ci][i][j];
				}
			}
    	}

        sum0 = 0.00f;
		sum1 = 0.00f;
		sum2 = 0.00f;
		sum3 = 0.00f;
		sum4 = 0.00f;
		sum5 = 0.00f;
		sum6 = 0.00f;
		sum7 = 0.00f;
		sum8 = 0.00f;
		sum9 = 0.00f;

		l4:for(k = 0; k < 400; k+=10)
		{
			//#pragma HLS UNROLL
			#pragma HLS PIPELINE
			sum0 += conv5_partial_sum[k+0];
			sum1 += conv5_partial_sum[k+1];
			sum2 += conv5_partial_sum[k+2];
			sum3 += conv5_partial_sum[k+3];
			sum4 += conv5_partial_sum[k+4];
			sum5 += conv5_partial_sum[k+5];
			sum6 += conv5_partial_sum[k+6];
			sum7 += conv5_partial_sum[k+7];
			sum8 += conv5_partial_sum[k+8];
			sum9 += conv5_partial_sum[k+9];
		}

		tmp = sum0 + sum1 + sum2 + sum3 + sum4  + sum5 + sum6 + sum7 + sum8 + sum9 + bias_local;
		//sum5 + sum6 + sum7 + sum8 + sum9 + bias[co];

		//tmp = sum + bias[co];
		if(tmp>0)
		{
			output[co] = tmp;
		}
		else
		{
			output[co] = 0.00f;
		}
		//output[co][h][w] = (tmp > 0)? tmp:0.00f;
    }
}


void conv5_divided(volatile DTYPE input[16][5][5], volatile DTYPE weights[16][5][5], volatile DTYPE bias[1], volatile DTYPE output[1])
{
	int co, i, j, k, m, n, ci;
	DTYPE tmp, sum;
	DTYPE sum0, sum1, sum2, sum3, sum4;
	DTYPE sum5, sum6, sum7, sum8, sum9;
	//DTYPE sum;
	//#pragma HLS ARRAY_PARTITION variable=conv3_partial_sum cyclic factor=5 dim=1

	//DTYPE output_buff[16][10][10];
	DTYPE weights_buff[16][5][5];
	DTYPE bias_local;
	DTYPE conv5_partial_sum[400];
	//#pragma HLS ARRAY_PARTITION variable=conv5_partial_sum cyclic factor=10 dim=1



    // l0:for(co = 0; co < 30; co++)
    // {
		for(int a=0; a<16; a++)
		for(int b=0; b<5; b++)
		for(int c=0; c<5; c++)
		#pragma HLS PIPELINE
			weights_buff[a][b][c] = weights[a][b][c];

		bias_local = bias[co];
        sum = 0;

        l1:for (ci = 0; ci < 16; ci++)
        {
        	#pragma HLS PIPELINE
			l2:for(i = 0; i < 5; i++)
			{
				#pragma HLS PIPELINE
				l3:for(j = 0; j < 5; j++)
				{
					#pragma HLS PIPELINE
					conv5_partial_sum[(ci*25)+(i*5)+j]= weights_buff[ci][i][j] * input[ci][i][j];
				}
			}
    	}

        sum0 = 0.00f;
		sum1 = 0.00f;
		sum2 = 0.00f;
		sum3 = 0.00f;
		sum4 = 0.00f;
		sum5 = 0.00f;
		sum6 = 0.00f;
		sum7 = 0.00f;
		sum8 = 0.00f;
		sum9 = 0.00f;

		l4:for(k = 0; k < 400; k+=10)
		{
			//#pragma HLS UNROLL
			#pragma HLS PIPELINE
			sum0 += conv5_partial_sum[k+0];
			sum1 += conv5_partial_sum[k+1];
			sum2 += conv5_partial_sum[k+2];
			sum3 += conv5_partial_sum[k+3];
			sum4 += conv5_partial_sum[k+4];
			sum5 += conv5_partial_sum[k+5];
			sum6 += conv5_partial_sum[k+6];
			sum7 += conv5_partial_sum[k+7];
			sum8 += conv5_partial_sum[k+8];
			sum9 += conv5_partial_sum[k+9];
		}

		tmp = sum0 + sum1 + sum2 + sum3 + sum4  + sum5 + sum6 + sum7 + sum8 + sum9 + bias_local;
		if(tmp>0)
		{
			output[0] = tmp;
		}
		else
		{
			output[0] = 0.00f;
		}
}

void fc6(volatile DTYPE input[120], volatile DTYPE weights[10][120], volatile DTYPE bias[10], volatile DTYPE output[10])
{

	int n, c;
	DTYPE tmp;
    for(n = 0; n < 10; n++)
    {
//#pragma HLS PIPELINE
    	tmp = 0.0f;
        for(c = 0; c < 120; c++)
        {
#pragma HLS PIPELINE
        	tmp += weights[n][c] * input[c];
        }
        output[n] = tmp + bias[n];
    }
}
