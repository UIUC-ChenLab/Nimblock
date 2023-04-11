/*===============================================================*/
/*                                                               */
/*                          digitrec.cpp                         */
/*                                                               */
/*             Hardware function for digit recognition           */
/*                                                               */
/*===============================================================*/

#include "knn_typedefs.h"
#include <stdio.h>

WholeDigitType pack_whole_digit(const DigitType *input){
	#pragma HLS inline
	WholeDigitType output;
	output.range(63 , 0  ) = input[0];
	output.range(127, 64 ) = input[1];
	output.range(191, 128) = input[2];
	output.range(255, 192) = input[3];
	return output;
}

void unpack_whole_digit(DigitType *output, WholeDigitType input ){
	output[0] = input.range(63 , 0  );
	output[1] = input.range(127, 64 );
	output[2] = input.range(191, 128);
	output[3] = input.range(255, 192);
}

// popcount function
int popcount(WholeDigitType x)
{
  // most straightforward implementation
  // actually not bad on FPGA
  int cnt = 0;
  for (int i = 0; i < 256; i ++ )
    cnt = cnt + x[i];

  return cnt;
}

// Given the test instance and a (new) training instance, this
// function maintains/updates an array of K minimum
// distances per training set.
//void update_knn( WholeDigitType test_inst, WholeDigitType train_inst, int min_distances[K_CONST] )
void update_knn(const DigitType *test_inst, const DigitType *train_inst, int min_distances[K_CONST] )
{
  #pragma HLS inline

  // Compute the difference using XOR
  WholeDigitType test_inst_whole = pack_whole_digit(test_inst);
  WholeDigitType train_inst_whole = pack_whole_digit(train_inst);
  WholeDigitType diff = test_inst_whole ^ train_inst_whole;

  int dist = 0;

  dist = popcount(diff);

  int max_dist = 0;
  int max_dist_id = K_CONST+1;
  int k = 0;

  // Find the max distance
  FIND_MAX_DIST: for ( int k = 0; k < K_CONST; ++k ) 
  {
    if ( min_distances[k] > max_dist ) 
    {
      max_dist = min_distances[k];
      max_dist_id = k;
    }
  }

  // Replace the entry with the max distance
  if ( dist < max_dist )
    min_distances[max_dist_id] = dist;

  return;
}

// Given 10xK minimum distance values, this function
// finds the actual K nearest neighbors and determines the
// final output based on the most common int represented by
// these nearest neighbors (i.e., a vote among KNNs).
LabelType knn_vote( int knn_set[PAR_FACTOR * K_CONST] ) 
{
  #pragma HLS inline

  // local buffers

  // final K nearest neighbors
  int min_distance_list[K_CONST];
  #pragma HLS array_partition variable=min_distance_list complete dim=0
  // labels for the K nearest neighbors
  int label_list[K_CONST];
  #pragma HLS array_partition variable=label_list complete dim=0
  // voting boxes
  int vote_list[10];
  #pragma HLS array_partition variable=vote_list complete dim=0

  int pos = 1000;

  // initialize
  INIT_1: for (int i = 0;i < K_CONST; i ++ )
  {
    #pragma HLS unroll
    min_distance_list[i] = 256;
    label_list[i] = 9;
  }

  INIT_2: for (int i = 0;i < 10; i ++ )
  {
    #pragma HLS unroll
    vote_list[i] = 0;
  }

  // go through all the lanes
  // do an insertion sort to keep a sorted neighbor list
  LANES: for (int i = 0; i < PAR_FACTOR; i ++ )
  {
    INSERTION_SORT_OUTER: for (int j = 0; j < K_CONST; j ++ )
    {
      #pragma HLS pipeline
      pos = 1000;
      INSERTION_SORT_INNER: for (int r = 0; r < K_CONST; r ++ )
      {
        #pragma HLS unroll
        pos = ((knn_set[i*K_CONST+j] < min_distance_list[r]) && (pos > K_CONST)) ? r : pos;
      }

      INSERT: for (int r = K_CONST ;r > 0; r -- )
      {
        #pragma HLS unroll
        if(r-1 > pos)
        {
          min_distance_list[r-1] = min_distance_list[r-2];
          label_list[r-1] = label_list[r-2];
        }
        else if (r-1 == pos)
        {
          min_distance_list[r-1] = knn_set[i*K_CONST+j];
          label_list[r-1] = i / (PAR_FACTOR / 10);
        }
      }
    }
  }

  // vote
  INCREMENT: for (int i = 0;i < K_CONST; i ++ )
  {
    #pragma HLS pipeline
    vote_list[label_list[i]] += 1;
  }

  LabelType max_vote;
  max_vote = 0;

  // find the maximum value
  VOTE: for (int i = 0;i < 10; i ++ )
  {
    #pragma HLS unroll
    if(vote_list[i] >= vote_list[max_vote])
    {
      max_vote = i;
    }
  }

  return max_vote;

}

