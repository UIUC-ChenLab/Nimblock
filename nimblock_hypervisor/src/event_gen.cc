#include "event_gen.h"

/* Data for Lenet */
#define MAX_ARG_LENET 6
int num_dep_lenet[NUM_IP_LENET] = {0, 1, 1};
int dep_list_lenet[NUM_IP_LENET][NUM_IP_LENET] = {
	{-1, -1, -1},
	{0, -1, -1},
	{1, -1, -1}
};
char *bin_names_lenet[NUM_IP_LENET][NUM_SLOTS] = {
	{"s0c12.bin", "s1c12.bin", "s2c12.bin", "s3c12.bin", "s4c12.bin", "s5c12.bin", "s6c12.bin", "s7c12.bin", "s8c12.bin", "s9c12.bin"},
	{"s0c13.bin", "s1c13.bin", "s2c13.bin", "s3c13.bin", "s4c13.bin", "s5c13.bin", "s6c13.bin", "s7c13.bin", "s8c13.bin", "s9c13.bin"},
	{"s0c3.bin", "s1c3.bin", "s2c3.bin", "s3c3.bin", "s4c3.bin", "s5c3.bin", "s6c3.bin", "s7c3.bin", "s8c3.bin", "s9c3.bin"},
};
int num_arg_lenet[NUM_IP_LENET] = {4, 4, 6};
int arg_size_lenet[NUM_IP_LENET][MAX_ARG_LENET] = {{32*32*sizeof(float), 6*5*5*sizeof(float), 6*sizeof(float), 6*14*14*sizeof(float), -1, -1},
                                                   {6*14*14*sizeof(float), 16*6*5*5*sizeof(float), 16*sizeof(float), 16*5*5*sizeof(float), -1, -1}, 
                                                   {16*5*5*sizeof(float), 120*16*5*5*sizeof(float), 120*sizeof(float), 10*120*sizeof(float), 10*sizeof(float), 10*sizeof(float)}};
u64 lat_lenet[NUM_IP_LENET] = {7130, 5115, 5270}; 
float func_lenet[FUNC_COEFFS] = {1.14285714, 0.78571429, 0.07142857};
/* End data for Lenet */

/* Data for KNN */
// Whole digit type is 32 chars in size 
// Label type is 1 char in size
// Training set global is NUM_TRAINING*sizeof(whole digit) + 32
// Testing set global is NUM_TEST*size of (whole digit) + 32
// knn set global is NUM_TEST*PAR_FACTOR_K_CONST*sizeof(int ) +32
// results global is NUM_TEST*sizeof(label type) + 32
#define MAX_ARG_KNN 3
#define NUM_TRAINING 18000
#define CLASS_SIZE 1800
#define NUM_TEST 2000
#define K_CONST 3
#define PAR_FACTOR 10
int num_dep_knn[NUM_IP_KNN] = {0, 1, 1};
int dep_list_knn[NUM_IP_KNN][NUM_IP_KNN] = {
	{-1, -1, -1},
	{0, -1, -1},
	{1, -1, -1}
};
char *bin_names_knn[NUM_IP_KNN][NUM_SLOTS] = {
	{"s0c19.bin", "s1c19.bin", "s2c19.bin", "s3c19.bin", "s4c19.bin", "s5c19.bin", "s6c19.bin", "s7c19.bin", "s8c19.bin", "s9c19.bin"},
	{"s0c20.bin", "s1c20.bin", "s2c20.bin", "s3c20.bin", "s4c20.bin", "s5c20.bin", "s6c20.bin", "s7c20.bin", "s8c20.bin", "s9c20.bin"},
	{"s0c21.bin", "s1c21.bin", "s2c21.bin", "s3c21.bin", "s4c21.bin", "s5c21.bin", "s6c21.bin", "s7c21.bin", "s8c21.bin", "s9c21.bin"},
};
int num_arg_knn[NUM_IP_KNN] = {1, 3, 2};
int arg_size_knn[NUM_IP_KNN][MAX_ARG_KNN] = {{NUM_TEST*PAR_FACTOR*K_CONST*sizeof(int) + 32, -1, -1},
                                             {NUM_TRAINING*32*sizeof(char)+32, NUM_TEST*32*sizeof(char)+32, NUM_TEST*PAR_FACTOR*K_CONST*sizeof(int) + 32},
                                             {NUM_TEST*sizeof(char) + 32, NUM_TEST*PAR_FACTOR*K_CONST*sizeof(int) + 32, -1}};
u64 lat_knn[NUM_IP_KNN] = {8060, 101605135, 1240};
float func_knn[FUNC_COEFFS] = {1.0, 0.0, 0.0};
/* End data for KNN */

/* Data for 3DR */
#define MAX_ARG_3DR 9
#define NUM_FB 16384
int num_dep_3dr[NUM_IP_3DR] = {0, 1, 1};
int dep_list_3dr[NUM_IP_3DR][NUM_IP_3DR] = {
	{-1, -1, -1},
	{0, -1, -1},
	{1, -1, -1}
};
char *bin_names_3dr[NUM_IP_3DR][NUM_SLOTS] = {
	{"s0c31.bin", "s1c31.bin", "s2c31.bin", "s3c31.bin", "s4c31.bin", "s5c31.bin", "s6c31.bin", "s7c31.bin", "s8c31.bin", "s9c31.bin"},
	{"s0c0.bin", "s1c0.bin", "s2c0.bin", "s3c0.bin", "s4c0.bin", "s5c0.bin", "s6c0.bin", "s7c0.bin", "s8c0.bin", "s9c0.bin"},
	{"s0c2.bin", "s1c2.bin", "s2c2.bin", "s3c2.bin", "s4c2.bin", "s5c2.bin", "s6c2.bin", "s7c2.bin", "s8c2.bin", "s9c2.bin"},
};

int num_arg_3dr[NUM_IP_3DR] = {6, 9, 5};
int arg_size_3dr[NUM_IP_3DR][MAX_ARG_3DR] = {{3*NUM_3D_TRI*sizeof(u32), 500*NUM_3D_TRI*sizeof(u8), 500*NUM_3D_TRI*sizeof(u8), 500*NUM_3D_TRI*sizeof(u8), 500*NUM_3D_TRI*sizeof(u8), NUM_3D_TRI*sizeof(u16), -1, -1, -1},
                                             {500 * NUM_3D_TRI * sizeof(u8),500 * NUM_3D_TRI * sizeof(u8),500 * NUM_3D_TRI * sizeof(u8), 500 * NUM_3D_TRI * sizeof(u8), NUM_3D_TRI*sizeof(u16), 500 * NUM_3D_TRI * sizeof(u8), 500 * NUM_3D_TRI * sizeof(u8), 500 * NUM_3D_TRI * sizeof(u8), NUM_3D_TRI*sizeof(u16)},
                                             {NUM_3D_TRI*sizeof(u16), 500 * NUM_3D_TRI * sizeof(u8), 500 * NUM_3D_TRI * sizeof(u8), 500 * NUM_3D_TRI * sizeof(u8), NUM_FB * sizeof(u32), -1, -1, -1, -1}};
u64 lat_3dr[NUM_IP_3DR] = {42315, 17205, 8370};

enum arg_proto linked_args_3dr[NUM_IP_3DR][MAX_ARG_3DR] = {
    {FREE, PASS, PASS, PASS, PASS, PASS, INVALID, INVALID, INVALID},
    {PASSED, PASSED, PASSED, PASSED, PASSED, PASS, PASS, PASS, PASS},
    {PASSED, PASSED, PASSED, PASSED, FREE, INVALID, INVALID, INVALID, INVALID}
};

struct arg_location arg_locations_3dr[NUM_IP_3DR][MAX_ARG_3DR] = {
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0}, {0}, {0}, {0}},
    {{1, 8}, {1, 5}, {1, 6}, {1, 7}, {0}, {0}, {0}, {0}, {0}}
};
float func_3dr[FUNC_COEFFS] = {2.33333333, -0.5, 0.16666667};
/* End data for 3DR */

