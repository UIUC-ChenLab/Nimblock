#include "scheduler.h"


// Reserving DRAM for the partial bitstreams
char APP0_CONFIG0[PARTIAL_BITFILE_ALLOCATED_SPACE]  __attribute__ ((aligned(8)));
char APP0_CONFIG1[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP0_CONFIG2[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP1_CONFIG0[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP1_CONFIG1[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP1_CONFIG2[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP2_CONFIG0[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP2_CONFIG1[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP2_CONFIG2[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP3_CONFIG0[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP3_CONFIG1[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP3_CONFIG2[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP3_CONFIG3[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP3_CONFIG4[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP3_CONFIG5[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP4_CONFIG0[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP4_CONFIG1[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP4_CONFIG2[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP4_CONFIG3[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP4_CONFIG4[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP4_CONFIG5[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP4_CONFIG6[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP4_CONFIG7[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP4_CONFIG8[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));

char APP5_CONFIG0[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG1[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG2[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG3[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG4[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG5[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG6[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG7[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG8[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG9[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG10[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG11[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG12[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG13[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG14[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG15[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG16[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG17[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG18[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG19[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG20[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG21[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG22[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG23[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG24[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG25[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG26[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG27[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG28[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG29[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG30[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG31[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG32[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG33[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG34[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG35[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG36[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));
char APP5_CONFIG37[PARTIAL_BITFILE_ALLOCATED_SPACE] __attribute__ ((aligned(8)));



// static status objects for schedule
extern struct slot slots[NUM_SLOTS];
extern struct ip ip_list[NUM_APPLICATIONS][MAX_NUM_IP];

int decouple_gpio[NUM_SLOTS] = {78, 79, 80, 81, 82, 83, 84, 85, 86, 87};
int reset_gpio[NUM_SLOTS] = {88, 89, 90, 91, 92, 93, 94, 95, 96, 97};
u32 slot_baseaddr[NUM_SLOTS] = {
		XPAR_RECONFIG_BLOCK_BLANK_0_S_AXI_CTRL_BASEADDR,
		XPAR_RECONFIG_BLOCK_BLANK_1_S_AXI_CTRL_BASEADDR,
		XPAR_RECONFIG_BLOCK_BLANK_2_S_AXI_CTRL_BASEADDR,
		XPAR_RECONFIG_BLOCK_BLANK_3_S_AXI_CTRL_BASEADDR,
		XPAR_RECONFIG_BLOCK_BLANK_4_S_AXI_CTRL_BASEADDR,
		XPAR_RECONFIG_BLOCK_BLANK_5_S_AXI_CTRL_BASEADDR,
		XPAR_RECONFIG_BLOCK_BLANK_6_S_AXI_CTRL_BASEADDR,
		XPAR_RECONFIG_BLOCK_BLANK_7_S_AXI_CTRL_BASEADDR,
		XPAR_RECONFIG_BLOCK_BLANK_8_S_AXI_CTRL_BASEADDR,
		XPAR_RECONFIG_BLOCK_BLANK_9_S_AXI_CTRL_BASEADDR,

};


u32 bin_len[NUM_SLOTS] = {
		PARTIAL_BITFILE_LEN_0,
		PARTIAL_BITFILE_LEN_1,
		PARTIAL_BITFILE_LEN_2,
		PARTIAL_BITFILE_LEN_3,
		PARTIAL_BITFILE_LEN_4,
		PARTIAL_BITFILE_LEN_5,
		PARTIAL_BITFILE_LEN_6,
		PARTIAL_BITFILE_LEN_7,
		PARTIAL_BITFILE_LEN_8,
		PARTIAL_BITFILE_LEN_9,
};




// Defining 3DR schedule and information
#define NUM_IP_3DR 3
int kern_mapping_3dr[NUM_IP_3DR] = {0, 1, 2};
int slot_mapping_3dr[NUM_IP_3DR] = {9, 8, 9};
int num_batch_3dr[NUM_IP_3DR] = {BATCH_SIZE_3DR, BATCH_SIZE_3DR, BATCH_SIZE_3DR};
int batch_offset_3dr[NUM_IP_3DR] = {0, 0, 0};
int num_dep_3dr[NUM_IP_3DR] = {0, 1, 1};
int dep_list_3dr[NUM_IP_3DR][NUM_IP_3DR] = {
	{-1, -1, -1},
	{0, -1, -1},
	{1, -1, -1}
};
int pr_times_3dr[NUM_IP_3DR] = {
	0,
	0,
	0,
};
int run_times_3dr[NUM_IP_3DR] = {
	0,
	0,
	0,
};
char *bin_names_3dr[NUM_IP_3DR][NUM_SLOTS] = {
	{"s0c31.bin", "s1c31.bin", "s2c31.bin", "s3c31.bin", "s4c31.bin", "s5c31.bin", "s6c31.bin", "s7c31.bin", "s8c31.bin", "s9c31.bin"},
	{"s0c0.bin", "s1c0.bin", "s2c0.bin", "s3c0.bin", "s4c0.bin", "s5c0.bin", "s6c0.bin", "s7c0.bin", "s8c0.bin", "s9c0.bin"},
	{"s0c2.bin", "s1c2.bin", "s2c2.bin", "s3c2.bin", "s4c2.bin", "s5c2.bin", "s6c2.bin", "s7c2.bin", "s8c2.bin", "s9c2.bin"},
};
char* pr_adders_3dr[NUM_IP_3DR] = {
	APP0_CONFIG0,
	APP0_CONFIG1,
	APP0_CONFIG2,
};

// Defining Lenet schedule and information
#define NUM_IP_LENET 3
int kern_mapping_lenet[NUM_IP_LENET] = {0, 1, 2};
int slot_mapping_lenet[NUM_IP_LENET] = {6,7,6,};
int num_batch_lenet[NUM_IP_LENET] = {BATCH_SIZE_LENET, BATCH_SIZE_LENET, BATCH_SIZE_LENET};
int batch_offset_lenet[NUM_IP_LENET] = {0, 0, 0};
int num_dep_lenet[NUM_IP_LENET] = {0, 1, 1};
int dep_list_lenet[NUM_IP_LENET][NUM_IP_LENET] = {
	{-1, -1, -1},
	{0, -1, -1},
	{1, -1, -1}
};
int pr_times_lenet[NUM_IP_LENET] = {
	0,
	0,
	0,
};
int run_times_lenet[NUM_IP_LENET] = {
	0,
	0,
	0,
};
char *bin_names_lenet[NUM_IP_LENET][NUM_SLOTS] = {
	{"s0c12.bin", "s1c12.bin", "s2c12.bin", "s3c12.bin", "s4c12.bin", "s5c12.bin", "s6c12.bin", "s7c12.bin", "s8c12.bin", "s9c12.bin"},
	{"s0c13.bin", "s1c13.bin", "s2c13.bin", "s3c13.bin", "s4c13.bin", "s5c13.bin", "s6c13.bin", "s7c13.bin", "s8c13.bin", "s9c13.bin"},
	{"s0c3.bin", "s1c3.bin", "s2c3.bin", "s3c3.bin", "s4c3.bin", "s5c3.bin", "s6c3.bin", "s7c3.bin", "s8c3.bin", "s9c3.bin"},
};
char* pr_adders_lenet[NUM_IP_LENET] = {
	APP1_CONFIG0,
	APP1_CONFIG1,
	APP1_CONFIG2,
};


// Defining knn schedule and information
#define NUM_IP_KNN 3
int kern_mapping_knn[NUM_IP_KNN] = {0, 1, 2};
int slot_mapping_knn[NUM_IP_KNN] = {7, 2, 1};
int num_batch_knn[NUM_IP_KNN] = {BATCH_SIZE_KNN, BATCH_SIZE_KNN, BATCH_SIZE_KNN};
int batch_offset_knn[NUM_IP_KNN] = {0, 0, 0};
int num_dep_knn[NUM_IP_KNN] = {0, 1, 1};
int dep_list_knn[NUM_IP_KNN][NUM_IP_KNN] = {
	{-1, -1, -1},
	{0, -1, -1},
	{1, -1, -1}
};
int pr_times_knn[NUM_IP_KNN] = {
	0,
	0,
	0,
};
int run_times_knn[NUM_IP_KNN] = {
	0,
	0,
	0,
};
char *bin_names_knn[NUM_IP_KNN][NUM_SLOTS] = {
	{"s0c19.bin", "s1c19.bin", "s2c19.bin", "s3c19.bin", "s4c19.bin", "s5c19.bin", "s6c19.bin", "s7c19.bin", "s8c19.bin", "s9c19.bin"},
	{"s0c20.bin", "s1c20.bin", "s2c20.bin", "s3c20.bin", "s4c20.bin", "s5c20.bin", "s6c20.bin", "s7c20.bin", "s8c20.bin", "s9c20.bin"},
	{"s0c21.bin", "s1c21.bin", "s2c21.bin", "s3c21.bin", "s4c21.bin", "s5c21.bin", "s6c21.bin", "s7c21.bin", "s8c21.bin", "s9c21.bin"},
};
char* pr_adders_knn[NUM_IP_KNN] = {
	APP2_CONFIG0,
	APP2_CONFIG1,
	APP2_CONFIG2,
};

// Defining image compression schedule and information
#define NUM_IP_IMG_CMP 6
int kern_mapping_img_cmp[NUM_IP_IMG_CMP] = {0, 1, 2, 3, 4, 5};
int slot_mapping_img_cmp[NUM_IP_IMG_CMP] = {4,0,2,3,9,5,};
int num_batch_img_cmp[NUM_IP_IMG_CMP] = {BATCH_SIZE_IMG_CMP, BATCH_SIZE_IMG_CMP, BATCH_SIZE_IMG_CMP, BATCH_SIZE_IMG_CMP, BATCH_SIZE_IMG_CMP, BATCH_SIZE_IMG_CMP};
int batch_offset_img_cmp[NUM_IP_IMG_CMP] = {0, 0, 0, 0, 0, 0};
int num_dep_img_cmp[NUM_IP_IMG_CMP] = {0, 1, 1, 1, 1, 1};
int dep_list_img_cmp[NUM_IP_IMG_CMP][NUM_IP_IMG_CMP] = {
	{-1, -1, -1, -1, -1, -1},
	{0, -1, -1, -1, -1, -1},
	{1, -1, -1, -1, -1, -1},
	{2, -1, -1, -1, -1, -1},
	{3, -1, -1, -1, -1, -1},
	{4, -1, -1, -1, -1, -1}
};
int pr_times_img_cmp[NUM_IP_IMG_CMP] = {
	0,
	0,
	0,
	0,
	0,
	0
};
int run_times_img_cmp[NUM_IP_IMG_CMP] = {
	0,
	0,
	0,
	0,
	0,
	0
};
char *bin_names_img_cmp[NUM_IP_IMG_CMP][NUM_SLOTS] = {
	{"s0c4.bin", "s1c4.bin", "s2c4.bin", "s3c4.bin", "s4c4.bin", "s5c4.bin", "s6c4.bin", "s7c4.bin", "s8c4.bin", "s9c4.bin"},
	{"s0c14.bin", "s1c14.bin", "s2c14.bin", "s3c14.bin", "s4c14.bin", "s5c14.bin", "s6c14.bin", "s7c14.bin", "s8c14.bin", "s9c14.bin"},
	{"s0c15.bin", "s1c15.bin", "s2c15.bin", "s3c15.bin", "s4c15.bin", "s5c15.bin", "s6c15.bin", "s7c15.bin", "s8c15.bin", "s9c15.bin"},
	{"s0c16.bin", "s1c16.bin", "s2c16.bin", "s3c16.bin", "s4c16.bin", "s5c16.bin", "s6c16.bin", "s7c16.bin", "s8c16.bin", "s9c16.bin"},
	{"s0c17.bin", "s1c17.bin", "s2c17.bin", "s3c17.bin", "s4c17.bin", "s5c17.bin", "s6c17.bin", "s7c17.bin", "s8c17.bin", "s9c17.bin"},
	{"s0c18.bin", "s1c18.bin", "s2c18.bin", "s3c18.bin", "s4c18.bin", "s5c18.bin", "s6c18.bin", "s7c18.bin","s8c18.bin", "s9c18.bin"},
};
char* pr_adders_img_cmp[NUM_IP_IMG_CMP] = {
	APP3_CONFIG0,
	APP3_CONFIG1,
	APP3_CONFIG2,
	APP3_CONFIG3,
	APP3_CONFIG4,
	APP3_CONFIG5,
};

// Defining optical flow schedule and information
#define NUM_IP_OPTICAL_FLOW 9
int kern_mapping_opt_flow[NUM_IP_OPTICAL_FLOW] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
int slot_mapping_opt_flow[NUM_IP_OPTICAL_FLOW] = {1,3,4,5,0,2,1,3,4,};
int num_batch_opt_flow[NUM_IP_OPTICAL_FLOW] = {BATCH_SIZE_OPTICAL_FLOW, BATCH_SIZE_OPTICAL_FLOW, BATCH_SIZE_OPTICAL_FLOW, BATCH_SIZE_OPTICAL_FLOW, BATCH_SIZE_OPTICAL_FLOW, BATCH_SIZE_OPTICAL_FLOW, BATCH_SIZE_OPTICAL_FLOW, BATCH_SIZE_OPTICAL_FLOW, BATCH_SIZE_OPTICAL_FLOW};
int batch_offset_opt_flow[NUM_IP_OPTICAL_FLOW] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
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
int pr_times_opt_flow[NUM_IP_OPTICAL_FLOW] = {
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
};
int run_times_opt_flow[NUM_IP_OPTICAL_FLOW] = {
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
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
char* pr_adders_opt_flow[NUM_IP_OPTICAL_FLOW] = {
	APP4_CONFIG0,
	APP4_CONFIG1,
	APP4_CONFIG2,
	APP4_CONFIG3,
	APP4_CONFIG4,
	APP4_CONFIG5,
	APP4_CONFIG6,
	APP4_CONFIG7,
	APP4_CONFIG8,
};


// Defining alexnet schedule and information
#define NUM_IP_ALEXNET 38
//int kern_mapping_alexnet[NUM_IP_ALEXNET] = {1,0,3,2,5,9,7,8,6,10,4,11,13,12,15,14,17,16,18,19,22,21,20,23,26,27,29,30,24,31,25,28,33,32,34,35,37,36,};
//int slot_mapping_alexnet[NUM_IP_ALEXNET] = {3,2,4,0,5,1,3,2,4,0,0,4,5,2,1,3,0,4,5,2,3,0,1,2,5,4,3,0,1,2,1,0,4,2,3,5,1,0,};

int kern_mapping_alexnet[NUM_IP_ALEXNET] = {
		2,		// layer 1_2
		1,		// layer 1_1
		0,		// layer 1_0
		3,		// layer 1_3
		8,		// layer 2b_0
		6,		// layer 2a_2
		9,		// layer 2b_1
		7,		// layer 2a_3
		10,		// layer 2b_2
		11,		// layer 2b_3
		5,		// layer 2a_1
		4,		// layer 2a_0
		12,		// layer 3_0
		14,		// layer 3_2
		15,		// layer 3_3
		13,		// layer 3_1
		19,		// layer 4a_3
		18,		// layer 4a_2
		16,		// layer 4a_0
		17,		// layer 4a_1
		23,		// layer 4b_3
		21,		// layer 4b_1
		20,		// layer 4b_0
		22,		// layer 4b_2
		29,		// layer 5b_1
		31,		// layer 5b_3
		24,		// layer 5a_0
		30,		// layer 5b_2
		26,		// layer 5a_2
		27,		// layer 5a_3
		25,		// layer 5a_1
		28,		// layer 5b_0
		33,		// layer 6_1
		32,		// layer 6_0
		35,		// layer 7_1
		34,		// layer 7_0
		37,		// layer 8_1
		36		// layer 8_0

};

int slot_mapping_alexnet[NUM_IP_ALEXNET] = {
		8,		// layer 1_0
		3,		// layer 1_1
		4,		// layer 1_2
		9,		// layer 1_3
		5,		// layer 2a_0
		2,		// layer 2a_1
		0,		// layer 2a_2
		7,		// layer 2a_3
		6,		// layer 2b_0
		1,		// layer 2b_1
		9,		// layer 2b_2
		8,		// layer 2b_3
		2,		// layer 3_2
		7,		// layer 3_0
		4,		// layer 3_3
		0,		// layer 3_1
		5,		// layer 4a_3
		9,		// layer 4a_2
		6,		// layer 4a_1
		3,		// layer 4a_0
		2,		// layer 4b_3
		0,		// layer 4b_0
		1,		// layer 4b_1
		3,		// layer 4b_2
		4,		// layer 5b_1
		5,		// layer 5b_2
		7,		// layer 5a_0
		9,		// layer 5a_2
		6,		// layer 5b_3
		8,		// layer 5a_3
		2,		// layer 5b_0
		0,		// layer 5a_1
		3,		// layer 6_0
		7,		// layer 6_1
		5,		// layer 7_1
		8,		// layer 7_0
		4,		// layer 8_0
		3		// layer 8_1
};
int num_batch_alexnet[NUM_IP_ALEXNET] = {
		BATCH_SIZE_ALEXNET,		// layer 1_0
		BATCH_SIZE_ALEXNET,		// layer 1_1
		BATCH_SIZE_ALEXNET,		// layer 1_2
		BATCH_SIZE_ALEXNET,		// layer 1_3
		BATCH_SIZE_ALEXNET,		// layer 2a_0
		BATCH_SIZE_ALEXNET,		// layer 2a_1
		BATCH_SIZE_ALEXNET,		// layer 2a_2
		BATCH_SIZE_ALEXNET,		// layer 2a_3
		BATCH_SIZE_ALEXNET,		// layer 2b_0
		BATCH_SIZE_ALEXNET,		// layer 2b_1
		BATCH_SIZE_ALEXNET,		// layer 2b_2
		BATCH_SIZE_ALEXNET,		// layer 2b_3
		BATCH_SIZE_ALEXNET,		// layer 3_0
		BATCH_SIZE_ALEXNET,		// layer 3_1
		BATCH_SIZE_ALEXNET,		// layer 3_2
		BATCH_SIZE_ALEXNET,		// layer 3_3
		BATCH_SIZE_ALEXNET,		// layer 4a_0
		BATCH_SIZE_ALEXNET,		// layer 4a_1
		BATCH_SIZE_ALEXNET,		// layer 4a_2
		BATCH_SIZE_ALEXNET,		// layer 4a_3
		BATCH_SIZE_ALEXNET,		// layer 4b_0
		BATCH_SIZE_ALEXNET,		// layer 4b_1
		BATCH_SIZE_ALEXNET,		// layer 4b_2
		BATCH_SIZE_ALEXNET,		// layer 4b_3
		BATCH_SIZE_ALEXNET,		// layer 5a_0
		BATCH_SIZE_ALEXNET,		// layer 5a_1
		BATCH_SIZE_ALEXNET,		// layer 5a_2
		BATCH_SIZE_ALEXNET,		// layer 5a_3
		BATCH_SIZE_ALEXNET,		// layer 5b_0
		BATCH_SIZE_ALEXNET,		// layer 5b_1
		BATCH_SIZE_ALEXNET,		// layer 5b_2
		BATCH_SIZE_ALEXNET,		// layer 5b_3
		BATCH_SIZE_ALEXNET,		// layer 6_0
		BATCH_SIZE_ALEXNET,		// layer 6_1
		BATCH_SIZE_ALEXNET,		// layer 7_0
		BATCH_SIZE_ALEXNET,		// layer 7_1
		BATCH_SIZE_ALEXNET,		// layer 8_0
		BATCH_SIZE_ALEXNET		// layer 8_1
};
int batch_offset_alexnet[NUM_IP_ALEXNET] = {
		0,		// layer 1_0
		0,		// layer 1_1
		0,		// layer 1_2
		0,		// layer 1_3
		0,		// layer 2a_0
		0,		// layer 2a_1
		0,		// layer 2a_2
		0,		// layer 2a_3
		0,		// layer 2b_0
		0,		// layer 2b_1
		0,		// layer 2b_2
		0,		// layer 2b_3
		0,		// layer 3_0
		0,		// layer 3_1
		0,		// layer 3_2
		0,		// layer 3_3
		0,		// layer 4a_0
		0,		// layer 4a_1
		0,		// layer 4a_2
		0,		// layer 4a_3
		0,		// layer 4b_0
		0,		// layer 4b_1
		0,		// layer 4b_2
		0,		// layer 4b_3
		0,		// layer 5a_0
		0,		// layer 5a_1
		0,		// layer 5a_2
		0,		// layer 5a_3
		0,		// layer 5b_0
		0,		// layer 5b_1
		0,		// layer 5b_2
		0,		// layer 5b_3
		0,		// layer 6_0
		0,		// layer 6_1
		0,		// layer 7_0
		0,		// layer 7_1
		0,		// layer 8_0
		0		// layer 8_1
};
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
int pr_times_alexnet[NUM_IP_ALEXNET] = {
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
};
int run_times_alexnet[NUM_IP_ALEXNET] = {
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
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
char* pr_adders_alexnet[NUM_IP_ALEXNET] = {
	// layer 1_0
	APP5_CONFIG0,
	// layer 1_1
	APP5_CONFIG1,
	// layer 1_2
	APP5_CONFIG2,
	// layer 1_3
	APP5_CONFIG3,
	// layer 2a_0
	APP5_CONFIG4,
	// layer 2a_1
	APP5_CONFIG5,
	// layer 2a_2
	APP5_CONFIG6,
	// layer 2a_3
	APP5_CONFIG7,
	// layer 2b_0
	APP5_CONFIG8,
	// layer 2b_1
	APP5_CONFIG9,
	// layer 2b_2
	APP5_CONFIG10,
	// layer 2b_3
	APP5_CONFIG11,
	// layer 3_0
	APP5_CONFIG12,
	// layer 3_1
	APP5_CONFIG13,
	// layer 3_2
	APP5_CONFIG14,
	// layer 3_3
	APP5_CONFIG15,
	// layer 4a_0
	APP5_CONFIG16,
	// layer 4a_1
	APP5_CONFIG17,
	// layer 4a_2
	APP5_CONFIG18,
	// layer 4a_3
	APP5_CONFIG19,
	// layer 4b_0
	APP5_CONFIG20,
	// layer 4b_1
	APP5_CONFIG21,
	// layer 4b_2
	APP5_CONFIG22,
	// layer 4b_3
	APP5_CONFIG23,
	// layer 5a_0
	APP5_CONFIG24,
	// layer 5a_1
	APP5_CONFIG25,
	// layer 5a_2
	APP5_CONFIG26,
	// layer 5a_3
	APP5_CONFIG27,
	// layer 5b_0
	APP5_CONFIG28,
	// layer 5b_1
	APP5_CONFIG29,
	// layer 5b_2
	APP5_CONFIG30,
	// layer 5b_3
	APP5_CONFIG31,
	// layer 6_0
	APP5_CONFIG32,
	// layer 6_1
	APP5_CONFIG33,
	// layer 7_0
	APP5_CONFIG34,
	// layer 7_1
	APP5_CONFIG35,
	// layer 8_0
	APP5_CONFIG36,
	// layer 8_1
	APP5_CONFIG37,
};

#define TOTAL_NUM_IPS (NUM_IP_3DR + NUM_IP_OPTICAL_FLOW + NUM_IP_LENET)
//#define TOTAL_NUM_IPS (NUM_IP_3DR + NUM_IP_ALEXNET + NUM_IP_OPTICAL_FLOW + NUM_IP_LENET)
//#define TOTAL_NUM_IPS (NUM_IP_3DR + NUM_IP_ALEXNET + NUM_IP_IMG_CMP + NUM_IP_OPTICAL_FLOW + NUM_IP_KNN + NUM_IP_LENET)
//#define TOTAL_NUM_IPS (NUM_IP_OPTICAL_FLOW + NUM_IP_KNN + NUM_IP_LENET)

int multi_app_order[TOTAL_NUM_IPS] = {
		1,
		0,
		2,
		1,
		0,
		2,
		1,
		0,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
};


int *num_ip_array;
int **kern_mapping;
int **slot_mapping;
int **num_batch;
int **batch_offset;
int **num_dep;
int **pr_times;
int **run_times;
int ***dep_list;
char ****bin_names;
char ***pr_adders;

void print_timers() {
	xil_printf("****** TIMING ******\n\r");
	for(int app = 0; app < NUM_APPLICATIONS; app++) {
		for(int ip = 0; ip < num_ip_array[app]; ip++) {
			printf("App %d IP %d Slot %d: \n\r", app, ip, ip_list[app][ip].slot);

			printf("\tIP time: %fus - %fus = %fus\n\r",
					1.0 * (ip_list[app][ip].tEnd_ip) / (COUNTS_PER_SECOND / 1000000),
					1.0 * (ip_list[app][ip]).tStart_ip / (COUNTS_PER_SECOND / 1000000),
					1.0 * (ip_list[app][ip].tEnd_ip - ip_list[app][ip].tStart_ip) / (COUNTS_PER_SECOND / 1000000));
			printf("\tPR time: %fus - %fus = %fus\n\r",
					1.0 * (ip_list[app][ip].tEnd_pr) / (COUNTS_PER_SECOND / 1000000),
					1.0 * (ip_list[app][ip].tStart_pr) / (COUNTS_PER_SECOND / 1000000),
					1.0 * (ip_list[app][ip].tEnd_pr - ip_list[app][ip].tStart_pr) / (COUNTS_PER_SECOND / 1000000));
			printf("\tWait time: %fus - %fus = %fus\n\r",
					1.0 * (ip_list[app][ip].tEnd_wait) / (COUNTS_PER_SECOND / 1000000),
					1.0 * (ip_list[app][ip].tStart_wait) / (COUNTS_PER_SECOND / 1000000),
					1.0 * (ip_list[app][ip].tEnd_wait - ip_list[app][ip].tStart_wait) / (COUNTS_PER_SECOND / 1000000));
			printf("\tRun time: %fus - %fus = %fus\n\r",
					1.0 * (ip_list[app][ip].tEnd_run) / (COUNTS_PER_SECOND / 1000000),
					1.0 * (ip_list[app][ip].tStart_run) / (COUNTS_PER_SECOND / 1000000),
					1.0 * (ip_list[app][ip].tEnd_run - ip_list[app][ip].tStart_run) / (COUNTS_PER_SECOND / 1000000));
			printf("\tIdle time: %fus - %fus = %fus\n\r",
					1.0 * (ip_list[app][ip].tStart_run) / (COUNTS_PER_SECOND / 1000000),
					1.0 * (ip_list[app][0].tStart_ip) / (COUNTS_PER_SECOND / 1000000),
					1.0 * (ip_list[app][ip].tStart_run - ip_list[app][0].tStart_ip) / (COUNTS_PER_SECOND / 1000000));
		}
		printf("Total app time: %fus - %fus = %fus\n\r",
				1.0 * (ip_list[app][num_ip_array[app] - 1].tEnd_ip) / (COUNTS_PER_SECOND / 1000000),
				1.0 * (ip_list[app][0].tStart_ip) / (COUNTS_PER_SECOND / 1000000),
				1.0 * (ip_list[app][num_ip_array[app] - 1].tEnd_ip - ip_list[app][0].tStart_ip) / (COUNTS_PER_SECOND / 1000000));
	}
}

int apps_init() {

	// Allocating arrays
#ifdef DEBUG
	xil_printf("Allocating scheduling structures\n\r");
#endif
	num_ip_array = 	(int *)		malloc(sizeof(int) * NUM_APPLICATIONS);
	kern_mapping = 	(int **)	malloc(sizeof(int *) * NUM_APPLICATIONS);
	slot_mapping = 	(int **)	malloc(sizeof(int *) * NUM_APPLICATIONS);
	pr_times 	= 	(int **)	malloc(sizeof(int *) * NUM_APPLICATIONS);
	run_times 	= 	(int **)	malloc(sizeof(int *) * NUM_APPLICATIONS);
	num_batch = 	(int **)	malloc(sizeof(int *) * NUM_APPLICATIONS);
	batch_offset = 	(int **)	malloc(sizeof(int *) * NUM_APPLICATIONS);
	num_dep = 		(int **)	malloc(sizeof(int *) * NUM_APPLICATIONS);
	dep_list = 		(int ***)	malloc(sizeof(int **) * NUM_APPLICATIONS);
	bin_names = 	(char ****)	malloc(sizeof(char ***) * NUM_APPLICATIONS);
	pr_adders = 	(char ***)	malloc(sizeof(char **) * NUM_APPLICATIONS);


	// Adding 3DR to structures
#ifdef DEBUG
	xil_printf("Creating lenet scheduling structures\n\r");
#endif
	num_ip_array[0] = 	NUM_IP_3DR;
	kern_mapping[0] = 	(int *)		malloc(sizeof(int) 		* NUM_IP_3DR);
	slot_mapping[0] = 	(int *)		malloc(sizeof(int) 		* NUM_IP_3DR);
	pr_times[0]		= 	(int *)		malloc(sizeof(int)		* NUM_IP_3DR);
	run_times[0]	= 	(int *)		malloc(sizeof(int) 		* NUM_IP_3DR);
	num_batch[0] 	= 	(int *)		malloc(sizeof(int) 		* NUM_IP_3DR);
	batch_offset[0] = 	(int *)		malloc(sizeof(int *) 	* NUM_IP_3DR);
	num_dep[0] 		= 	(int *)		malloc(sizeof(int *) 	* NUM_IP_3DR);
	dep_list[0] 	= 	(int **)	malloc(sizeof(int *) 	* NUM_IP_3DR);
	bin_names[0] 	= 	(char ***)	malloc(sizeof(char **) 	* NUM_IP_3DR);
	pr_adders[0] 	= 	(char **)	malloc(sizeof(char *) 	* NUM_IP_3DR);
	for(int i = 0; i < NUM_IP_LENET; i++) {
		kern_mapping[0][i] = kern_mapping_3dr[i];
		slot_mapping[0][i] = slot_mapping_3dr[i];
		pr_times[0][i] = pr_times_3dr[i];
		run_times[0][i] = run_times_3dr[i];
		num_batch[0][i] = num_batch_3dr[i];
		batch_offset[0][i] = batch_offset_3dr[i];
		num_dep[0][i] = num_dep_3dr[i];
		dep_list[0][i] 	= dep_list_3dr[i];
		bin_names[0][i] = bin_names_3dr[i];
		pr_adders[0][i] = pr_adders_3dr[i];
	}

	// Adding lenet to structures
#ifdef DEBUG
	xil_printf("Creating lenet scheduling structures\n\r");
#endif
	num_ip_array[1] = 	NUM_IP_LENET;
	kern_mapping[1] = 	(int *)		malloc(sizeof(int) 		* NUM_IP_LENET);
	slot_mapping[1] = 	(int *)		malloc(sizeof(int) 		* NUM_IP_LENET);
	pr_times[1]		= 	(int *)		malloc(sizeof(int)		* NUM_IP_LENET);
	run_times[1]	= 	(int *)		malloc(sizeof(int) 		* NUM_IP_LENET);
	num_batch[1] 	= 	(int *)		malloc(sizeof(int) 		* NUM_IP_LENET);
	batch_offset[1] = 	(int *)		malloc(sizeof(int *) 	* NUM_IP_LENET);
	num_dep[1] 		= 	(int *)		malloc(sizeof(int *) 	* NUM_IP_LENET);
	dep_list[1] 	= 	(int **)	malloc(sizeof(int *) 	* NUM_IP_LENET);
	bin_names[1] 	= 	(char ***)	malloc(sizeof(char **) 	* NUM_IP_LENET);
	pr_adders[1] 	= 	(char **)	malloc(sizeof(char *) 	* NUM_IP_LENET);
	for(int i = 0; i < NUM_IP_LENET; i++) {
		kern_mapping[1][i] = kern_mapping_lenet[i];
		slot_mapping[1][i] = slot_mapping_lenet[i];
		pr_times[1][i] = pr_times_lenet[i];
		run_times[1][i] = run_times_lenet[i];
		num_batch[1][i] = num_batch_lenet[i];
		batch_offset[1][i] = batch_offset_lenet[i];
		num_dep[1][i] = num_dep_lenet[i];
		dep_list[1][i] 	= dep_list_lenet[i];
		bin_names[1][i] = bin_names_lenet[i];
		pr_adders[1][i] = pr_adders_lenet[i];
	}

//	// Adding knn to structures
//#ifdef DEBUG
//	xil_printf("Creating knn scheduling structures\n\r");
//#endif
//	num_ip_array[2] = 	NUM_IP_KNN;
//	kern_mapping[2] = 	(int *)		malloc(sizeof(int) 		* NUM_IP_KNN);
//	slot_mapping[2] = 	(int *)		malloc(sizeof(int) 		* NUM_IP_KNN);
//	pr_times[2] 	= 	(int *)		malloc(sizeof(int) 		* NUM_IP_KNN);
//	run_times[2] 	= 	(int *)		malloc(sizeof(int) 		* NUM_IP_KNN);
//	num_batch[2] 	= 	(int *)		malloc(sizeof(int) 		* NUM_IP_KNN);
//	batch_offset[2] = 	(int *)		malloc(sizeof(int *) 	* NUM_IP_KNN);
//	num_dep[2] 		= 	(int *)		malloc(sizeof(int *) 	* NUM_IP_KNN);
//	dep_list[2] 	= 	(int **)	malloc(sizeof(int *) 	* NUM_IP_KNN);
//	bin_names[2] 	= 	(char ***)	malloc(sizeof(char **) 	* NUM_IP_KNN);
//	pr_adders[2] 	= 	(char **)	malloc(sizeof(char *) 	* NUM_IP_KNN);
//	for(int i = 0; i < NUM_IP_KNN; i++) {
//		kern_mapping[2][i] = kern_mapping_knn[i];
//		slot_mapping[2][i] = slot_mapping_knn[i];
//		pr_times[2][i] = pr_times_knn[i];
//		run_times[2][i] = run_times_knn[i];
//		num_batch[2][i] = num_batch_knn[i];
//		batch_offset[2][i] = batch_offset_knn[i];
//		num_dep[2][i] = num_dep_knn[i];
//		dep_list[2][i] 	= dep_list_knn[i];
//		bin_names[2][i] = bin_names_knn[i];
//		pr_adders[2][i] = pr_adders_knn[i];
//	}

	// Adding optical flow to structures
#ifdef DEBUG
	xil_printf("Creating opt flow scheduling structures\n\r");
#endif
	num_ip_array[2] = 	NUM_IP_OPTICAL_FLOW;
	kern_mapping[2] = 	(int *)		malloc(sizeof(int) 		* NUM_IP_OPTICAL_FLOW);
	slot_mapping[2] = 	(int *)		malloc(sizeof(int) 		* NUM_IP_OPTICAL_FLOW);
	pr_times[2] 	= 	(int *)		malloc(sizeof(int) 		* NUM_IP_OPTICAL_FLOW);
	run_times[2] 	= 	(int *)		malloc(sizeof(int) 		* NUM_IP_OPTICAL_FLOW);
	num_batch[2] 	= 	(int *)		malloc(sizeof(int) 		* NUM_IP_OPTICAL_FLOW);
	batch_offset[2] = 	(int *)		malloc(sizeof(int *) 	* NUM_IP_OPTICAL_FLOW);
	num_dep[2] 		= 	(int *)		malloc(sizeof(int *) 	* NUM_IP_OPTICAL_FLOW);
	dep_list[2] 	= 	(int **)	malloc(sizeof(int *) 	* NUM_IP_OPTICAL_FLOW);
	bin_names[2] 	= 	(char ***)	malloc(sizeof(char **) 	* NUM_IP_OPTICAL_FLOW);
	pr_adders[2] 	= 	(char **)	malloc(sizeof(char *) 	* NUM_IP_OPTICAL_FLOW);
	for(int i = 0; i < NUM_IP_OPTICAL_FLOW; i++) {
		kern_mapping[2][i] = kern_mapping_opt_flow[i];
		slot_mapping[2][i] = slot_mapping_opt_flow[i];
		pr_times[2][i] = pr_times_opt_flow[i];
		run_times[2][i] = run_times_opt_flow[i];
		num_batch[2][i] = num_batch_opt_flow[i];
		batch_offset[2][i] = batch_offset_opt_flow[i];
		num_dep[2][i] = num_dep_opt_flow[i];
		dep_list[2][i] 	= dep_list_opt_flow[i];
		bin_names[2][i] = bin_names_opt_flow[i];
		pr_adders[2][i] = pr_adders_opt_flow[i];
	}

//	// Adding alexnet to structures
//#ifdef DEBUG
//	xil_printf("Creating alexnet scheduling structures\n\r");
//#endif
//	num_ip_array[3] = 	NUM_IP_ALEXNET;
//	kern_mapping[3] = 	(int *)		malloc(sizeof(int) 		* NUM_IP_ALEXNET);
//	slot_mapping[3] = 	(int *)		malloc(sizeof(int) 		* NUM_IP_ALEXNET);
//	pr_times[3] 	= 	(int *)		malloc(sizeof(int) 		* NUM_IP_ALEXNET);
//	run_times[3] 	= 	(int *)		malloc(sizeof(int) 		* NUM_IP_ALEXNET);
//	num_batch[3] 	= 	(int *)		malloc(sizeof(int) 		* NUM_IP_ALEXNET);
//	batch_offset[3] = 	(int *)		malloc(sizeof(int *) 	* NUM_IP_ALEXNET);
//	num_dep[3] 		= 	(int *)		malloc(sizeof(int *) 	* NUM_IP_ALEXNET);
//	dep_list[3] 	= 	(int **)	malloc(sizeof(int *) 	* NUM_IP_ALEXNET);
//	bin_names[3] 	= 	(char ***)	malloc(sizeof(char **) 	* NUM_IP_ALEXNET);
//	pr_adders[3] 	= 	(char **)	malloc(sizeof(char *) 	* NUM_IP_ALEXNET);
//	for(int i = 0; i < NUM_IP_ALEXNET; i++) {
//		kern_mapping[3][i] = kern_mapping_alexnet[i];
//		slot_mapping[3][i] = slot_mapping_alexnet[i];
//		run_times[3][i] = run_times_alexnet[i];
//		pr_times[3][i] = pr_times_alexnet[i];
//		num_batch[3][i] = num_batch_alexnet[i];
//		batch_offset[3][i] = batch_offset_alexnet[i];
//		num_dep[3][i] = num_dep_alexnet[i];
//		dep_list[3][i] 	= dep_list_alexnet[i];
//		bin_names[3][i] = bin_names_alexnet[i];
//		pr_adders[3][i] = pr_adders_alexnet[i];
//	}

//	// Adding img cmp to structures
//#ifdef DEBUG
//	xil_printf("Creating img cmp scheduling structures\n\r");
//#endif
//	num_ip_array[3] = 	NUM_IP_IMG_CMP;
//	kern_mapping[3] = 	(int *)		malloc(sizeof(int) 		* NUM_IP_IMG_CMP);
//	slot_mapping[3] = 	(int *)		malloc(sizeof(int) 		* NUM_IP_IMG_CMP);
//	pr_times[3] 	= 	(int *)		malloc(sizeof(int) 		* NUM_IP_IMG_CMP);
//	run_times[3] 	= 	(int *)		malloc(sizeof(int) 		* NUM_IP_IMG_CMP);
//	num_batch[3] 	= 	(int *)		malloc(sizeof(int) 		* NUM_IP_IMG_CMP);
//	batch_offset[3] = 	(int *)		malloc(sizeof(int *) 	* NUM_IP_IMG_CMP);
//	num_dep[3] 		= 	(int *)		malloc(sizeof(int *) 	* NUM_IP_IMG_CMP);
//	dep_list[3] 	= 	(int **)	malloc(sizeof(int *) 	* NUM_IP_IMG_CMP);
//	bin_names[3] 	= 	(char ***)	malloc(sizeof(char **) 	* NUM_IP_IMG_CMP);
//	pr_adders[3] 	= 	(char **)	malloc(sizeof(char *) 	* NUM_IP_IMG_CMP);
//	for(int i = 0; i < NUM_IP_IMG_CMP; i++) {
//		kern_mapping[3][i] = kern_mapping_img_cmp[i];
//		slot_mapping[3][i] = slot_mapping_img_cmp[i];
//		pr_times[3][i] = pr_times_img_cmp[i];
//		run_times[3][i] = run_times_img_cmp[i];
//		num_batch[3][i] = num_batch_img_cmp[i];
//		batch_offset[3][i] = batch_offset_img_cmp[i];
//		num_dep[3][i] = num_dep_img_cmp[i];
//		dep_list[3][i] 	= dep_list_img_cmp[i];
//		bin_names[3][i] = bin_names_img_cmp[i];
//		pr_adders[3][i] = pr_adders_img_cmp[i];
//	}





#ifdef DEBUG
	xil_printf("Sucessfully created all apps\n\r");
#endif

	return XST_SUCCESS;

}

int choose_next_app(int next_app_idx) {

	// If last app return -1
	if(next_app_idx < TOTAL_NUM_IPS) {
		return multi_app_order[next_app_idx];
	}
	else {
		return -1;
	}


}


void sched_init(struct slot *slots, struct ip ip_list[NUM_APPLICATIONS][MAX_NUM_IP]){
    int i, j, slot, kern, run_time, pr_time;
    for (i = 0; i < NUM_SLOTS; i++){
        init_slot(i, &slots[i]);
    }
    for(i = 0; i < NUM_APPLICATIONS; i++) {
		for (j = 0; j < num_ip_array[i]; j++){
#ifdef DEBUG
			xil_printf("App %d IP %d\n\r", i, j);
#endif
			slot = slot_mapping[i][j];
			kern = kern_mapping[i][j];
			pr_time = pr_times[i][j];
			run_time = run_times[i][j];
			init_ip_entry(j, i, bin_len[slot], dep_list[i][j], num_dep[i][j], batch_offset[i][j], num_batch[i][j], slot,
					kern, pr_time, run_time, decouple_gpio[slot], reset_gpio[slot], bin_names[i][kern][slot], pr_adders[i][kern], &ip_list[i][j]);
		}
    }
}

void init_slot(int id, struct slot *ret){
	int status;
	ret->status = IDLE;
    ret->id = id;
	ret->batch_count = 0;
	ret->num_invoke = 0;

	// Initializing reset and setting to one
	XGpioPs_SetDirectionPin(&psGpioInstancePtr, reset_gpio[id], 0x1);
	XGpioPs_SetOutputEnablePin(&psGpioInstancePtr, reset_gpio[id], 1); // Setting direction to output
	XGpioPs_WritePin(&psGpioInstancePtr, reset_gpio[id], 1);

	// Initializing decouple and setting to zero
	XGpioPs_SetDirectionPin(&psGpioInstancePtr, decouple_gpio[id], 0x1);
	XGpioPs_SetOutputEnablePin(&psGpioInstancePtr, decouple_gpio[id], 1);
	XGpioPs_WritePin(&psGpioInstancePtr, decouple_gpio[id], 0);

	// Initializing accelerator
	XHls_ip_Config accelerator_config;
	accelerator_config.DeviceId = 0; // Can this just be zero? Or do we need to set it to the value in xparams?
	accelerator_config.Axilites_BaseAddress = slot_baseaddr[id];
	status = XHls_ip_CfgInitialize(&ret->accelerator, &accelerator_config);
	if(status != XST_SUCCESS) {
		xil_printf("[ERROR] Initialization of slot %d failed.", id);
	}

}

void init_ip_entry(int id, int app_id, u32 len, int *dep, int num_dep, int batch_offset, int batch,
		int slot, int kern, int pr_time, int run_time, int decouple, int reset, char* bin_name, char* pr_address, struct ip *ret){

    ret->id = id;
    ret->app_id = app_id;
    ret->dep = dep;
	ret->num_dep = num_dep;
	ret->batch = batch;
    ret->slot = slot;
    ret->kern = kern;
    ret->pr_time = pr_time;
    ret->run_time = run_time;
    ret->BitAddr = (u64)pr_address;
    ret->Bitlen = len;
    ret->decouple = decouple;
    ret->reset = reset;
    ret->num_invoke = 0;
    ret->bin_name = bin_name;
	ret->batch_offset = batch_offset;
#ifdef DEBUG
    xil_printf("Setting up app %d ip %d with slot %d kern %d bin_name %s decouple %d and reset %d\n\r", ret->app_id, ret->id, ret->slot, ret->kern, ret->bin_name, ret->decouple, ret->reset);
    xil_printf("with address: %p, run_time: %d pr_time: %d\n\r", ret->BitAddr, ret->run_time, ret->pr_time);
    int i;
    for (i = 0; i < ret->num_dep; i++){
    	xil_printf("%d ", ret->dep[i]);
    }
    xil_printf("\r\n");
#endif
}

int SD_TransferPartial(const char *FileName, u64 DestinationAddress, u32 ByteLength)
{
  FIL fil;
  FRESULT rc;
  UINT br;

  rc = f_open(&fil, FileName, FA_READ);
  if (rc) {
    xil_printf(" ERROR : f_open returned %d\r\n", rc);
    return XST_FAILURE;
  }

  rc = f_lseek(&fil, 0);
  if (rc) {
    xil_printf(" ERROR : f_lseek returned %d\r\n", rc);
    return XST_FAILURE;
  }

  rc = f_read(&fil, (void*) DestinationAddress, ByteLength, &br);
  if (rc) {
    xil_printf(" ERROR : f_read returned %d\r\n", rc);
    return XST_FAILURE;
  }

  rc = f_close(&fil);
  if (rc) {
    xil_printf(" ERROR : f_close returned %d\r\n", rc);
    return XST_FAILURE;
  }

  return XST_SUCCESS;
}

void free_all(){
    return;
}

void load_bins(){
    int i;
    struct ip *iterator;
    for(int i = 0; i < NUM_APPLICATIONS; i++) {
    	for(int j = 0; j < num_ip_array[i]; j++) {
        	iterator = &ip_list[i][j];
#ifdef DEBUG
        	xil_printf("Loading %s for App %d IP %d into address %p\r\n", iterator->bin_name, i, j, iterator->BitAddr);
#endif
        	SD_TransferPartial(iterator->bin_name, iterator->BitAddr, iterator->Bitlen); // TODO: This might be an issue
    	}
    }
}

int get_total_ips() {
	int ret_val = 0;

	for(int i = 0; i < NUM_APPLICATIONS; i++) {
		ret_val += num_ip_array[i];
	}

	return ret_val;
}