int knn_init_module (
	int knn_set[NUM_TEST][PAR_FACTOR * K_CONST],
	ap_int<64> *dummy_port_1,
	int* dummy_port_2,
	int* dummy_port_3,
	int* dummy_port_4,
	int* dummy_port_5,
	int* dummy_port_6,
	int* dummy_port_7,
	int* dummy_port_8,
	int* dummy_port_9
){
	#pragma HLS interface s_axilite port=return bundle=ctrl
	#pragma HLS interface m_axi port=knn_set offset=slave bundle=data
	#pragma HLS interface m_axi port=dummy_port_1 offset=slave bundle=data
	#pragma HLS interface m_axi port=dummy_port_2 offset=slave bundle=data
	#pragma HLS interface m_axi port=dummy_port_3 offset=slave bundle=data
	#pragma HLS interface m_axi port=dummy_port_4 offset=slave bundle=data
	#pragma HLS interface m_axi port=dummy_port_5 offset=slave bundle=data
	#pragma HLS interface m_axi port=dummy_port_6 offset=slave bundle=data
	#pragma HLS interface m_axi port=dummy_port_7 offset=slave bundle=data
	#pragma HLS interface m_axi port=dummy_port_8 offset=slave bundle=data
	#pragma HLS interface m_axi port=dummy_port_9 offset=slave bundle=data


	for (int t = 0; t < NUM_TEST; ++t)
	{
		for ( int i = 0; i < K_CONST * PAR_FACTOR ; ++i )
		{
		  #pragma HLS pipeline
		  knn_set[t][i] = 256;
		}
	}
	return 0;
}


int train_module (
//	WholeDigitType training_set[NUM_TRAINING],
//	WholeDigitType test_set[NUM_TEST],
	const DigitType training_set[NUM_TRAINING*4],
	const DigitType test_set[NUM_TEST*4],
	int knn_set[NUM_TEST][PAR_FACTOR * K_CONST],
	int* dummy_port_1,
	int* dummy_port_2,
	int* dummy_port_3,
	int* dummy_port_4,
	int* dummy_port_5,
	int* dummy_port_6,
	int* dummy_port_7
){
	#pragma HLS interface s_axilite port=return bundle=ctrl
	#pragma HLS interface m_axi port=training_set offset=slave bundle=data
	#pragma HLS interface m_axi port=test_set offset=slave bundle=data
	#pragma HLS interface m_axi port=knn_set offset=slave bundle=data
	#pragma HLS interface m_axi port=dummy_port_1 offset=slave bundle=data
	#pragma HLS interface m_axi port=dummy_port_2 offset=slave bundle=data
	#pragma HLS interface m_axi port=dummy_port_3 offset=slave bundle=data
	#pragma HLS interface m_axi port=dummy_port_4 offset=slave bundle=data
	#pragma HLS interface m_axi port=dummy_port_5 offset=slave bundle=data
	#pragma HLS interface m_axi port=dummy_port_6 offset=slave bundle=data
	#pragma HLS interface m_axi port=dummy_port_7 offset=slave bundle=data

  // loop through test set
  TEST_LOOP: for (int t = 0; t < NUM_TEST; ++t)
  {
	TRAINING_LOOP : for ( int i = 0; i < NUM_TRAINING / PAR_FACTOR; ++i )
	{
	  //#pragma HLS pipeline
	  LANES : for ( int j = 0; j < PAR_FACTOR; j++ )
	  {
		//#pragma HLS pipeline
		//#pragma HLS unroll
		update_knn( &test_set[t*4], &training_set[(j * NUM_TRAINING / PAR_FACTOR + i)*4], &knn_set[t][j * K_CONST] );
	  }
	}
  }
  return 0;
}

int predict_module(
	LabelType global_results[NUM_TEST],
	int knn_set[NUM_TEST][PAR_FACTOR * K_CONST],
	ap_int<64>* dummy_port_1,
	int* dummy_port_2,
	int* dummy_port_3,
	int* dummy_port_4,
	int* dummy_port_5,
	int* dummy_port_6,
	int* dummy_port_7
){
	#pragma HLS interface s_axilite port=return bundle=ctrl
	#pragma HLS interface m_axi port=global_results offset=slave bundle=data
	#pragma HLS interface m_axi port=knn_set offset=slave bundle=data
	#pragma HLS interface m_axi port=dummy_port_1 offset=slave bundle=data
	#pragma HLS interface m_axi port=dummy_port_2 offset=slave bundle=data
	#pragma HLS interface m_axi port=dummy_port_3 offset=slave bundle=data
	#pragma HLS interface m_axi port=dummy_port_4 offset=slave bundle=data
	#pragma HLS interface m_axi port=dummy_port_5 offset=slave bundle=data
	#pragma HLS interface m_axi port=dummy_port_6 offset=slave bundle=data
	#pragma HLS interface m_axi port=dummy_port_7 offset=slave bundle=data

	//local buffer
	static LabelType results           [NUM_TEST];

	// loop through test set
	  TEST_LOOP: for (int t = 0; t < NUM_TEST; ++t)
	  {
		// Compute the final output
		 LabelType max_vote = knn_vote(knn_set[t]);
		 results[t] = max_vote;
	  }

	  // copy the results out
	  for (int i = 0; i < NUM_TEST; i ++ ){
		//#pragma HLS pipeline
		global_results[i] = results[i];
	  }
	 return 0;
}

//void DigitRec (
////	WholeDigitType global_training_set[NUM_TRAINING],
////	WholeDigitType global_test_set[NUM_TEST],
//	const DigitType global_training_set[NUM_TRAINING*4],
//	const DigitType global_test_set[NUM_TEST*4],
//	LabelType global_results[NUM_TEST]
//){
//  //This array stores K minimum distances per training set
//  int knn_set[NUM_TEST][PAR_FACTOR * K_CONST];
//  knn_init_module(knn_set);
//  printf("training start\n");
//  train_module ( global_training_set, global_test_set, knn_set);
//  for(int t = 0;t < 10;t ++){
//	  for(int j = 0;j < PAR_FACTOR * K_CONST;j ++){
//		  printf("%d ", knn_set[t][j]);
//	  }
//	  printf("\n\r");
//  }
//  predict_module (global_results, knn_set);
//  return;
//}