/* Data for Image Compression */
#define MAX_ARG_IMG_CMP 6
#define IMG_CMP_SIZE 2073600
int num_dep_img_cmp[NUM_IP_IMG_CMP] = {0, 1, 1, 1, 1, 1};
int dep_list_img_cmp[NUM_IP_IMG_CMP][NUM_IP_IMG_CMP] = {
	{-1, -1, -1, -1, -1, -1},
	{0, -1, -1, -1, -1, -1},
	{1, -1, -1, -1, -1, -1},
	{2, -1, -1, -1, -1, -1},
	{3, -1, -1, -1, -1, -1},
	{4, -1, -1, -1, -1, -1}
};
char *bin_names_img_cmp[NUM_IP_IMG_CMP][NUM_SLOTS] = {
	{"s0c4.bin", "s1c4.bin", "s2c4.bin", "s3c4.bin", "s4c4.bin", "s5c4.bin", "s6c4.bin", "s7c4.bin", "s8c4.bin", "s9c4.bin"},
	{"s0c14.bin", "s1c14.bin", "s2c14.bin", "s3c14.bin", "s4c14.bin", "s5c14.bin", "s6c14.bin", "s7c14.bin", "s8c14.bin", "s9c14.bin"},
	{"s0c15.bin", "s1c15.bin", "s2c15.bin", "s3c15.bin", "s4c15.bin", "s5c15.bin", "s6c15.bin", "s7c15.bin", "s8c15.bin", "s9c15.bin"},
	{"s0c16.bin", "s1c16.bin", "s2c16.bin", "s3c16.bin", "s4c16.bin", "s5c16.bin", "s6c16.bin", "s7c16.bin", "s8c16.bin", "s9c16.bin"},
	{"s0c17.bin", "s1c17.bin", "s2c17.bin", "s3c17.bin", "s4c17.bin", "s5c17.bin", "s6c17.bin", "s7c17.bin", "s8c17.bin", "s9c17.bin"},
	{"s0c18.bin", "s1c18.bin", "s2c18.bin", "s3c18.bin", "s4c18.bin", "s5c18.bin", "s6c18.bin", "s7c18.bin","s8c18.bin", "s9c18.bin"},
};
int num_arg_img_cmp[NUM_IP_IMG_CMP] = {6, 4, 4, 4, 4, 4};
int arg_size_img_cmp[NUM_IP_IMG_CMP][MAX_ARG_IMG_CMP] = {{IMG_CMP_SIZE*sizeof(float), IMG_CMP_SIZE*sizeof(float), -2, -2, -2, -2},
                                                         {IMG_CMP_SIZE*sizeof(float), IMG_CMP_SIZE*sizeof(float), -2, -2, -1, -1},
                                                         {IMG_CMP_SIZE*sizeof(float), IMG_CMP_SIZE*sizeof(float), -2, -2, -1, -1},
                                                         {IMG_CMP_SIZE*sizeof(float), IMG_CMP_SIZE*sizeof(float), -2, -2, -1, -1},
                                                         {IMG_CMP_SIZE*sizeof(float), IMG_CMP_SIZE*sizeof(float), -2, -2, -1, -1},
                                                         {IMG_CMP_SIZE*sizeof(float), IMG_CMP_SIZE*sizeof(float), -2, -2, -1, -1}};
u64 lat_img_cmp[NUM_IP_IMG_CMP] = {2635, 1240, 775, 775, 1240, 155};
float func_img_cmp[FUNC_COEFFS] ={1.45098039,  0.41429475, -0.01201771};
/* End data for Image compression */

/* Data for Optical Flow */
typedef u16 input_t;
typedef u32 pixel_t;
typedef u32 outer_pixel_t;
typedef u64 calc_pixel_t;
typedef u16 vel_pixel_t;
typedef struct{
    outer_pixel_t val[6];
}outer_t;
typedef struct{
    outer_pixel_t val[6];
}tensor_t;
typedef u64 frames_t;
#define MAX_HEIGHT 436
#define MAX_WIDTH 256
#define BUF_SIZE MAX_HEIGHT*MAX_WIDTH
#define MAX_ARG_OPTICAL_FLOW 8
int num_dep_opt_flow[NUM_IP_OPTICAL_FLOW] = {0, 1, 1, 1, 1, 1, 1, 1, 1};
int dep_list_opt_flow[NUM_IP_OPTICAL_FLOW][NUM_IP_OPTICAL_FLOW] = {
	{-1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, -1, -1, -1, -1, -1, -1, -1, -1},
	{5, -1, -1, -1, -1, -1, -1, -1, -1},
	{6, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, -1, -1, -1, -1, -1, -1, -1, -1}
};
char *bin_names_opt_flow[NUM_IP_OPTICAL_FLOW][NUM_SLOTS] = {
	{"s0c22.bin", "s1c22.bin", "s2c22.bin", "s3c22.bin", "s4c22.bin", "s5c22.bin", "s6c22.bin", "s7c22.bin", "s8c22.bin", "s9c22.bin"},
	{"s0c23.bin", "s1c23.bin", "s2c23.bin", "s3c23.bin", "s4c23.bin", "s5c23.bin", "s6c23.bin", "s7c23.bin", "s8c23.bin", "s9c23.bin"},
	{"s0c24.bin", "s1c24.bin", "s2c24.bin", "s3c24.bin", "s4c24.bin", "s5c24.bin", "s6c24.bin", "s7c24.bin", "s8c24.bin", "s9c24.bin"},
	{"s0c25.bin", "s1c25.bin", "s2c25.bin", "s3c25.bin", "s4c25.bin", "s5c25.bin", "s6c25.bin", "s7c25.bin", "s8c25.bin", "s9c25.bin"},
	{"s0c26.bin", "s1c26.bin", "s2c26.bin", "s3c26.bin", "s4c26.bin", "s5c26.bin", "s6c26.bin", "s7c26.bin", "s8c26.bin", "s9c26.bin"},
	{"s0c27.bin", "s1c27.bin", "s2c27.bin", "s3c27.bin", "s4c27.bin", "s5c27.bin", "s6c27.bin", "s7c27.bin", "s8c27.bin", "s9c27.bin"},
	{"s0c28.bin", "s1c28.bin", "s2c28.bin", "s3c28.bin", "s4c28.bin", "s5c28.bin", "s6c28.bin", "s7c28.bin", "s8c28.bin", "s9c28.bin"},
	{"s0c29.bin", "s1c29.bin", "s2c29.bin", "s3c29.bin", "s4c29.bin", "s5c29.bin", "s6c29.bin", "s7c29.bin", "s8c29.bin", "s9c29.bin"},
	{"s0c30.bin", "s1c30.bin", "s2c30.bin", "s3c30.bin", "s4c30.bin", "s5c30.bin", "s6c30.bin", "s7c30.bin", "s8c30.bin", "s9c30.bin"},
};
int num_arg_opt_flow[NUM_IP_OPTICAL_FLOW] = {7, 3, 6, 7, 8, 5, 2, 2, 3};
int arg_size_opt_flow[NUM_IP_OPTICAL_FLOW][MAX_ARG_OPTICAL_FLOW] = {{BUF_SIZE*sizeof(frames_t), BUF_SIZE*sizeof(input_t), BUF_SIZE*sizeof(input_t), BUF_SIZE*sizeof(input_t), BUF_SIZE*sizeof(input_t), BUF_SIZE*sizeof(input_t), BUF_SIZE*sizeof(input_t), -1},
                                                                    {BUF_SIZE*sizeof(input_t), BUF_SIZE*sizeof(pixel_t), BUF_SIZE*sizeof(pixel_t), -1, -1, -1, -1, -1},
                                                                    {BUF_SIZE*sizeof(input_t), BUF_SIZE*sizeof(input_t), BUF_SIZE*sizeof(input_t), BUF_SIZE*sizeof(input_t), BUF_SIZE*sizeof(input_t), BUF_SIZE*sizeof(pixel_t), -1, -1},
                                                                    {BUF_SIZE*sizeof(input_t), BUF_SIZE*sizeof(input_t), BUF_SIZE*sizeof(input_t), BUF_SIZE*sizeof(pixel_t), BUF_SIZE*sizeof(pixel_t), BUF_SIZE*sizeof(pixel_t), BUF_SIZE*sizeof(pixel_t), -1},
                                                                    {BUF_SIZE*sizeof(pixel_t), BUF_SIZE*sizeof(pixel_t), BUF_SIZE*sizeof(pixel_t), BUF_SIZE*sizeof(pixel_t), BUF_SIZE*sizeof(pixel_t), BUF_SIZE*sizeof(pixel_t), BUF_SIZE*sizeof(pixel_t), BUF_SIZE*sizeof(pixel_t)},
                                                                    {BUF_SIZE*sizeof(pixel_t), BUF_SIZE*sizeof(pixel_t), BUF_SIZE*sizeof(pixel_t), BUF_SIZE*sizeof(pixel_t), BUF_SIZE*sizeof(outer_t), -1, -1, -1},
                                                                    {BUF_SIZE*sizeof(outer_t), BUF_SIZE*sizeof(tensor_t), -1, -1, -1, -1, -1, -1},
                                                                    {BUF_SIZE*sizeof(tensor_t), BUF_SIZE*sizeof(tensor_t), -1, -1, -1, -1, -1, -1},
                                                                    {BUF_SIZE*sizeof(tensor_t), BUF_SIZE*sizeof(vel_pixel_t), BUF_SIZE*sizeof(vel_pixel_t), -1, -1, -1, -1, -1}};
