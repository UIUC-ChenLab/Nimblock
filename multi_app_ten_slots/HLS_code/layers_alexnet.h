#ifndef _LAYERS_H
#define _LAYERS_H
#include <algorithm>

using namespace std;

//HLS Versions
#define MAX_SLOTS 2

#define conv1_slots 4
#define conv1_output 96
int convolution1_hls(float bottom[3][227][227], float weights[96][3][11][11], float g_bias[96], float top[96][31][31], int output_offset);

#define conv2_slots 4
#define conv2_output 128
int convolution2_hls(float bottom[96][31][31], float g_weights[256][48][5][5], float g_bias[256], float top[256][15][15], int output_offset, int input_offset);

#define conv3_slots 4
#define conv3_output 384
int convolution3_hls(float bottom[256][15][15], float weights[384][256][3][3], float bias[384], float top[384][15][15], int output_offset);

#define conv4_slots 4
#define conv4_output 192
int convolution4_hls(float bottom[384][15][15], float weights[384][192][3][3], float bias[384], float top[384][15][15], int output_offset, int input_offset);

#define conv5_slots 4
#define conv5_output 128
int convolution5_hls(float bottom[384][15][15], float weights[256][192][3][3], float bias[256], float top[256][6][6], int output_offset, int input_offset);

#define conv6_slots MAX_SLOTS
#define conv6_output 256
int fullconnection6_hls(const float bottom[256][6][6], const float weights[256][256][6][6], const float bias[256], float top[256], int output_offset);

#define conv7_slots MAX_SLOTS
#define conv7_output 256
int fullconnection7_hls(const float bottom[256], const float weights[256][256], const float bias[256], float top[256], int output_offset);

#define conv8_slots MAX_SLOTS
#define conv8_output 1000
int fullconnection8_hls(const float bottom[256], const float weights[1000][256], const float bias[1000], float top[1000], int output_offset);

#endif