u64 lat_opt_flow[NUM_IP_OPTICAL_FLOW] = {90830, 15035, 14880, 84475, 86025, 88970, 89125, 89280, 177940};
float func_opt_flow[FUNC_COEFFS] = {2.2375, 0.39697051, -0.00691146};
/* End data for Optical Flow */

/* Data for Alexnet */
#define MAX_ARG_ALEXNET 6
#define ALEXNET_CONV1_SLOTS 4
#define ALEXNET_CONV1_OUTPUT 96
#define ALEXNET_CONV2_SLOTS 4
#define ALEXNET_CONV2_OUTPUT 128
#define ALEXNET_CONV3_SLOTS 4
#define ALEXNET_CONV3_OUTPUT 384
#define ALEXNET_CONV4_SLOTS 4
#define ALEXNET_CONV4_OUTPUT 192
#define ALEXNET_CONV5_SLOTS 4
#define ALEXNET_CONV5_OUTPUT 128
#define ALEXNET_FC6_SLOTS 2
#define ALEXNET_FC6_OUTPUT 256
#define ALEXNET_FC7_SLOTS 2
#define ALEXNET_FC7_OUTPUT 256
#define ALEXNET_FC8_SLOTS 2
#define ALEXNET_FC8_OUTPUT 1000

int num_dep_alexnet[NUM_IP_ALEXNET] ={
		0,		// layer 1_0
		0,		// layer 1_1
		0,		// layer 1_2
		0,		// layer 1_3
		4,		// layer 2a_0
		4,		// layer 2a_1
		4,		// layer 2a_2
		4,		// layer 2a_3
		4,		// layer 2b_0
		4,		// layer 2b_1
		4,		// layer 2b_2
		4,		// layer 2b_3
		8,		// layer 3_0
		8,		// layer 3_1
		8,		// layer 3_2
		8,		// layer 3_3
		4,		// layer 4a_0
		4,		// layer 4a_1
		4,		// layer 4a_2
		4,		// layer 4a_3
		4,		// layer 4b_0
		4,		// layer 4b_1
		4,		// layer 4b_2
		4,		// layer 4b_3
		8,		// layer 5a_0
		8,		// layer 5a_1
		8,		// layer 5a_2
		8,		// layer 5a_3
		8,		// layer 5b_0
		8,		// layer 5b_1
		8,		// layer 5b_2
		8,		// layer 5b_3
		8,		// layer 6_0
		8,		// layer 6_1
		2,		// layer 7_0
		2,		// layer 7_1
		2,		// layer 8_0
		2		// layer 8_1
};
int dep_list_alexnet[NUM_IP_ALEXNET][NUM_IP_ALEXNET] = {
	// layer 1_0
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 1_1
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 1_2
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 1_3
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 2a_0
	{0, 1, 2, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 2a_1
	{0, 1, 2, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 2a_2
	{0, 1, 2, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 2a_3
	{0, 1, 2, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 2b_0
	{0, 1, 2, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 2b_1
	{0, 1, 2, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 2b_2
	{0, 1, 2, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 2b_3
	{0, 1, 2, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 3_0
	{4, 5, 6, 7, 8, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 3_1
	{4, 5, 6, 7, 8, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 3_2
	{4, 5, 6, 7, 8, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 3_3
	{4, 5, 6, 7, 8, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 4a_0
	{12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 4a_1
	{12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 4a_2
	{12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 4a_3
	{12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 4b_0
	{12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 4b_1
	{12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 4b_2
	{12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 4b_3
	{12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 5a_0
	{16, 17, 18, 19, 20, 21, 22, 23, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 5a_1
	{16, 17, 18, 19, 20, 21, 22, 23, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 5a_2
	{16, 17, 18, 19, 20, 21, 22, 23, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 5a_3
	{16, 17, 18, 19, 20, 21, 22, 23, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 5b_0
	{16, 17, 18, 19, 20, 21, 22, 23, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 5b_1
	{16, 17, 18, 19, 20, 21, 22, 23, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 5b_2
	{16, 17, 18, 19, 20, 21, 22, 23, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 5b_3
	{16, 17, 18, 19, 20, 21, 22, 23, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 6_0
	{24, 25, 26, 27, 28, 29, 30, 31, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 6_1
	{24, 25, 26, 27, 28, 29, 30, 31, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 7_0
	{32, 33, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 7_1
	{32, 33, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 8_0
	{34, 35, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	// layer 8_1
	{34, 35, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
};
char *bin_names_alexnet[NUM_IP_ALEXNET][NUM_SLOTS] = {
	// layer 1_0
	{"s0c5.bin", "s1c5.bin", "s2c5.bin", "s3c5.bin", "s4c5.bin", "s5c5.bin", "s6c5.bin", "s7c5.bin", "s8c5.bin", "s9c5.bin"},
	// layer 1_1
	{"s0c5.bin", "s1c5.bin", "s2c5.bin", "s3c5.bin", "s4c5.bin", "s5c5.bin", "s6c5.bin", "s7c5.bin", "s8c5.bin", "s9c5.bin"},
	// layer 1_2
	{"s0c5.bin", "s1c5.bin", "s2c5.bin", "s3c5.bin", "s4c5.bin", "s5c5.bin", "s6c5.bin", "s7c5.bin", "s8c5.bin", "s9c5.bin"},
	// layer 1_3
	{"s0c5.bin", "s1c5.bin", "s2c5.bin", "s3c5.bin", "s4c5.bin", "s5c5.bin", "s6c5.bin", "s7c5.bin", "s8c5.bin", "s9c5.bin"},
	// layer 2a_0
	{"s0c6.bin", "s1c6.bin", "s2c6.bin", "s3c6.bin", "s4c6.bin", "s5c6.bin", "s6c6.bin", "s7c6.bin", "s8c6.bin", "s9c6.bin"},
	// layer 2a_1
	{"s0c6.bin", "s1c6.bin", "s2c6.bin", "s3c6.bin", "s4c6.bin", "s5c6.bin", "s6c6.bin", "s7c6.bin", "s8c6.bin", "s9c6.bin"},
	// layer 2a_2
	{"s0c6.bin", "s1c6.bin", "s2c6.bin", "s3c6.bin", "s4c6.bin", "s5c6.bin", "s6c6.bin", "s7c6.bin", "s8c6.bin", "s9c6.bin"},
	// layer 2a_3
	{"s0c6.bin", "s1c6.bin", "s2c6.bin", "s3c6.bin", "s4c6.bin", "s5c6.bin", "s6c6.bin", "s7c6.bin", "s8c6.bin", "s9c6.bin"},
	// layer 2b_0
	{"s0c6.bin", "s1c6.bin", "s2c6.bin", "s3c6.bin", "s4c6.bin", "s5c6.bin", "s6c6.bin", "s7c6.bin", "s8c6.bin", "s9c6.bin"},
	// layer 2b_1
	{"s0c6.bin", "s1c6.bin", "s2c6.bin", "s3c6.bin", "s4c6.bin", "s5c6.bin", "s6c6.bin", "s7c6.bin", "s8c6.bin", "s9c6.bin"},
	// layer 2b_2
	{"s0c6.bin", "s1c6.bin", "s2c6.bin", "s3c6.bin", "s4c6.bin", "s5c6.bin", "s6c6.bin", "s7c6.bin", "s8c6.bin", "s9c6.bin"},
	// layer 2b_3
	{"s0c6.bin", "s1c6.bin", "s2c6.bin", "s3c6.bin", "s4c6.bin", "s5c6.bin", "s6c6.bin", "s7c6.bin", "s8c6.bin", "s9c6.bin"},
	// layer 3_0
	{"s0c7.bin", "s1c7.bin", "s2c7.bin", "s3c7.bin", "s4c7.bin", "s5c7.bin", "s6c7.bin", "s7c7.bin", "s8c7.bin", "s9c7.bin"},
	// layer 3_1
	{"s0c7.bin", "s1c7.bin", "s2c7.bin", "s3c7.bin", "s4c7.bin", "s5c7.bin", "s6c7.bin", "s7c7.bin", "s8c7.bin", "s9c7.bin"},
	// layer 3_2
	{"s0c7.bin", "s1c7.bin", "s2c7.bin", "s3c7.bin", "s4c7.bin", "s5c7.bin", "s6c7.bin", "s7c7.bin", "s8c7.bin", "s9c7.bin"},
	// layer 3_3
	{"s0c7.bin", "s1c7.bin", "s2c7.bin", "s3c7.bin", "s4c7.bin", "s5c7.bin", "s6c7.bin", "s7c7.bin", "s8c7.bin", "s9c7.bin"},
	// layer 4a_0
	{"s0c8.bin", "s1c8.bin", "s2c8.bin", "s3c8.bin", "s4c8.bin", "s5c8.bin", "s6c8.bin", "s7c8.bin", "s8c8.bin", "s9c8.bin"},
	// layer 4a_1
	{"s0c8.bin", "s1c8.bin", "s2c8.bin", "s3c8.bin", "s4c8.bin", "s5c8.bin", "s6c8.bin", "s7c8.bin", "s8c8.bin", "s9c8.bin"},
	// layer 4a_2
	{"s0c8.bin", "s1c8.bin", "s2c8.bin", "s3c8.bin", "s4c8.bin", "s5c8.bin", "s6c8.bin", "s7c8.bin", "s8c8.bin", "s9c8.bin"},
	// layer 4a_3
	{"s0c8.bin", "s1c8.bin", "s2c8.bin", "s3c8.bin", "s4c8.bin", "s5c8.bin", "s6c8.bin", "s7c8.bin", "s8c8.bin", "s9c8.bin"},
	// layer 4b_0
	{"s0c8.bin", "s1c8.bin", "s2c8.bin", "s3c8.bin", "s4c8.bin", "s5c8.bin", "s6c8.bin", "s7c8.bin", "s8c8.bin", "s9c8.bin"},
	// layer 4b_1
	{"s0c8.bin", "s1c8.bin", "s2c8.bin", "s3c8.bin", "s4c8.bin", "s5c8.bin", "s6c8.bin", "s7c8.bin", "s8c8.bin", "s9c8.bin"},
	// layer 4b_2
	{"s0c8.bin", "s1c8.bin", "s2c8.bin", "s3c8.bin", "s4c8.bin", "s5c8.bin", "s6c8.bin", "s7c8.bin", "s8c8.bin", "s9c8.bin"},
	// layer 4b_3
	{"s0c8.bin", "s1c8.bin", "s2c8.bin", "s3c8.bin", "s4c8.bin", "s5c8.bin", "s6c8.bin", "s7c8.bin", "s8c8.bin", "s9c8.bin"},
	// layer 5a_0
	{"s0c9.bin", "s1c9.bin", "s2c9.bin", "s3c9.bin", "s4c9.bin", "s5c9.bin", "s6c9.bin", "s7c9.bin", "s8c9.bin", "s9c9.bin"},
	// layer 5a_1
	{"s0c9.bin", "s1c9.bin", "s2c9.bin", "s3c9.bin", "s4c9.bin", "s5c9.bin", "s6c9.bin", "s7c9.bin", "s8c9.bin", "s9c9.bin"},
	// layer 5a_2
	{"s0c9.bin", "s1c9.bin", "s2c9.bin", "s3c9.bin", "s4c9.bin", "s5c9.bin", "s6c9.bin", "s7c9.bin", "s8c9.bin", "s9c9.bin"},
	// layer 5a_3
	{"s0c9.bin", "s1c9.bin", "s2c9.bin", "s3c9.bin", "s4c9.bin", "s5c9.bin", "s6c9.bin", "s7c9.bin", "s8c9.bin", "s9c9.bin"},
	// layer 5b_0
	{"s0c9.bin", "s1c9.bin", "s2c9.bin", "s3c9.bin", "s4c9.bin", "s5c9.bin", "s6c9.bin", "s7c9.bin", "s8c9.bin", "s9c9.bin"},
	// layer 5b_1
	{"s0c9.bin", "s1c9.bin", "s2c9.bin", "s3c9.bin", "s4c9.bin", "s5c9.bin", "s6c9.bin", "s7c9.bin", "s8c9.bin", "s9c9.bin"},
	// layer 5b_2
	{"s0c9.bin", "s1c9.bin", "s2c9.bin", "s3c9.bin", "s4c9.bin", "s5c9.bin", "s6c9.bin", "s7c9.bin", "s8c9.bin", "s9c9.bin"},
	// layer 5b_3
	{"s0c9.bin", "s1c9.bin", "s2c9.bin", "s3c9.bin", "s4c9.bin", "s5c9.bin", "s6c9.bin", "s7c9.bin", "s8c9.bin", "s9c9.bin"},
	// layer 6_0
	{"s0c1.bin", "s1c1.bin", "s2c1.bin", "s3c1.bin", "s4c1.bin", "s5c1.bin", "s6c1.bin", "s7c1.bin", "s8c1.bin", "s9c1.bin"},
	// layer 6_1
	{"s0c1.bin", "s1c1.bin", "s2c1.bin", "s3c1.bin", "s4c1.bin", "s5c1.bin", "s6c1.bin", "s7c1.bin", "s8c1.bin", "s9c1.bin"},
	// layer 7_0
	{"s0c10.bin", "s1c10.bin", "s2c10.bin", "s3c10.bin", "s4c10.bin", "s5c10.bin", "s6c10.bin", "s7c10.bin", "s8c10.bin", "s9c10.bin"},
	// layer 7_1
	{"s0c10.bin", "s1c10.bin", "s2c10.bin", "s3c10.bin", "s4c10.bin", "s5c10.bin", "s6c10.bin", "s7c10.bin", "s8c10.bin", "s9c10.bin"},
	// layer 8_0
	{"s0c11.bin", "s1c11.bin", "s2c11.bin", "s3c11.bin", "s4c11.bin", "s5c11.bin", "s6c11.bin", "s7c11.bin", "s8c11.bin", "s9c11.bin"},
	// layer 8_1
	{"s0c11.bin", "s1c11.bin", "s2c11.bin", "s3c11.bin", "s4c11.bin", "s5c11.bin", "s6c11.bin", "s7c11.bin", "s8c11.bin", "s9c11.bin"},
};
int num_arg_alexnet[NUM_IP_ALEXNET] = {
    // Conv 1
    5, 5, 5, 5,
    // Conv 2a
    6, 6, 6, 6,
    // Conv 2b
    6, 6, 6, 6,
    // Conv 3
    5, 5, 5, 5,
    // Conv 4a
    6, 6, 6, 6,
    // Conv 4b
    6, 6, 6, 6, 
    // Conv 5a
    6, 6, 6, 6,
    // Conv 5b
    6, 6, 6, 6,
    // fc6
    5, 5,
    // fc 7
    5, 5,
    // fc 8
    5, 5
};
int arg_size_alexnet[NUM_IP_ALEXNET][MAX_ARG_ALEXNET] = {
    // Conv 1
    {3*227*227*sizeof(float), 96*3*11*11*sizeof(float), 96, 96*31*31*sizeof(float), -2, -1},
    {3*227*227*sizeof(float), 96*3*11*11*sizeof(float), 96, 96*31*31*sizeof(float), -2, -1},
    {3*227*227*sizeof(float), 96*3*11*11*sizeof(float), 96, 96*31*31*sizeof(float), -2, -1},
    {3*227*227*sizeof(float), 96*3*11*11*sizeof(float), 96, 96*31*31*sizeof(float), -2, -1},
    // Conv 2a
    {96*31*31*sizeof(float), 256*48*5*5*sizeof(float), 256*sizeof(float), 256*15*15*sizeof(float), -2, -2},
    {96*31*31*sizeof(float), 256*48*5*5*sizeof(float), 256*sizeof(float), 256*15*15*sizeof(float), -2, -2},
    {96*31*31*sizeof(float), 256*48*5*5*sizeof(float), 256*sizeof(float), 256*15*15*sizeof(float), -2, -2},
    {96*31*31*sizeof(float), 256*48*5*5*sizeof(float), 256*sizeof(float), 256*15*15*sizeof(float), -2, -2},
    // Conv 2b
    {96*31*31*sizeof(float), 256*48*5*5*sizeof(float), 256*sizeof(float), 256*15*15*sizeof(float), -2, -2},
    {96*31*31*sizeof(float), 256*48*5*5*sizeof(float), 256*sizeof(float), 256*15*15*sizeof(float), -2, -2},
    {96*31*31*sizeof(float), 256*48*5*5*sizeof(float), 256*sizeof(float), 256*15*15*sizeof(float), -2, -2},
    {96*31*31*sizeof(float), 256*48*5*5*sizeof(float), 256*sizeof(float), 256*15*15*sizeof(float), -2, -2},
    // Conv 3
    {256*15*15*sizeof(float), 384*256*3*3*sizeof(float), 384*sizeof(float), 384*15*15*sizeof(float), -2, -1},
    {256*15*15*sizeof(float), 384*256*3*3*sizeof(float), 384*sizeof(float), 384*15*15*sizeof(float), -2, -1},
    {256*15*15*sizeof(float), 384*256*3*3*sizeof(float), 384*sizeof(float), 384*15*15*sizeof(float), -2, -1},
    {256*15*15*sizeof(float), 384*256*3*3*sizeof(float), 384*sizeof(float), 384*15*15*sizeof(float), -2, -1},
    // Conv 4a
    {384*15*15*sizeof(float), 384*192*3*3*sizeof(float), 384*sizeof(float), 384*15*15*sizeof(float), -2, -2},
    {384*15*15*sizeof(float), 384*192*3*3*sizeof(float), 384*sizeof(float), 384*15*15*sizeof(float), -2, -2},
    {384*15*15*sizeof(float), 384*192*3*3*sizeof(float), 384*sizeof(float), 384*15*15*sizeof(float), -2, -2},
    {384*15*15*sizeof(float), 384*192*3*3*sizeof(float), 384*sizeof(float), 384*15*15*sizeof(float), -2, -2},
    // Conv 4b
    {384*15*15*sizeof(float), 384*192*3*3*sizeof(float), 384*sizeof(float), 384*15*15*sizeof(float), -2, -2},
    {384*15*15*sizeof(float), 384*192*3*3*sizeof(float), 384*sizeof(float), 384*15*15*sizeof(float), -2, -2},
    {384*15*15*sizeof(float), 384*192*3*3*sizeof(float), 384*sizeof(float), 384*15*15*sizeof(float), -2, -2},
    {384*15*15*sizeof(float), 384*192*3*3*sizeof(float), 384*sizeof(float), 384*15*15*sizeof(float), -2, -2},
    // Conv 5a
    {384*15*15*sizeof(float), 256*192*3*3*sizeof(float), 256*sizeof(float), 256*6*6*sizeof(float), -2, -2},
    {384*15*15*sizeof(float), 256*192*3*3*sizeof(float), 256*sizeof(float), 256*6*6*sizeof(float), -2, -2},
    {384*15*15*sizeof(float), 256*192*3*3*sizeof(float), 256*sizeof(float), 256*6*6*sizeof(float), -2, -2},
    {384*15*15*sizeof(float), 256*192*3*3*sizeof(float), 256*sizeof(float), 256*6*6*sizeof(float), -2, -2},
    // Conv 5b
    {384*15*15*sizeof(float), 256*192*3*3*sizeof(float), 256*sizeof(float), 256*6*6*sizeof(float), -2, -2},
    {384*15*15*sizeof(float), 256*192*3*3*sizeof(float), 256*sizeof(float), 256*6*6*sizeof(float), -2, -2},
    {384*15*15*sizeof(float), 256*192*3*3*sizeof(float), 256*sizeof(float), 256*6*6*sizeof(float), -2, -2},
    {384*15*15*sizeof(float), 256*192*3*3*sizeof(float), 256*sizeof(float), 256*6*6*sizeof(float), -2, -2},
    // fc6
    {256*6*6*sizeof(float), 256*256*6*6*sizeof(float), 256*sizeof(float), 256*sizeof(float), -2, -1},
    {256*6*6*sizeof(float), 256*256*6*6*sizeof(float), 256*sizeof(float), 256*sizeof(float), -2, -1},
    // fc7
    {256*sizeof(float), 256*256*sizeof(float), 256*sizeof(float), 256*sizeof(float), -2, -1},
    {256*sizeof(float), 256*256*sizeof(float), 256*sizeof(float), 256*sizeof(float), -2, -1},
    // fc8
    {256*sizeof(float), 1000*256*sizeof(float), 1000*sizeof(float), 1000*sizeof(float), -2, -1},
	{256*sizeof(float), 1000*256*sizeof(float), 1000*sizeof(float), 1000*sizeof(float), -2, -1}
};
u64 lat_alexnet[NUM_IP_ALEXNET] = {
        // Conv 1
    270165, 270165, 270165, 270165,
    // Conv 2a
    259625, 259625, 259625, 259625,
    // Conv 2b
    259625, 259625, 259625, 259625,
    // Conv 3
    552885, 552885, 552885, 552885,
    // Conv 4a
    207390, 207390, 207390, 207390,
    // Conv 4b
    207390, 207390, 207390, 207390, 
    // Conv 5a
    83390, 83390, 83390, 83390,
    // Conv 5b
    83390, 83390, 83390, 83390,
    // fc6
    21080, 21080,
    // fc 7
    1860, 1860,
    // fc 8
    7130, 7130
};
float func_alexnet[FUNC_COEFFS] = {4, 0, 0}; // TODO fix this eventually
/* End data for Alexnet */

#ifdef REPLAY
	double replay_event_offset[REPLAY_RUNS][LEN_REPLAY] = {
		    { 179000.0, 163000.0, 161000.0, 156000.0, 192000.0, 178000.0, 184000.0, 164000.0, 195000.0, 193000.0, 175000.0, 162000.0, 151000.0, 166000.0, 180000.0, 178000.0, 198000.0, 155000.0, 170000.0, 183000.0},
		    { 191000.0, 157000.0, 198000.0, 186000.0, 200000.0, 178000.0, 197000.0, 166000.0, 155000.0, 184000.0, 153000.0, 173000.0, 181000.0, 165000.0, 168000.0, 179000.0, 164000.0, 188000.0, 182000.0, 196000.0},
		    { 194000.0, 157000.0, 171000.0, 184000.0, 185000.0, 172000.0, 197000.0, 173000.0, 169000.0, 155000.0, 170000.0, 155000.0, 191000.0, 166000.0, 198000.0, 193000.0, 181000.0, 187000.0, 168000.0, 161000.0},
		    { 200000.0, 172000.0, 191000.0, 153000.0, 188000.0, 198000.0, 159000.0, 157000.0, 160000.0, 199000.0, 186000.0, 187000.0, 180000.0, 182000.0, 180000.0, 162000.0, 183000.0, 191000.0, 168000.0, 177000.0},
		    { 153000.0, 183000.0, 181000.0, 186000.0, 165000.0, 179000.0, 158000.0, 170000.0, 163000.0, 198000.0, 178000.0, 198000.0, 182000.0, 188000.0, 179000.0, 151000.0, 192000.0, 165000.0, 172000.0, 168000.0},
		    { 198000.0, 184000.0, 156000.0, 174000.0, 188000.0, 152000.0, 168000.0, 171000.0, 192000.0, 176000.0, 159000.0, 154000.0, 173000.0, 188000.0, 161000.0, 150000.0, 187000.0, 196000.0, 169000.0, 184000.0},
		    { 164000.0, 160000.0, 177000.0, 175000.0, 151000.0, 156000.0, 171000.0, 172000.0, 185000.0, 171000.0, 190000.0, 193000.0, 152000.0, 157000.0, 154000.0, 161000.0, 174000.0, 180000.0, 185000.0, 158000.0},
		    { 167000.0, 199000.0, 160000.0, 192000.0, 198000.0, 182000.0, 153000.0, 150000.0, 151000.0, 189000.0, 165000.0, 174000.0, 167000.0, 178000.0, 160000.0, 194000.0, 185000.0, 189000.0, 190000.0, 175000.0},
		    { 172000.0, 196000.0, 150000.0, 169000.0, 163000.0, 162000.0, 192000.0, 183000.0, 155000.0, 178000.0, 176000.0, 183000.0, 161000.0, 197000.0, 190000.0, 178000.0, 153000.0, 185000.0, 150000.0, 191000.0},
		    { 167000.0, 152000.0, 164000.0, 169000.0, 195000.0, 185000.0, 154000.0, 152000.0, 179000.0, 156000.0, 184000.0, 153000.0, 173000.0, 189000.0, 154000.0, 159000.0, 181000.0, 171000.0, 150000.0, 151000.0}
		};


    int replay_batch[REPLAY_RUNS][LEN_REPLAY] = {
    	    { 20, 20, 20, 20, 1, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
    	    { 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
    	    { 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
    	    { 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
    	    { 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
    	    { 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
    	    { 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
    	    { 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
    	    { 20, 1, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
    	    { 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20}
    	};
    int replay_new_app[REPLAY_RUNS][LEN_REPLAY] = {
    	    { 2, 4, 3, 0, 1, 3, 3, 2, 0, 2, 2, 2, 4, 2, 2, 0, 3, 0, 2, 4},
    	    { 2, 0, 3, 4, 4, 5, 4, 4, 0, 4, 2, 3, 3, 3, 2, 2, 2, 3, 3, 2},
    	    { 5, 2, 0, 0, 3, 0, 0, 2, 3, 3, 2, 4, 0, 4, 2, 0, 2, 2, 2, 4},
    	    { 2, 4, 2, 2, 3, 3, 2, 2, 3, 3, 4, 4, 2, 0, 3, 3, 3, 4, 4, 4},
    	    { 0, 0, 3, 4, 3, 3, 3, 0, 2, 2, 2, 4, 3, 4, 3, 0, 0, 3, 0, 4},
    	    { 3, 2, 2, 0, 0, 3, 2, 4, 2, 2, 3, 0, 2, 3, 0, 4, 3, 2, 3, 2},
    	    { 2, 0, 4, 0, 5, 4, 3, 4, 0, 2, 0, 0, 4, 3, 0, 4, 3, 0, 3, 0},
    	    { 0, 0, 5, 3, 4, 2, 4, 0, 3, 0, 0, 4, 3, 0, 3, 4, 3, 3, 0, 4},
    	    { 4, 1, 5, 4, 2, 4, 3, 0, 0, 3, 3, 2, 0, 2, 4, 2, 4, 0, 3, 3},
    	    { 3, 2, 4, 4, 0, 5, 4, 3, 2, 3, 0, 3, 4, 4, 0, 3, 3, 3, 4, 4}
    	};
    int replay_priority_level[REPLAY_RUNS][LEN_REPLAY] = {
    	    { 3, 1, 3, 9, 1, 9, 3, 3, 1, 9, 9, 1, 3, 1, 9, 3, 1, 1, 3, 1},
    	    { 9, 1, 1, 9, 1, 1, 1, 9, 1, 3, 3, 9, 3, 1, 3, 3, 9, 9, 9, 3},
    	    { 9, 3, 9, 3, 1, 3, 1, 9, 1, 9, 9, 9, 3, 1, 1, 3, 1, 3, 3, 1},
    	    { 9, 1, 3, 1, 9, 9, 9, 3, 9, 9, 9, 3, 1, 9, 1, 9, 9, 3, 3, 1},
    	    { 9, 3, 3, 9, 1, 9, 9, 1, 9, 3, 1, 1, 9, 1, 9, 9, 9, 9, 1, 1},
    	    { 9, 3, 3, 1, 9, 3, 3, 1, 1, 9, 3, 9, 3, 1, 3, 1, 1, 1, 3, 3},
    	    { 1, 9, 1, 3, 1, 3, 1, 3, 3, 1, 3, 1, 9, 9, 9, 3, 1, 9, 9, 9},
    	    { 9, 1, 1, 1, 9, 3, 3, 3, 9, 3, 1, 9, 3, 3, 9, 1, 9, 9, 3, 1},
    	    { 1, 3, 9, 1, 1, 9, 1, 9, 9, 9, 9, 1, 3, 9, 9, 1, 9, 1, 3, 9},
    	    { 3, 9, 3, 9, 1, 9, 3, 1, 3, 9, 1, 1, 9, 3, 1, 3, 9, 1, 3, 3}
    	};
#endif
/*
 * Return the new random event that needs to be selected at this timestep
 */
int pick_new_app() {
    return rand() % NUM_APPS;
}

/*
 * Return a random batch size for the new event
 */
int generate_batch_size(int max_batch) {
    return (rand() % max_batch) + 1;
}

int generate_priority_level() {
    std::vector<int> l{1, 3, 9};
    int index = rand() % l.size();
    return l[index];
}

/*
 * Returns offset to next event arrival
 * offset is generated in ms and returned in us
 */
double next_event_delay(int min, int max) {
    int off_ms = rand()%max + min;
    return 1.0*off_ms*1000.0; // translate to us
}

char* app_names[6] = {"Lenet", "KNN", "Optical Flow", "Image Compression","3DR", "Alexnet"};
int parallelism[6] = {3, 1, 3, 3, 2, 4};
#ifdef REPLAY
double next_event_delay_replay(int i, int j) {
    return replay_event_offset[j][i];
}

int calculate_slots(float coeffs[FUNC_COEFFS], int batch) {
    float val = 0;
    val += coeffs[0];
    val += coeffs[1]*1.0*batch;
    val += coeffs[2]*1.0*batch*batch;
    return static_cast<int>(::round(val));
}

Application* add_new_event_replay(std::list< Application*>& pending_applications, std::list<Task*>& pending_tasks, int i, int j) {
    int new_app = replay_new_app[j][i];
    int batch_size = replay_batch[j][i];
    int p_level = replay_priority_level[j][i];
    Application* my_app = new Application();
    XTime_GetTime(&(my_app->arrival_time));
    my_app->priority = p_level;
    my_app->latency = 0;
    my_app->priority_level = p_level;
    my_app->id = new_app;
    my_app->parallelism = parallelism[new_app];
    my_app->occupancy = 0;
    unsigned long priority;
    #ifdef NIMBLOCK
    priority = p_level;
    #else
    priority = my_app->arrival_time;
    #endif
    switch (new_app) {
        case 0: /* Lenet */
//        	batch_size = generate_batch_size(MAX_BATCH_LENET);
            for (int i = 0; i < NUM_IP_LENET; i++) {
                Task* my_task = new Task(i, my_app, priority, batch_size, bin_names_lenet[i], 0, num_arg_lenet[i], arg_size_lenet[i]);
                my_task->latency = lat_lenet[i];
                // Add task to application
                my_app->tasks.push_back(my_task);
                my_app->dependency_list.push_back(dep_list_lenet[i]);
                my_app->num_deps.push_back(num_dep_lenet[i]);
                my_app->parallelism = std::min(calculate_slots(func_lenet, batch_size), NUM_IP_LENET);
                // Add task to pending tasks
                pending_tasks.push_back(my_task);
            }
            break;
        case 1: /* KNN */
//        	batch_size = generate_batch_size(MAX_BATCH_KNN);
            for (int i = 0; i < NUM_IP_KNN; i++) {
                Task* my_task = new Task(i, my_app, priority, batch_size, bin_names_knn[i], 0, num_arg_knn[i], arg_size_knn[i]);
                my_task->latency = lat_knn[i];
                // Add task to application
                my_app->tasks.push_back(my_task);
                my_app->dependency_list.push_back(dep_list_knn[i]);
                my_app->num_deps.push_back(num_dep_knn[i]);
                my_app->parallelism = std::min(NUM_IP_KNN, calculate_slots(func_knn, batch_size));
                // Add task to pending tasks
                pending_tasks.push_back(my_task);
            }
            break;
        case 2: /* Optical Flow */
//        	batch_size = generate_batch_size(MAX_BATCH_OPTICAL_FLOW);
            for (int i = 0; i < NUM_IP_OPTICAL_FLOW; i++) {
                Task* my_task = new Task(i, my_app, priority, batch_size, bin_names_opt_flow[i], 0, num_arg_opt_flow[i], arg_size_opt_flow[i]);
                my_task->latency = lat_opt_flow[i];
                // Add task to application
                my_app->tasks.push_back(my_task);
                my_app->dependency_list.push_back(dep_list_opt_flow[i]);
                my_app->num_deps.push_back(num_dep_opt_flow[i]);
                my_app->parallelism = std::min(NUM_IP_OPTICAL_FLOW, calculate_slots(func_opt_flow, batch_size));
                // Add task to pending tasks
                pending_tasks.push_back(my_task);
            }
            break;
        case 3: /* Image Compression */
//        	batch_size = generate_batch_size(MAX_BATCH_IMG_CMP);
            for (int i = 0; i < NUM_IP_IMG_CMP; i++) {
                Task* my_task = new Task(i, my_app, priority, batch_size, bin_names_img_cmp[i], 0, num_arg_img_cmp[i], arg_size_img_cmp[i]);
                my_task->latency = lat_img_cmp[i];
                // Add task to application
                my_app->tasks.push_back(my_task);
                my_app->dependency_list.push_back(dep_list_img_cmp[i]);
                my_app->num_deps.push_back(num_dep_img_cmp[i]);
                if (i == 0) {
					my_task->const_values.push_back(25);
					my_task->const_values.push_back(10);
					my_task->const_values.push_back(16);
					my_task->const_values.push_back(32);
                } else {
					my_task->const_values.push_back(16);
					my_task->const_values.push_back(32);
                }
                my_app->parallelism = std::min(NUM_IP_IMG_CMP, calculate_slots(func_img_cmp, batch_size));
                // Add task to pending tasks
                pending_tasks.push_back(my_task);
            }
            break;
        case 4: /* 3DR */
//        	batch_size = generate_batch_size(MAX_BATCH_3DR);
            for (int i = 0; i < NUM_IP_3DR; i++) {
                Task* my_task = new Task(i, my_app, priority, batch_size, bin_names_3dr[i], 0, num_arg_3dr[i], arg_size_3dr[i]);
                my_task->latency = lat_3dr[i];
                my_task->arg_protocols = linked_args_3dr[i];
                my_task->arg_locations = arg_locations_3dr[i];
                // Add task to application
                my_app->tasks.push_back(my_task);
                my_app->dependency_list.push_back(dep_list_3dr[i]);
                my_app->num_deps.push_back(num_dep_3dr[i]);
                my_app->parallelism = std::min(NUM_IP_3DR, calculate_slots(func_3dr, batch_size));
                // Add task to pending tasks
                pending_tasks.push_back(my_task);
            }
            break;
        case 5: /* Alexnet, not implemented yet */
//        	batch_size = generate_batch_size(MAX_BATCH_ALEXNET);/
            for (int i = 0; i < NUM_IP_ALEXNET; i++) {
                Task* my_task = new Task(i, my_app, priority, batch_size, bin_names_alexnet[i], 0, num_arg_alexnet[i], arg_size_alexnet[i]);
                my_task->latency = lat_alexnet[i];
                // Add task to application
                my_app->tasks.push_back(my_task);
                my_app->dependency_list.push_back(dep_list_alexnet[i]);
                my_app->num_deps.push_back(num_dep_alexnet[i]);
                // Const values
                int k = i % 4;
                int k_2 = i % 2; 
                switch (i) {
                    case 0: case 1: case 2: case 3:
                        my_task->const_values.push_back(k*ALEXNET_CONV1_OUTPUT/ALEXNET_CONV1_SLOTS);
                        break;
                    case 4: case 5: case 6: case 7:
                        my_task->const_values.push_back(k*ALEXNET_CONV2_OUTPUT/ALEXNET_CONV2_SLOTS);
                        my_task->const_values.push_back(0);
                        break;
                    case 8: case 9: case 10: case 11:
                        my_task->const_values.push_back((k*ALEXNET_CONV2_OUTPUT/ALEXNET_CONV2_SLOTS)+128);
                        my_task->const_values.push_back(48);
                        break;
                    case 12: case 13: case 14: case 15:
                        my_task->const_values.push_back(k*ALEXNET_CONV3_OUTPUT/ALEXNET_CONV3_SLOTS);
                        break;
                    case 16: case 17: case 18: case 19:
                        my_task->const_values.push_back(k*ALEXNET_CONV4_OUTPUT/ALEXNET_CONV4_SLOTS);
                        my_task->const_values.push_back(0);
                        break;
                    case 20: case 21: case 22: case 23:
                        my_task->const_values.push_back(k*ALEXNET_CONV4_OUTPUT/ALEXNET_CONV4_SLOTS);
                        my_task->const_values.push_back(192);
                        break;
                    case 24: case 25: case 26: case 27:
                        my_task->const_values.push_back((k*ALEXNET_CONV5_OUTPUT/ALEXNET_CONV5_SLOTS));
                        my_task->const_values.push_back(0);
                        break;
                    case 28: case 29: case 30: case 31:
                        my_task->const_values.push_back((k*ALEXNET_CONV5_OUTPUT/ALEXNET_CONV5_SLOTS)+128);
                        my_task->const_values.push_back(192);
                        break;
                    case 32: case 33:
                        my_task->const_values.push_back(k_2*ALEXNET_FC6_OUTPUT/ALEXNET_FC6_SLOTS);
                        break;
                    case 34: case 35:
                        my_task->const_values.push_back(k_2*ALEXNET_FC7_OUTPUT/ALEXNET_FC7_SLOTS);
                        break;
                    case 36: case 37:
                        my_task->const_values.push_back(k_2*ALEXNET_FC8_OUTPUT/ALEXNET_FC8_SLOTS);
                        break;
                    default:
                        return NULL;
                        break;
                }
                my_app->parallelism = std::min(NUM_IP_ALEXNET, calculate_slots(func_alexnet, batch_size));
                // Add task to pending tasks
                pending_tasks.push_back(my_task);
            }
            break;
        default:
            delete my_app;
            return NULL;
    }
    // Add application to pending applications
    my_app->batch = batch_size;
    my_app->app_name = app_names[new_app];
    my_app->parallelism = parallelism[new_app]; // TODO only temporary, remove later
    pending_applications.push_back(my_app);
    printf("Slot goal is for %s with batch size %d is %d\n\r", my_app->app_name, my_app->batch, my_app->parallelism);
    return my_app;
}

#endif



/*
 * Adds a new event to the pending application queue and its tasks to the pending_task_queue
 */
Application* add_new_event(std::list< Application*>& pending_applications, std::list<Task*>& pending_tasks) {
    int new_app = pick_new_app();
    int batch_size;
    int p_level = generate_priority_level();
    Application* my_app = new Application();
    XTime_GetTime(&(my_app->arrival_time));
    my_app->priority = p_level;
    my_app->priority_level = p_level;
    my_app->latency = 0;
    my_app->id = new_app;
    my_app->occupancy = 0;
    my_app->parallelism = parallelism[new_app];
    unsigned long priority;
    #ifdef NIMBLOCK
    priority = p_level;
    #else
    priority = my_app->arrival_time;
    #endif
    switch (new_app) {
        case 0: /* Lenet */
        	batch_size = generate_batch_size(MAX_BATCH_LENET);
            for (int i = 0; i < NUM_IP_LENET; i++) {
                Task* my_task = new Task(i, my_app, priority, batch_size, bin_names_lenet[i], 0, num_arg_lenet[i], arg_size_lenet[i]);
                my_task->latency = lat_lenet[i];
                // Add task to application
                my_app->tasks.push_back(my_task);
                my_app->dependency_list.push_back(dep_list_lenet[i]);
                my_app->num_deps.push_back(num_dep_lenet[i]);
                my_app->parallelism = calculate_slots(func_lenet, batch_size);
                // Add task to pending tasks
                pending_tasks.push_back(my_task);
            }
            break;
        case 1: /* KNN */
        	batch_size = generate_batch_size(MAX_BATCH_KNN);
            for (int i = 0; i < NUM_IP_KNN; i++) {
                Task* my_task = new Task(i, my_app, priority, batch_size, bin_names_knn[i], 0, num_arg_knn[i], arg_size_knn[i]);
                my_task->latency = lat_knn[i];
                // Add task to application
                my_app->tasks.push_back(my_task);
                my_app->dependency_list.push_back(dep_list_knn[i]);
                my_app->num_deps.push_back(num_dep_knn[i]);
                my_app->parallelism = calculate_slots(func_knn, batch_size);
                // Add task to pending tasks
                pending_tasks.push_back(my_task);
            }
            break;
        case 2: /* Optical Flow */
        	batch_size = generate_batch_size(MAX_BATCH_OPTICAL_FLOW);
            for (int i = 0; i < NUM_IP_OPTICAL_FLOW; i++) {
                Task* my_task = new Task(i, my_app, priority, batch_size, bin_names_opt_flow[i], 0, num_arg_opt_flow[i], arg_size_opt_flow[i]);
                my_task->latency = lat_opt_flow[i];
                // Add task to application
                my_app->tasks.push_back(my_task);
                my_app->dependency_list.push_back(dep_list_opt_flow[i]);
                my_app->num_deps.push_back(num_dep_opt_flow[i]);
                my_app->parallelism = calculate_slots(func_opt_flow, batch_size);
                // Add task to pending tasks
                pending_tasks.push_back(my_task);
            }
            break;
        case 3: /* Image Compression */
        	batch_size = generate_batch_size(MAX_BATCH_IMG_CMP);
            for (int i = 0; i < NUM_IP_IMG_CMP; i++) {
                Task* my_task = new Task(i, my_app, priority, batch_size, bin_names_img_cmp[i], 0, num_arg_img_cmp[i], arg_size_img_cmp[i]);
                my_task->latency = lat_img_cmp[i];
                // Add task to application
                my_app->tasks.push_back(my_task);
                my_app->dependency_list.push_back(dep_list_img_cmp[i]);
                my_app->num_deps.push_back(num_dep_img_cmp[i]);
                if (i == 0) {
					my_task->const_values.push_back(25);
					my_task->const_values.push_back(10);
					my_task->const_values.push_back(16);
					my_task->const_values.push_back(32);
                } else {
					my_task->const_values.push_back(16);
					my_task->const_values.push_back(32);
                }
                my_app->parallelism = calculate_slots(func_img_cmp, batch_size);
                // Add task to pending tasks
                pending_tasks.push_back(my_task);
            }
            break;
        case 4: /* 3DR */
        	batch_size = generate_batch_size(MAX_BATCH_3DR);
            for (int i = 0; i < NUM_IP_3DR; i++) {
                Task* my_task = new Task(i, my_app, priority, batch_size, bin_names_3dr[i], 0, num_arg_3dr[i], arg_size_3dr[i]);
                my_task->latency = lat_3dr[i];
                my_task->arg_protocols = linked_args_3dr[i];
                my_task->arg_locations = arg_locations_3dr[i];
                // Add task to application
                my_app->tasks.push_back(my_task);
                my_app->dependency_list.push_back(dep_list_3dr[i]);
                my_app->num_deps.push_back(num_dep_3dr[i]);
                my_app->parallelism = calculate_slots(func_3dr, batch_size);
                // Add task to pending tasks
                pending_tasks.push_back(my_task);
            }
            break;
        case 5: /* Alexnet */
        	batch_size = generate_batch_size(MAX_BATCH_ALEXNET);
            for (int i = 0; i < NUM_IP_ALEXNET; i++) {
                Task* my_task = new Task(i, my_app, priority, batch_size, bin_names_alexnet[i], 0, num_arg_alexnet[i], arg_size_alexnet[i]);
                my_task->latency = lat_alexnet[i];
                // Add task to application
                my_app->tasks.push_back(my_task);
                my_app->dependency_list.push_back(dep_list_alexnet[i]);
                my_app->num_deps.push_back(num_dep_alexnet[i]);
                // Const values
                int k = i % 4;
                int k_2 = i % 2; 
                switch (i) {
                    case 0: case 1: case 2: case 3:
                        my_task->const_values.push_back(k*ALEXNET_CONV1_OUTPUT/ALEXNET_CONV1_SLOTS);
                        break;
                    case 4: case 5: case 6: case 7:
                        my_task->const_values.push_back(k*ALEXNET_CONV2_OUTPUT/ALEXNET_CONV2_SLOTS);
                        my_task->const_values.push_back(0);
                        break;
                    case 8: case 9: case 10: case 11:
                        my_task->const_values.push_back((k*ALEXNET_CONV2_OUTPUT/ALEXNET_CONV2_SLOTS)+128);
                        my_task->const_values.push_back(48);
                        break;
                    case 12: case 13: case 14: case 15:
                        my_task->const_values.push_back(k*ALEXNET_CONV3_OUTPUT/ALEXNET_CONV3_SLOTS);
                        break;
                    case 16: case 17: case 18: case 19:
                        my_task->const_values.push_back(k*ALEXNET_CONV4_OUTPUT/ALEXNET_CONV4_SLOTS);
                        my_task->const_values.push_back(0);
                        break;
                    case 20: case 21: case 22: case 23:
                        my_task->const_values.push_back(k*ALEXNET_CONV4_OUTPUT/ALEXNET_CONV4_SLOTS);
                        my_task->const_values.push_back(192);
                        break;
                    case 24: case 25: case 26: case 27:
                        my_task->const_values.push_back((k*ALEXNET_CONV5_OUTPUT/ALEXNET_CONV5_SLOTS));
                        my_task->const_values.push_back(0);
                        break;
                    case 28: case 29: case 30: case 31:
                        my_task->const_values.push_back((k*ALEXNET_CONV5_OUTPUT/ALEXNET_CONV5_SLOTS)+128);
                        my_task->const_values.push_back(192);
                        break;
                    case 32: case 33:
                        my_task->const_values.push_back(k_2*ALEXNET_FC6_OUTPUT/ALEXNET_FC6_SLOTS);
                        break;
                    case 34: case 35:
                        my_task->const_values.push_back(k_2*ALEXNET_FC7_OUTPUT/ALEXNET_FC7_SLOTS);
                        break;
                    case 36: case 37:
                        my_task->const_values.push_back(k_2*ALEXNET_FC8_OUTPUT/ALEXNET_FC8_SLOTS);
                        break;
                    default:
                        return NULL;
                        break;
                }
                my_app->parallelism = calculate_slots(func_alexnet, batch_size);
                // Add task to pending tasks
                pending_tasks.push_back(my_task);
            }
            break;
        default:
            delete my_app;
            return NULL;
    }
    // Add application to pending applications
    my_app->batch = batch_size;
    my_app->app_name = app_names[new_app];
    pending_applications.push_back(my_app);
    return my_app;
}
