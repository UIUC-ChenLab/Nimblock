/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xil_io.h"
#include "xil_types.h"
#include "xgpiops.h"
#include "xil_cache.h"
#include "ff.h"
#include "xhls_ip.h"
#include "xtime_l.h"
#include "scheduler.h"
#include "globals.h"
#include "imageLib/imageLib.h"
#include "xilfpga.h"
#include "xfpga_config.h"



//#define NUM_RESET 10000

#undef str //fix for sstream, https://stackoverflow.com/questions/49389144/how-to-use-stringstream-in-xilinx-sdk

// Include all input data
#include "input_data.h"
#include "training_data.h"
#include "testing_data.h"

// number of us in an ILP time-step
#define TIME_STEP 1

// Used to control which scenario we run
#define MULTI_APP
#define PIPELINE
#define APP_ID 2 // APP ID if we are only running one app. If MULTI_APP is defined this does nothing
#define APP_NUM_IPS 3 // Number of IPS if we are only running one app

// #defines used when compiling Alexnet HLS - Defines how many slots the layers were split up into. All spliting is by output channel dimension
#define ALEXNET_MAX_SLOTS 2
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
#define ALEXNET_FC6_SLOTS ALEXNET_MAX_SLOTS
#define ALEXNET_FC6_OUTPUT 256
#define ALEXNET_FC7_SLOTS ALEXNET_MAX_SLOTS
#define ALEXNET_FC7_OUTPUT 256
#define ALEXNET_FC8_SLOTS ALEXNET_MAX_SLOTS
#define ALEXNET_FC8_OUTPUT 1000

// static status objects for schedule
struct slot slots[NUM_SLOTS];
struct ip ip_list[NUM_APPLICATIONS][MAX_NUM_IP];

// KNN structures -- Need to be 64-bit aligned as we use 64-bit data bus
char training_set_global[BATCH_SIZE_KNN][NUM_TRAINING * sizeof(WholeDigitType) + 32] __attribute__ ((aligned(8)));
char testing_set_global[BATCH_SIZE_KNN][NUM_TEST * sizeof(WholeDigitType) + 32] __attribute__ ((aligned(8)));
char knn_set_global[BATCH_SIZE_KNN][NUM_TEST * PAR_FACTOR * K_CONST * sizeof(int) + 32] __attribute__ ((aligned(8)));
char results_global[BATCH_SIZE_KNN][NUM_TEST * sizeof(LabelType) + 32] __attribute__ ((aligned(8)));
buffers_knn buf_knn[BATCH_SIZE_KNN];

// Lenet structures -- Need to be 64-bit aligned as we use 64-bit data bus
volatile float image[32][32] __attribute__ ((aligned(8))) = {0};
volatile float conv1_weights[6][5][5] __attribute__ ((aligned(8))) = {0};
volatile float conv1_bias[6] __attribute__ ((aligned(8))) = {0};
volatile float pool2_output[BATCH_SIZE_LENET][6][14][14] __attribute__ ((aligned(8))) = {0};
volatile float conv3_weights[16][6][5][5] __attribute__ ((aligned(8))) = {0};
volatile float conv3_bias[16] __attribute__ ((aligned(8))) = {0};
volatile float pool4_output[BATCH_SIZE_LENET][16][5][5] __attribute__ ((aligned(8))) = {0};
volatile float conv5_weights[120][16][5][5] __attribute__ ((aligned(8))) = {0};
volatile float conv5_bias[120] __attribute__ ((aligned(8))) = {0};
volatile float fc6_weights[10][120] __attribute__ ((aligned(8))) = {0};
volatile float fc6_bias[10] __attribute__ ((aligned(8))) = {0};
volatile float fc6_output[10] __attribute__ ((aligned(8))) = {0};
unsigned char lenet_images[LENET_NUM_TEST*28*28];
unsigned char lenet_labels[LENET_NUM_TEST];
int lenet_num_correct = 0;


// Image compression structures -- Need to be 64-bit aligned as we use 64-bit data bus
int W = 16;
int H = 32;
float img_cmp_src[BATCH_SIZE_IMG_CMP][1920 * 1080] __attribute__ ((aligned(8)));
float img_cmp_dst[BATCH_SIZE_IMG_CMP][1920 * 1080] __attribute__ ((aligned(8)));
float img_cmp_output_golden[BATCH_SIZE_IMG_CMP][1920 * 1080];


// 3DR structures -- Alignment is encoded in structure so don't need to do it here
buffers_3dr buf_3dr[BATCH_SIZE_3DR];


// Optical flow structures -- Alignment is encoded in structure so don't need to do it here
buffers_optical_flow buf_optical_flow[BATCH_SIZE_OPTICAL_FLOW];

// Alexnet structures -- Alignment is encoded in structure so don't need to do it here
buffers_alexnet buf_alexnet[BATCH_SIZE_ALEXNET];


// static status objects for schedule
int done_list[NUM_APPLICATIONS][MAX_NUM_IP];
int num_done[NUM_APPLICATIONS];
int batch_done[NUM_APPLICATIONS][MAX_NUM_IP] = {0};
int cur_progress[NUM_APPLICATIONS] = {0};
int total_cur_progress = 0;
int pr_in_progress = 0;

// Static objects for PR
int status;
XFpga XFpgaInstance = {0U};

// Static objects for GPIO
XGpioPs psGpioInstancePtr;
XGpioPs_Config *GpioConfigPtr;
int xStatus;

// SD card and file reading objects
static FIL fil;
static FATFS fatfs;
static char *SD_File;

XTime cur_time, first_pr_time;


int SD_Init() {
	FRESULT rc;

	rc = f_mount(&fatfs, "", 0);
	if (rc) {
		xil_printf(" ERROR : f_mount returned %d\r\n", rc);
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

float img_cmp_check_result(volatile float *golden, volatile float *test, int max_width, int max_height){
	int w, h;
	float cum_error = 0.0;
	float difference = 0.0;
	for (h = 0; h < max_height; h++){
		for (w = 0; w < max_width; w++){
			difference = golden[h * W + w] - test[h * W + w];
			if (golden[h * W + w] == 0){
				xil_printf("Error! 0 input!\r\n");
			}
			cum_error += sqrt(difference * difference) / golden[h * W + w];
		}
	}

    return cum_error / (W * H);
}

// Initializing the gpio object
int init_gpio() {
	GpioConfigPtr = XGpioPs_LookupConfig(XPAR_PSU_APM_0_DEVICE_ID);

	if(GpioConfigPtr == NULL) {
		print("Cannot acquire GPIO config Ptr \n\r");
		return XST_FAILURE;
	}

	xStatus = XGpioPs_CfgInitialize(&psGpioInstancePtr,
		  GpioConfigPtr,
		  GpioConfigPtr->BaseAddr);

	if(XST_SUCCESS != xStatus) {
		print("PS GPIO INIT FAILED \n\r");
		return XST_FAILURE;
	}
	else {
		print("PS GPIO INIT SUCCEED \n\r");
		return XST_SUCCESS;
	}

}


int ip_check_dep(struct ip *self){

	if (self->num_dep == 0)
		return 1;
	int remain = self->num_dep;
#ifdef PIPELINE
// iterate through all dependencies and ensure that each dependency has processed more than you
	for (int i = 0; i < self->num_dep; i++){
		if(batch_done[self->app_id][self->dep[i]] > batch_done[self->app_id][self->id]) {
			remain--;
		}
		if (remain == 0){
			return 1;
		}
	}

	return 0;
#else
	for(int i = 0; i < self->num_dep; i++) {
		for(int j = 0; j < num_done[self->app_id]; j++) {
			if(done_list[self->app_id][j] == self->dep[i]) {
				remain--;
				break;
			}
		}
		if (remain == 0){
			return 1;
		}
	}

	return 0;
#endif
}


// make a customized the set val function
void XAccel_Set_Val(XHls_ip *InstancePtr, u64 Data, int offset) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
    XHls_ip_WriteReg(InstancePtr->Axilites_BaseAddress, offset, (u32)Data);
}


void set_params_and_run_bundle1(XHls_ip* slot, buffers_3dr buf) {
#ifdef DEBUG
	xil_printf("Passing Parameters bundle1\r\n");
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)(buf.input), 0x18); //explicitly pass the pointer as an int
	XAccel_Set_Val(slot, (u64)(buf.fragment_x), 0x20);
	XAccel_Set_Val(slot, (u64)(buf.fragment_y), 0x28);
	XAccel_Set_Val(slot, (u64)(buf.fragment_z), 0x30);
	XAccel_Set_Val(slot, (u64)(buf.fragment_color), 0x38);
	XAccel_Set_Val(slot, (u64)(buf.size_fragment), 0x40);
	XHls_ip_Start(slot);
}

void set_params_and_run_zculling(XHls_ip* slot, buffers_3dr buf){
#ifdef DEBUG
	xil_printf("Passing Parameters zculling\r\n");
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)buf.fragment_x, 0x18);
	XAccel_Set_Val(slot, (u64)buf.fragment_y, 0x20);
	XAccel_Set_Val(slot, (u64)buf.fragment_z, 0x28);
	XAccel_Set_Val(slot, (u64)buf.fragment_color, 0x30);
	XAccel_Set_Val(slot, (u64)buf.size_fragment, 0x38);
	XAccel_Set_Val(slot, (u64)buf.pixels_x, 0x40);
	XAccel_Set_Val(slot, (u64)buf.pixels_y, 0x48);
	XAccel_Set_Val(slot, (u64)buf.pixels_color, 0x50);
	XAccel_Set_Val(slot, (u64)buf.size_pixels, 0x58);
	XHls_ip_Start(slot);
}

void set_params_and_run_bundle2(XHls_ip* slot, buffers_3dr buf){
#ifdef DEBUG
	xil_printf("Passing Parameters bundle2\r\n");
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)buf.size_pixels, 0x18);
	XAccel_Set_Val(slot, (u64)buf.pixels_x, 0x20);
	XAccel_Set_Val(slot, (u64)buf.pixels_y, 0x28);
	XAccel_Set_Val(slot, (u64)buf.pixels_color, 0x30);
	XAccel_Set_Val(slot, (u64)buf.output, 0x38);
	XHls_ip_Start(slot);
}

void set_params_and_run_layer_conv1_pool2(XHls_ip* slot, volatile float input_image[32][32], volatile float output_pool2_output[6][14][14]) {
#ifdef DEBUG
	xil_printf("Passing Parameters layer_conv1_pool2\r\n");
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)input_image, 0x18); //explicitly pass the pointer as an int
	XAccel_Set_Val(slot, (u64)conv1_weights, 0x20);
	XAccel_Set_Val(slot, (u64)conv1_bias, 0x28);
	XAccel_Set_Val(slot, (u64)output_pool2_output, 0x30);
	XHls_ip_Start(slot);
}

void set_params_and_run_layer_conv3_pool4(XHls_ip* slot, volatile float input_pool2_output[6][14][14], volatile float output_pool4_output[16][5][5]){
#ifdef DEBUG
	xil_printf("Passing Parameters layer_conv3_pool4\r\n");
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)input_pool2_output, 0x18);
	XAccel_Set_Val(slot, (u64)conv3_weights, 0x20);
	XAccel_Set_Val(slot, (u64)conv3_bias, 0x28);
	XAccel_Set_Val(slot, (u64)output_pool4_output, 0x30);
	XHls_ip_Start(slot);
}

void set_params_and_run_layer_conv5_fc6(XHls_ip* slot, volatile float input_pool4_output[16][5][5], volatile float output_fc6_output[10]){
#ifdef DEBUG
	xil_printf("Passing Parameters layer_conv5_fc6\r\n");
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)input_pool4_output, 0x18);
	XAccel_Set_Val(slot, (u64)conv5_weights, 0x20);
	XAccel_Set_Val(slot, (u64)conv5_bias, 0x28);
	XAccel_Set_Val(slot, (u64)fc6_weights, 0x30);
	XAccel_Set_Val(slot, (u64)fc6_bias, 0x38);
	XAccel_Set_Val(slot, (u64)output_fc6_output, 0x40);
	XHls_ip_Start(slot);
}

void set_params_and_run_knn_init(XHls_ip* slot, buffers_knn buf) {
#ifdef DEBUG
	xil_printf("Passing Parameters knn_init\r\n");
#endif
	XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)buf.knn_set, 0x18);
	XHls_ip_Start(slot);
}

void set_params_and_run_knn_train(XHls_ip* slot, buffers_knn buf) {
#ifdef DEBUG
	xil_printf("Passing Parameters knn_train\r\n");
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)buf.training_set, 0x18);
	XAccel_Set_Val(slot, (u64)buf.testing_set, 0x20);
	XAccel_Set_Val(slot, (u64)buf.knn_set, 0x28);
	XHls_ip_Start(slot);
}

void set_params_and_run_knn_pred(XHls_ip* slot, buffers_knn buf) {
#ifdef DEBUG
	xil_printf("Passing Parameters knn_pred\r\n");
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)(buf.results + 8), 0x18);
	XAccel_Set_Val(slot, (u64)buf.knn_set, 0x20);
	XHls_ip_Start(slot);
}

void set_params_and_run_bundle1_module(XHls_ip* slot, int batch) {
#ifdef DEBUG
	xil_printf("Passing Parameters bundle1_module\r\n");
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)img_cmp_src[batch], 0x18);
	XAccel_Set_Val(slot, (u64)img_cmp_dst[batch], 0x20);
	XAccel_Set_Val(slot, (u64)25, 0x28);
	XAccel_Set_Val(slot, (u64)10, 0x30);
	XAccel_Set_Val(slot, (u64)W, 0x38);
	XAccel_Set_Val(slot, (u64)H, 0x40);
	XHls_ip_Start(slot);
}

void set_params_and_run_dct(XHls_ip* slot, int batch) {
#ifdef DEBUG
	xil_printf("Passing Parameters dct\r\n");
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)img_cmp_src[batch], 0x18);
	XAccel_Set_Val(slot, (u64)img_cmp_dst[batch], 0x20);
	XAccel_Set_Val(slot, (u64)W, 0x28);
	XAccel_Set_Val(slot, (u64)H, 0x30);
	XHls_ip_Start(slot);
}

void set_params_and_run_quantize(XHls_ip* slot, int batch) {
#ifdef DEBUG
	xil_printf("Passing Parameters quantize\r\n");
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)img_cmp_dst[batch], 0x18);
	XAccel_Set_Val(slot, (u64)img_cmp_src[batch], 0x20);
	XAccel_Set_Val(slot, (u64)W, 0x28);
	XAccel_Set_Val(slot, (u64)H, 0x30);
	XHls_ip_Start(slot);
}

void set_params_and_run_dequantize(XHls_ip* slot, int batch) {
#ifdef DEBUG
	xil_printf("Passing Parameters dequantize\r\n");
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)img_cmp_src[batch], 0x18);
	XAccel_Set_Val(slot, (u64)img_cmp_dst[batch], 0x20);
	XAccel_Set_Val(slot, (u64)W, 0x28);
	XAccel_Set_Val(slot, (u64)H, 0x30);
	XHls_ip_Start(slot);
}

void set_params_and_run_idct(XHls_ip* slot, int batch) {
#ifdef DEBUG
	xil_printf("Passing Parameters idct\r\n");
#endif
	XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)img_cmp_dst[batch], 0x18);
	XAccel_Set_Val(slot, (u64)img_cmp_src[batch], 0x20);
	XAccel_Set_Val(slot, (u64)W, 0x28);
	XAccel_Set_Val(slot, (u64)H, 0x30);
	XHls_ip_Start(slot);
}

void set_params_and_run_reconstructBlockD(XHls_ip* slot, int batch) {
#ifdef DEBUG
	xil_printf("Passing Parameters reconstructBlockD\r\n");
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)img_cmp_src[batch], 0x18);
	XAccel_Set_Val(slot, (u64)img_cmp_dst[batch], 0x20);
	XAccel_Set_Val(slot, (u64)W, 0x28);
	XAccel_Set_Val(slot, (u64)H, 0x30);
	XHls_ip_Start(slot);
}

// functions for setting parameters
void set_params_read_input(XHls_ip* slot, buffers_optical_flow* buf){
#ifdef DEBUG
	xil_printf("Passing Parameters set_params_read_input\r\n");
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)buf->frames, 0x18);
	XAccel_Set_Val(slot, (u64)buf->frame1_a, 0x20);
	XAccel_Set_Val(slot, (u64)buf->frame2_a, 0x28);
	XAccel_Set_Val(slot, (u64)buf->frame3_a, 0x30);
	XAccel_Set_Val(slot, (u64)buf->frame3_b, 0x38);
	XAccel_Set_Val(slot, (u64)buf->frame4_a, 0x40);
	XAccel_Set_Val(slot, (u64)buf->frame5_a, 0x48);
	XHls_ip_Start(slot);
}

void set_params_gradient_xy_calc(XHls_ip* slot, buffers_optical_flow* buf){
#ifdef DEBUG
	xil_printf("Passing Parameters set_params_gradient_xy_calc\r\n");
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)buf->frame3_a, 0x18);
	XAccel_Set_Val(slot, (u64)buf->gradient_x, 0x20);
	XAccel_Set_Val(slot, (u64)buf->gradient_y, 0x28);
	XHls_ip_Start(slot);
}

void set_params_gradient_z_calc(XHls_ip* slot, buffers_optical_flow* buf){
#ifdef DEBUG
	xil_printf("Passing Parameters set_params_gradient_z_calc\r\n");
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)buf->frame1_a, 0x18);
	XAccel_Set_Val(slot, (u64)buf->frame2_a, 0x20);
	XAccel_Set_Val(slot, (u64)buf->frame3_b, 0x28);
	XAccel_Set_Val(slot, (u64)buf->frame4_a, 0x30);
	XAccel_Set_Val(slot, (u64)buf->frame5_a, 0x38);
	XAccel_Set_Val(slot, (u64)buf->gradient_z, 0x40);
	XHls_ip_Start(slot);
}

void set_params_gradient_weight_y(XHls_ip* slot, buffers_optical_flow* buf){
#ifdef DEBUG
	xil_printf("Passing Parameters set_params_gradient_weight_y\r\n");
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)buf->gradient_x, 0x18);
	XAccel_Set_Val(slot, (u64)buf->gradient_y, 0x20);
	XAccel_Set_Val(slot, (u64)buf->gradient_z, 0x28);
	XAccel_Set_Val(slot, (u64)buf->y_filtered_x, 0x30);
	XAccel_Set_Val(slot, (u64)buf->y_filtered_y, 0x38);
	XAccel_Set_Val(slot, (u64)buf->y_filtered_z, 0x40);
	XAccel_Set_Val(slot, (u64)buf->y_filtered_pad, 0x48);
	XHls_ip_Start(slot);
}

void set_params_gradient_weight_x(XHls_ip* slot, buffers_optical_flow* buf){
#ifdef DEBUG
	xil_printf("Passing Parameters set_params_gradient_weight_x\r\n");
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)buf->y_filtered_x, 0x18);
	XAccel_Set_Val(slot, (u64)buf->y_filtered_y, 0x20);
	XAccel_Set_Val(slot, (u64)buf->y_filtered_z, 0x28);
	XAccel_Set_Val(slot, (u64)buf->y_filtered_pad, 0x30);
	XAccel_Set_Val(slot, (u64)buf->filtered_gradient_x, 0x38);
	XAccel_Set_Val(slot, (u64)buf->filtered_gradient_y, 0x40);
	XAccel_Set_Val(slot, (u64)buf->filtered_gradient_z, 0x48);
	XAccel_Set_Val(slot, (u64)buf->filtered_gradient_pad, 0x50);
	XHls_ip_Start(slot);
}

void set_params_outer_product(XHls_ip* slot, buffers_optical_flow* buf){
#ifdef DEBUG
	xil_printf("Passing Parameters set_params_outer_product\r\n");
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)buf->filtered_gradient_x, 0x18);
	XAccel_Set_Val(slot, (u64)buf->filtered_gradient_y, 0x20);
	XAccel_Set_Val(slot, (u64)buf->filtered_gradient_z, 0x28);
	XAccel_Set_Val(slot, (u64)buf->filtered_gradient_pad, 0x30);
	XAccel_Set_Val(slot, (u64)buf->out_product, 0x38);
	XHls_ip_Start(slot);
}

void set_params_tensor_weight_y(XHls_ip* slot, buffers_optical_flow* buf){
#ifdef DEBUG
	xil_printf("Passing Parameters set_params_tensor_weight_y\r\n");
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)buf->out_product, 0x18);
	XAccel_Set_Val(slot, (u64)buf->tensor_y, 0x20);
	XHls_ip_Start(slot);
}

void set_params_tensor_weight_x(XHls_ip* slot, buffers_optical_flow* buf){
#ifdef DEBUG
	xil_printf("Passing Parameters set_params_tensor_weight_x\r\n");
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)buf->tensor_y, 0x18);
	XAccel_Set_Val(slot, (u64)buf->tensor, 0x20);
	XHls_ip_Start(slot);
}

void set_params_flow_calc(XHls_ip* slot, buffers_optical_flow* buf){
#ifdef DEBUG
	xil_printf("Passing Parameters set_params_flow_calc\r\n");
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)buf->tensor, 0x18);
	XAccel_Set_Val(slot, (u64)buf->outputs_x, 0x20);
	XAccel_Set_Val(slot, (u64)buf->outputs_y, 0x28);
	XHls_ip_Start(slot);
}


void set_params_convolution1(XHls_ip* slot, buffers_alexnet* buf, int i){
#ifdef DEBUG
	xil_printf("Passing Parameters set_params_convolution1 %d\r\n", i);
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)buf->image, 0x18);
	XAccel_Set_Val(slot, (u64)buf->conv1_weights, 0x20);
	XAccel_Set_Val(slot, (u64)buf->conv1_bias, 0x28);
	XAccel_Set_Val(slot, (u64)buf->pool1_rst, 0x30);
	XAccel_Set_Val(slot, (u64)(i*ALEXNET_CONV1_OUTPUT/ALEXNET_CONV1_SLOTS), 0x38);
	XHls_ip_Start(slot);
}

void set_params_convolution2_a(XHls_ip* slot, buffers_alexnet* buf, int i){
#ifdef DEBUG
	xil_printf("Passing Parameters set_params_convolution2_a\r\n");
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)buf->pool1_rst, 0x18);
	XAccel_Set_Val(slot, (u64)buf->conv2_weights, 0x20);
	XAccel_Set_Val(slot, (u64)buf->conv2_bias, 0x28);
	XAccel_Set_Val(slot, (u64)buf->pool2_rst, 0x30);
	XAccel_Set_Val(slot, (u64)(i*ALEXNET_CONV2_OUTPUT/ALEXNET_CONV2_SLOTS), 0x38);
	XAccel_Set_Val(slot, (u64)0, 0x40);
	XHls_ip_Start(slot);
}

void set_params_convolution2_b(XHls_ip* slot, buffers_alexnet* buf, int i){
#ifdef DEBUG
	xil_printf("Passing Parameters set_params_convolution2_b\r\n");
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)buf->pool1_rst, 0x18);
	XAccel_Set_Val(slot, (u64)buf->conv2_weights, 0x20);
	XAccel_Set_Val(slot, (u64)buf->conv2_bias, 0x28);
	XAccel_Set_Val(slot, (u64)buf->pool2_rst, 0x30);
	XAccel_Set_Val(slot, (u64)(i*ALEXNET_CONV2_OUTPUT/ALEXNET_CONV2_SLOTS)+128, 0x38);
	XAccel_Set_Val(slot, (u64)48, 0x40);
	XHls_ip_Start(slot);
}

void set_params_convolution3(XHls_ip* slot, buffers_alexnet* buf, int i){
#ifdef DEBUG
	xil_printf("Passing Parameters set_params_convolution3\r\n");
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)buf->pool2_rst, 0x18);
	XAccel_Set_Val(slot, (u64)buf->conv3_weights, 0x20);
	XAccel_Set_Val(slot, (u64)buf->conv3_bias, 0x28);
	XAccel_Set_Val(slot, (u64)buf->conv3_rst, 0x30);
	XAccel_Set_Val(slot, (u64)(i*ALEXNET_CONV3_OUTPUT/ALEXNET_CONV3_SLOTS), 0x38);
	XHls_ip_Start(slot);
}

void set_params_convolution4_a(XHls_ip* slot, buffers_alexnet* buf, int i){
#ifdef DEBUG
	xil_printf("Passing Parameters set_params_convolution4_a\r\n");
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)buf->conv3_rst, 0x18);
	XAccel_Set_Val(slot, (u64)buf->conv4_weights, 0x20);
	XAccel_Set_Val(slot, (u64)buf->conv4_bias, 0x28);
	XAccel_Set_Val(slot, (u64)buf->conv4_rst, 0x30);
	XAccel_Set_Val(slot, (u64)(i*ALEXNET_CONV4_OUTPUT/ALEXNET_CONV4_SLOTS), 0x38);
	XAccel_Set_Val(slot, (u64)0, 0x40);
	XHls_ip_Start(slot);
}

void set_params_convolution4_b(XHls_ip* slot, buffers_alexnet* buf, int i){
#ifdef DEBUG
	xil_printf("Passing Parameters set_params_convolution4_b\r\n");
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)buf->conv3_rst, 0x18);
	XAccel_Set_Val(slot, (u64)buf->conv4_weights, 0x20);
	XAccel_Set_Val(slot, (u64)buf->conv4_bias, 0x28);
	XAccel_Set_Val(slot, (u64)buf->conv4_rst, 0x30);
	XAccel_Set_Val(slot, (u64)(i*ALEXNET_CONV4_OUTPUT/ALEXNET_CONV4_SLOTS), 0x38);
	XAccel_Set_Val(slot, (u64)192, 0x40);
	XHls_ip_Start(slot);
}

void set_params_convolution5_a(XHls_ip* slot, buffers_alexnet* buf, int i){
#ifdef DEBUG
	xil_printf("Passing Parameters set_params_convolution5_a\r\n");
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)buf->conv4_rst, 0x18);
	XAccel_Set_Val(slot, (u64)buf->conv5_weights, 0x20);
	XAccel_Set_Val(slot, (u64)buf->conv5_bias, 0x28);
	XAccel_Set_Val(slot, (u64)buf->pool5_rst, 0x30);
	XAccel_Set_Val(slot, (u64)(i*ALEXNET_CONV5_OUTPUT/ALEXNET_CONV5_SLOTS), 0x38);
	XAccel_Set_Val(slot, (u64)0, 0x40);
	XHls_ip_Start(slot);
}

void set_params_convolution5_b(XHls_ip* slot, buffers_alexnet* buf, int i){
#ifdef DEBUG
	xil_printf("Passing Parameters set_params_convolution5_b\r\n");
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)buf->conv4_rst, 0x18);
	XAccel_Set_Val(slot, (u64)buf->conv5_weights, 0x20);
	XAccel_Set_Val(slot, (u64)buf->conv5_bias, 0x28);
	XAccel_Set_Val(slot, (u64)buf->pool5_rst, 0x30);
	XAccel_Set_Val(slot, (u64)(i*ALEXNET_CONV5_OUTPUT/ALEXNET_CONV5_SLOTS)+128, 0x38);
	XAccel_Set_Val(slot, (u64)192, 0x40);
	XHls_ip_Start(slot);
}

void set_params_fullconnection6(XHls_ip* slot, buffers_alexnet* buf, int i){
#ifdef DEBUG
	xil_printf("Passing Parameters set_params_fullconnection6\r\n");
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)buf->pool5_rst, 0x18);
	XAccel_Set_Val(slot, (u64)buf->fc6_weights, 0x20);
	XAccel_Set_Val(slot, (u64)buf->fc6_bias, 0x28);
	XAccel_Set_Val(slot, (u64)buf->fc6_rst, 0x30);
	XAccel_Set_Val(slot, (u64)(i*ALEXNET_FC6_OUTPUT/ALEXNET_FC6_SLOTS), 0x38);
	XHls_ip_Start(slot);
}

void set_params_fullconnection7(XHls_ip* slot, buffers_alexnet* buf, int i){
#ifdef DEBUG
	xil_printf("Passing Parameters set_params_fullconnection7\r\n");
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)buf->fc6_rst, 0x18);
	XAccel_Set_Val(slot, (u64)buf->fc7_weights, 0x20);
	XAccel_Set_Val(slot, (u64)buf->fc7_bias, 0x28);
	XAccel_Set_Val(slot, (u64)buf->fc7_rst, 0x30);
	XAccel_Set_Val(slot, (u64)(i*ALEXNET_FC7_OUTPUT/ALEXNET_FC7_SLOTS), 0x38);
	XHls_ip_Start(slot);
}

void set_params_fullconnection8(XHls_ip* slot, buffers_alexnet* buf, int i){
#ifdef DEBUG
	xil_printf("Passing Parameters set_params_fullconnection8\r\n");
#endif
	//XHls_ip_IsDone(slot);
	XAccel_Set_Val(slot, (u64)buf->fc7_rst, 0x18);
	XAccel_Set_Val(slot, (u64)buf->fc8_weights, 0x20);
	XAccel_Set_Val(slot, (u64)buf->fc8_bias, 0x28);
	XAccel_Set_Val(slot, (u64)buf->fc8_rst, 0x30);
	XAccel_Set_Val(slot, (u64)(i*ALEXNET_FC8_OUTPUT/ALEXNET_FC8_SLOTS), 0x38);
	XHls_ip_Start(slot);
	//xil_printf("Parameters Passed\r\n");
}

void init_img_cmp_data(int max_height, int max_width) {
#ifdef DEBUG
	xil_printf("Generating random %d by %d test image...\r\n", W, H);
#endif

	srand(0);
	float rand_0;

	for(int batch = 0; batch < BATCH_SIZE_IMG_CMP; batch++) {
		for(int h = 0; h < max_height; h++) {
			rand_0 = rand() * 254.0 / RAND_MAX + 1;
			for(int w = 0; w < max_width; w++) {
				img_cmp_src[batch][h * max_width + w] = 0.5;
				img_cmp_output_golden[batch][h * max_width + w] = 0.5;
				img_cmp_dst[batch][h * max_width + w] = 0.0f;
			}
		}
	}
}

void initialize_img_weights_bias() {

	for(int batch = 0; batch < BATCH_SIZE_IMG_CMP; batch++) {
		for (int i = 0; i < 32; i++){
			for (int j = 0; j < 32; j++){
				image[i][j] = (DTYPE)(j);
			}
		}
		for (int i = 0; i < 6; i++){
			for (int j = 0; j < 14; j++){
				for (int k = 0; k < 14; k++){
					pool2_output[batch][i][j][k] = 0.0;
				}
			}
		}
		for (int i = 0; i < 16; i++){
			for (int j = 0; j < 5; j++){
				for (int k = 0; k < 5; k++){
					pool4_output[batch][i][j][k] = 0.0;
				}
			}
		}
		for (int i = 0; i < 10; i++){
			fc6_output[i] = 0.0;
		}
	}


	for (int i = 0; i < 10; i++){
		fc6_bias[i] = (DTYPE)(i);
		for (int j = 0; j < 120; j++){
			fc6_weights[i][j] = (DTYPE)(j);
		}
	}
	for (int i = 0; i < 6; i++){
		conv1_bias[i] = (DTYPE)(i);
	}
	for (int i = 0; i < 16; i++){
		conv3_bias[i] = (DTYPE)(i);
	}

	for (int i = 0; i < 6; i++){
		for (int j = 0; j < 5; j++){
			for (int k = 0; k < 5; k++){
				conv1_weights[i][j][k] = (DTYPE)(k);//(DTYPE)(rand() % 255);
			}
		}
	}
	for (int i = 0; i < 16; i++){
		for (int j = 0; j < 6; j++){
			for (int k = 0; k < 5; k++){
				for (int c = 0; c < 5; c++){
					conv3_weights[i][j][k][c] = (DTYPE)(c);//(DTYPE)(rand() % 255);
				}
			}
		}
	}
	for (int i = 0; i < 120; i++){
		for (int j = 0; j < 16; j++){
			for (int k = 0; k < 5; k++){
				for (int c = 0; c < 5; c++){
					conv5_weights[i][j][k][c] = (DTYPE)(c);//(DTYPE)(rand() % 255);
				}
			}
		}
	}

#ifdef DEBUG
	xil_printf("Lenet array addresses:\r\n");
	xil_printf("image is at %p\r\n", image);
	xil_printf("conv1_weights is at %p\r\n", conv1_weights);
	xil_printf("conv1_bias is at %p\r\n", conv1_bias);
	xil_printf("pool2_output is at %p\r\n", pool2_output);
	xil_printf("conv3_weights is at %p\r\n", conv3_weights);
	xil_printf("conv3_bias is at %p\r\n", conv3_bias);
	xil_printf("pool4_output is at %p\r\n", pool4_output);
	xil_printf("conv5_weights is at %p\r\n", conv5_weights);
	xil_printf("conv5_bias is at %p\r\n", conv5_bias);
	xil_printf("fc6_bias is at %p\r\n", fc6_bias);
	xil_printf("fc6_weights is at %p\r\n", fc6_weights);
	xil_printf("fc6_output is at %p\r\n", fc6_output);
#endif
}

// Parse MNIST test images
int parse_mnist_images(char* filename, unsigned char *images)
{
	FRESULT Res;
	UINT NumBytesRead;
    SD_File = (char *)filename;
    unsigned int header[4];

	Res = f_open(&fil, SD_File, FA_READ);
	if (Res)
	{
		xil_printf("ERROR when opening mnist images data file!\n\r");
		return XST_FAILURE;
	}

	Res = f_lseek(&fil, 0);
	if (Res)
	{
		xil_printf("Cant seek to start\n\r");
		return XST_FAILURE;
	}

	Res = f_read(&fil, (void*)header, sizeof(unsigned int)*4, &NumBytesRead);
	if (Res)
	{
		xil_printf("Cant read header from file\n\r");
		return XST_FAILURE;
	}

	Res = f_read(&fil, (void*)images, sizeof(unsigned char)*LENET_NUM_TEST*28*28, &NumBytesRead);
	if (Res)
	{
		xil_printf("Cant read images from file\n\r");
		return XST_FAILURE;
	}

	Res = f_close(&fil);
	if (Res)
	{
		xil_printf("Failed to close images file\n\r");
		return XST_FAILURE;
	}

	xil_printf("Returning...\n\r");


	return XST_SUCCESS;
}

// Parse MNIST test image labels
int parse_mnist_labels(char* filename, unsigned char *labels)
{
	FRESULT Res;
	UINT NumBytesRead;
	SD_File = (char *)filename;
    unsigned int header[2];

	Res = f_open(&fil, SD_File, FA_READ);
	if (Res)
	{
		xil_printf("ERROR when opening mnist label data file!\n\r");
		return XST_FAILURE;
	}

	Res = f_lseek(&fil, 0);
	if (Res)
	{
		xil_printf("Cant seek to start\n\r");
		return XST_FAILURE;
	}

	Res = f_read(&fil, (void*)header, sizeof(unsigned int)*2, &NumBytesRead);
	if (Res)
	{
		xil_printf("Cant read header from file\n\r");
		return XST_FAILURE;
	}

	Res = f_read(&fil, (void*)labels, sizeof(unsigned char)*LENET_NUM_TEST, &NumBytesRead);
	if (Res)
	{
		xil_printf("Cant read labels from file\n\r");
		return XST_FAILURE;
	}

	Res = f_close(&fil);
	if (Res)
	{
		xil_printf("Failed to close labels file\n\r");
		return XST_FAILURE;
	}

	return 0;
}


// Parse parameter file and load it in to the arrays
int parse_parameters(char* filename) {
	FRESULT Res;
	UINT NumBytesRead;
    SD_File = (char *)filename;

	Res = f_open(&fil, SD_File, FA_READ);
	if (Res)
	{
		xil_printf("ERROR when opening parameter file!\n\r");
		return XST_FAILURE;
	}

	Res = f_lseek(&fil, 0);
	if (Res)
	{
		xil_printf("Cant seek to start\n\r");
		return XST_FAILURE;
	}

	Res = f_read(&fil, (void*)**conv1_weights, sizeof(float)*150, &NumBytesRead);
	if (Res)
	{
		xil_printf("Cant read conv1_weights from file\n\r");
		return XST_FAILURE;
	}

	Res = f_read(&fil, (void*)conv1_bias, sizeof(float)*6, &NumBytesRead);
	if (Res)
	{
		xil_printf("Cant read conv1_bias from file\n\r");
		return XST_FAILURE;
	}

	Res = f_read(&fil, (void*)***conv3_weights, sizeof(float)*2400, &NumBytesRead);
	if (Res)
	{
		xil_printf("Cant read conv3_weights from file\n\r");
		return XST_FAILURE;
	}

	Res = f_read(&fil, (void*)conv3_bias, sizeof(float)*16, &NumBytesRead);
	if (Res)
	{
		xil_printf("Cant read conv3_bias from file\n\r");
		return XST_FAILURE;
	}

	Res = f_read(&fil, (void*)***conv5_weights, sizeof(float)*48000, &NumBytesRead);
	if (Res)
	{
		xil_printf("Cant read conv5_weights from file\n\r");
		return XST_FAILURE;
	}

	Res = f_read(&fil, (void*)conv5_bias, sizeof(float)*120, &NumBytesRead);
	if (Res)
	{
		xil_printf("Cant read conv5_bias from file\n\r");
		return XST_FAILURE;
	}

	Res = f_read(&fil, (void*)*fc6_weights, sizeof(float)*1200, &NumBytesRead);
	if (Res)
	{
		xil_printf("Cant read fc6_weights from file\n\r");
		return XST_FAILURE;
	}

	Res = f_read(&fil, (void*)fc6_bias, sizeof(float)*10, &NumBytesRead);
	if (Res)
	{
		xil_printf("Cant read fc6_bias from file\n\r");
		return XST_FAILURE;
	}

	Res = f_close(&fil);
	if (Res)
	{
		xil_printf("Failed to close labels file\n\r");
		return XST_FAILURE;
	}

	return 0;

}

void lenet_get_image(unsigned char *images, unsigned int idx, volatile float image[32][32])
{
    for(int i = 0; i < 32; i++)
        for(int j = 0; j < 32; j++)
        {
            if (i < 2 || i > 29 || j < 2 || j > 29)
                image[i][j] = -1.0;
            else
                image[i][j] = images[idx*28*28 + (i-2)*28 + j-2] / 255.0 * 2.0 - 1.0;
        }
}

void print_conv1_weights() {
        for(int i = 0; i < 5; i++) {
                printf("conv1_weights[0][0][%d] = %f\r\n", i, conv1_weights[0][0][i]);
        }
}

void print_conv1_bias() {
        for(int i = 0; i < 6; i++) {
                printf("conv1_bias[%d] = %f\r\n", i, conv1_bias[i]);
        }
}


void print_img(int batch_num) {
        for(int i = 0; i < 10; i++) {
                printf("image[%d][0][%d] = %f\r\n", batch_num, i, image[0][i]);
        }
}

void print_pool2(int batch_num) {
        for(int i = 0; i < 14; i++) {
                printf("pool2_output[%d][0][0][%d] = %f\r\n", batch_num, i, pool2_output[batch_num][0][0][i]);
        }
}

void print_pool4(int batch_num) {
        for(int i = 0; i < 5; i++) {
                printf("pool2_output[%d][0][0][%d] = %f\r\n", batch_num, i, pool4_output[batch_num][0][0][i]);
        }
}

void print_fc6(int batch_num) {
        for(int i = 0; i < 10; i++) {
                printf("fc6_output[%d] = %f\r\n", i, fc6_output[i]);
        }
}

void print_img_cmp_data(int batch) {
        for(int i = 0; i < 10; i++) {
                printf("src[%d][%d] = %f\r\n", batch, i, img_cmp_src[batch][i]);
        }
        for(int i = 0; i < 10; i++) {
                printf("dst[%d][%d] = %f\r\n", batch, i, img_cmp_dst[batch][i]);
        }
}

void print_img_cmp_addresses() {
        xil_printf("src is at %p\r\n", img_cmp_src);
        xil_printf("dst is at %p\r\n", img_cmp_dst);

}

#include "math.h"
#include "ff.h"
//read function
void readimage(const char FILname[], float image[3][227][227])
{
	FIL fin;
	UINT bytes_read;
	f_open(&fin, FILname , FA_READ);

	for(int c = 0; c < 3; ++c){
		for(int h = 0; h < 227; ++h){
			f_read(&fin, image[c][h], sizeof(float)*227, &bytes_read);
		}
	}
	f_close(&fin);
	return;
}

void readweights(const char ss[], float weights[96][3][11][11], float bias[96])
{
    FIL fin;
    UINT bytes_read;
    f_open(&fin, ss, FA_READ);

    for(int n=0;n<96;n++)
        for(int j=0;j<3;j++)
            for(int k=0;k<11;k++)
            	f_read(&fin, weights[n][j][k], sizeof(float)*11, &bytes_read);
    f_read(&fin, bias, sizeof(float)*96, &bytes_read);
    f_close(&fin);

}

void readweights2(const char ss[], float weights[256][48][5][5] , float bias[256])
{
	FIL fin;
	UINT bytes_read;
	f_open(&fin, ss, FA_READ);

    for(int n=0;n<256;n++)
        for(int j=0;j<48;j++)
            for(int k=0;k<5;k++)
    			f_read(&fin, weights[n][j][k], sizeof(float)*5, &bytes_read);
    f_read(&fin, bias, sizeof(float)*256, &bytes_read);
    f_close(&fin);
	return;
}

void readweights3(const char ss[], float weights[384][256][3][3] , float bias[384])
{
	FIL fin;
	UINT bytes_read;
	f_open(&fin, ss, FA_READ);

    for(int n=0;n<384;n++)
        for(int j=0;j<256;j++)
            for(int k=0;k<3;k++)
            	f_read(&fin, weights[n][j][k], sizeof(float)*3, &bytes_read);
    f_read(&fin, bias, sizeof(float)*384, &bytes_read);
    f_close(&fin);
	return;
}

void readweights4(const char ss[], float weights[384][192][3][3] , float bias[384])
{
	FIL fin;
	UINT bytes_read;
	f_open(&fin, ss, FA_READ);

    for(int n=0;n<384;n++)
        for(int j=0;j<192;j++)
            for(int k=0;k<3;k++)
            	f_read(&fin, weights[n][j][k], sizeof(float)*3, &bytes_read);
    f_read(&fin, bias, sizeof(float)*384, &bytes_read);
    f_close(&fin);
	return;
}

void readweights5(const char ss[], float weights[256][192][3][3] , float bias[256])
{
	FIL fin;
	UINT bytes_read;
	f_open(&fin, ss, FA_READ);

    for(int n=0;n<256;n++)
        for(int j=0;j<192;j++)
            for(int k=0;k<3;k++)
            	f_read(&fin, weights[n][j][k], sizeof(float)*3, &bytes_read);
    f_read(&fin, bias, sizeof(float)*256, &bytes_read);
    f_close(&fin);
	return;
}

void readweights6(const char ss[], float weights[256][256][6][6] , float bias[256])
{
    FIL fin;
    UINT bytes_read;
    f_open(&fin, ss, FA_READ);
	for(int i=0;i<256;i++)
        for(int j=0;j<256;j++)
            for(int k=0;k<6;k++)
            	f_read(&fin, weights[i][j][k], sizeof(float)*6, &bytes_read);
	f_read(&fin, bias, sizeof(float)*256, &bytes_read);
    f_close(&fin);
    return;
}

void readweights7(const char ss[], float weights[256][256] , float bias[256])
{
    FIL fin;
    UINT bytes_read;
    f_open(&fin, ss, FA_READ);

	for(int i=0;i<256;i++)
		f_read(&fin, weights[i], sizeof(float)*256, &bytes_read);

	f_read(&fin, bias, sizeof(float)*256, &bytes_read);
    f_close(&fin);
    return;
}

void readweights8(const char ss[], float weights[1000][256] , float bias[1000])
{
    FIL fin;
    UINT bytes_read;
    f_open(&fin, ss, FA_READ);

	for(int i=0;i<1000;i++)
		f_read(&fin, weights[i], sizeof(float)*256, &bytes_read);

	f_read(&fin, bias, sizeof(float)*1000, &bytes_read);
    f_close(&fin);
    return;
}

void softmax(float *input, double *output, int N)
{
	double max=0.00f, sum=0.00f;

	for(int i=0; i<N; i++)
	{
		double data = (double)input[i];
		if(data > max)
			max = data;
	}

	for(int i=0; i<N; i++)
	{
		double data = (double)input[i];
		double partial = data/max;
		sum += partial;
	}

	for(int i=0; i<N; i++)
	{
		double data = (double)input[i];
		output[i] = exp(data/max)/exp(sum);
	}

}


void run_pr_flow(int total_num_ips) {
     int i;
     struct ip* next_ip = &ip_list[0][0]; // Start with the first IP of the first application
     int flag = 0;
     int all_done = 0;
     int next_app;

     while(!all_done) {
    	 flag = 0;

		 for (i = 0; i < NUM_SLOTS; i++){
			 if (i != slots[i].id){
				 xil_printf("Error! Slot ID mismatch!! Should be %d but is %d!\r\n", i, slots[i].id);
				 return;
			 }
			 switch (slots[i].status) {
				 case IDLE:
					 break;
				 case PR_READY:
#ifdef DEBUG
					 xil_printf("Slot %d in PR_READY\n\r", i);
#endif
					 // Recording start time of PR
					 XTime_GetTime(&(slots[i].cur->tStart_pr));

					 // Starting PR
					 XGpioPs_WritePin(&psGpioInstancePtr,slots[i].cur->decouple, 1);
					 status = XFpga_PL_BitStream_Load(&XFpgaInstance, (UINTPTR)slots[i].cur->BitAddr, slots[i].cur->Bitlen, XFPGA_PARTIAL_EN);
					 // Recording end of PR time
					 XTime_GetTime(&(slots[i].cur->tEnd_pr));
#ifdef DEBUG
					 xil_printf("Configuring Configuring slot %d to app %d IP %d with Bitstream %s of size %d and address %p\n\r", i, slots[i].cur->app_id, slots[i].cur->id, slots[i].cur->bin_name, slots[i].cur->Bitlen, (UINTPTR)slots[i].cur->BitAddr);
#endif
					 if(status != XST_SUCCESS) {
						 xil_printf("[ERROR] Configuring slot %d to app %d IP %d failed\n\r", i, slots[i].cur->app_id, slots[i].cur->id);
						 return;
					 }

					 // Resseting slot and turn decouple off
					 XGpioPs_WritePin(&psGpioInstancePtr, slots[i].cur->decouple, 0);
					 XGpioPs_WritePin(&psGpioInstancePtr, slots[i].cur->reset, 0);
					 usleep(100);
					 XGpioPs_WritePin(&psGpioInstancePtr, slots[i].cur->reset, 1);
					 slots[i].status = WAITING;

					 // Recording start time
					 XTime_GetTime(&(slots[i].cur->tStart_wait));
					 break;
				 case WAITING:

					 // Getting the current time
					 XTime_GetTime(&cur_time);

					 // Running if dependencies are met and scheduler has scheduled it
					 if (ip_check_dep(slots[i].cur) && ((float)(slots[i].cur->run_time * TIME_STEP) < 1.0 * (cur_time - first_pr_time) / (COUNTS_PER_SECOND / 1000000))) {
#ifdef DEBUG
					 	 xil_printf("Slot %d has no dependencies and will start\n\r", i);
#endif

                         // If first of batch end waiting time and start running time
                         if(slots[i].batch_count == 0) {
                        	 XTime_GetTime(&(slots[i].cur->tEnd_wait));
                        	 XTime_GetTime(&(slots[i].cur->tStart_run));
                         }

                         if(slots[i].cur->app_id == 0) {
                        	 switch (slots[i].cur->kern){
                        	 	 case 0:
                        	 		 set_params_and_run_bundle1(&(slots[i].accelerator), buf_3dr[slots[i].batch_count]);
                        	 		 break;
                        	 	 case 1:
                        	 		 set_params_and_run_zculling(&(slots[i].accelerator), buf_3dr[slots[i].batch_count]);
                        	 		 break;
                        	 	 case 2:
                        	 		 set_params_and_run_bundle2(&(slots[i].accelerator), buf_3dr[slots[i].batch_count]);
                        	 		 break;
                        	 }
                         }
                         else if(slots[i].cur->app_id == 1) {
                        	 switch (slots[i].cur->kern){
                        	 	 case 0:
                        	 		 lenet_get_image(lenet_images, slots[i].batch_count, image);
                        	 		 set_params_and_run_layer_conv1_pool2(&(slots[i].accelerator), image, pool2_output[slots[i].batch_count]);
                        	 		 break;
                        	 	 case 1:
                        	 		 set_params_and_run_layer_conv3_pool4(&(slots[i].accelerator), pool2_output[slots[i].batch_count], pool4_output[slots[i].batch_count]);
                        	 		 break;
                        	 	 case 2:
                        	 		 set_params_and_run_layer_conv5_fc6(&(slots[i].accelerator), pool4_output[slots[i].batch_count], fc6_output);
                        	 		 break;
                        	 }
                         }
//                         else if(slots[i].cur->app_id == 2) {
//                        	 switch (slots[i].cur->kern){
//                        	 	 case 0:
//                        	 		 set_params_and_run_knn_init(&(slots[i].accelerator), buf_knn[slots[i].batch_count]);
//                        	 		 break;
//                        	 	 case 1:
//                        	 		 set_params_and_run_knn_train(&(slots[i].accelerator), buf_knn[slots[i].batch_count]);
//                        	 		 break;
//                        	 	 case 2:
//                        	 		 set_params_and_run_knn_pred(&(slots[i].accelerator), buf_knn[slots[i].batch_count]);
//                        	 		 break;
//                        	 }
//                         }
//                         else if(slots[i].cur->app_id == 3) {
//                        	 switch (slots[i].cur->kern){
//                        	 	 case 0:
//                        	 		 set_params_and_run_bundle1_module(&(slots[i].accelerator), slots[i].batch_count);
//                        	 		 break;
//                        	 	 case 1:
//                        	 		 set_params_and_run_dct(&(slots[i].accelerator), slots[i].batch_count);
//                        	 		 break;
//                        	 	 case 2:
//                        	 		 set_params_and_run_quantize(&(slots[i].accelerator), slots[i].batch_count);
//                        	 		 break;
//                        	 	 case 3:
//                        	 		 set_params_and_run_dequantize(&(slots[i].accelerator), slots[i].batch_count);
//                        	 		 break;
//                        	 	 case 4:
//                        	 		 set_params_and_run_idct(&(slots[i].accelerator), slots[i].batch_count);
//                        	 		 break;
//                        	 	 case 5:
//                        	 		 set_params_and_run_reconstructBlockD(&(slots[i].accelerator), slots[i].batch_count);
//                        	 		 break;
//                        	 }
//                         }
						 else if(slots[i].cur->app_id == 2) {
							 switch (slots[i].cur->kern){
								 case 0:
									 set_params_read_input(&(slots[i].accelerator), &buf_optical_flow[slots[i].batch_count]);
									 break;
								 case 1:
									 set_params_gradient_xy_calc(&(slots[i].accelerator), &buf_optical_flow[slots[i].batch_count]);
									 break;
								 case 2:
									 set_params_gradient_z_calc(&(slots[i].accelerator), &buf_optical_flow[slots[i].batch_count]);
									 break;
								 case 3:
									 set_params_gradient_weight_y(&(slots[i].accelerator), &buf_optical_flow[slots[i].batch_count]);
									 break;
								 case 4:
									 set_params_gradient_weight_x(&(slots[i].accelerator), &buf_optical_flow[slots[i].batch_count]);
									 break;
								 case 5:
									 set_params_outer_product(&(slots[i].accelerator), &buf_optical_flow[slots[i].batch_count]);
									 break;
								 case 6:
									 set_params_tensor_weight_y(&(slots[i].accelerator), &buf_optical_flow[slots[i].batch_count]);
									 break;
								 case 7:
									 set_params_tensor_weight_x(&(slots[i].accelerator), &buf_optical_flow[slots[i].batch_count]);
									 break;
								 case 8:
									 set_params_flow_calc(&(slots[i].accelerator), &buf_optical_flow[slots[i].batch_count]);
									 break;
							 }
						 }
//						 else if(slots[i].cur->app_id == 3) {
//							 switch (slots[i].cur->kern){
//								 case 0: case 1: case 2: case 3:
//									 set_params_convolution1(&(slots[i].accelerator), &buf_alexnet[slots[i].batch_count], slots[i].cur->kern);
//									 break;
//								 case 4: case 5: case 6: case 7:
//									 set_params_convolution2_a(&(slots[i].accelerator), &buf_alexnet[slots[i].batch_count], slots[i].cur->kern % 4);
//									 break;
//								 case 8: case 9: case 10: case 11:
//									 set_params_convolution2_b(&(slots[i].accelerator), &buf_alexnet[slots[i].batch_count], slots[i].cur->kern % 4);
//									 break;
//								 case 12: case 13: case 14: case 15:
//									 set_params_convolution3(&(slots[i].accelerator), &buf_alexnet[slots[i].batch_count], slots[i].cur->kern % 4);
//									 break;
//								 case 16: case 17: case 18: case 19:
//									 set_params_convolution4_a(&(slots[i].accelerator), &buf_alexnet[slots[i].batch_count], slots[i].cur->kern % 4);
//									 break;
//								 case 20: case 21: case 22: case 23:
//									 set_params_convolution4_b(&(slots[i].accelerator), &buf_alexnet[slots[i].batch_count], slots[i].cur->kern % 4);
//									 break;
//								 case 24: case 25: case 26: case 27:
//									 set_params_convolution5_a(&(slots[i].accelerator), &buf_alexnet[slots[i].batch_count], slots[i].cur->kern % 4);
//									 break;
//								 case 28: case 29: case 30: case 31:
//									 set_params_convolution5_b(&(slots[i].accelerator), &buf_alexnet[slots[i].batch_count], slots[i].cur->kern % 4);
//									 break;
//								 case 32: case 33:
//									 set_params_fullconnection6(&(slots[i].accelerator), &buf_alexnet[slots[i].batch_count], slots[i].cur->kern % 2);
//									 break;
//								 case 34: case 35:
//									 set_params_fullconnection7(&(slots[i].accelerator), &buf_alexnet[slots[i].batch_count], slots[i].cur->kern % 2);
//									 break;
//								 case 36: case 37:
//									 set_params_fullconnection8(&(slots[i].accelerator), &buf_alexnet[slots[i].batch_count], slots[i].cur->kern % 2);
//									 break;
//							 }
//						 }
                         else {
                        	 xil_printf("Unknown app ID of %d\n\r", slots[i].cur->app_id);
                         }
                         slots[i].status = RUNNING;
					  }

					 break;
				 case RUNNING:
					 if (XHls_ip_IsDone(&(slots[i].accelerator))) {
#ifdef DEBUG
						 xil_printf("Slot %d is DONE\n\r", i);
#endif

						 // Need to post-process data from some kernels when they are done
						 if(slots[i].cur->app_id == 1 && slots[i].cur->kern == 2) {

							 int max_label = 0;
							 float max_val = -100.0;
							 for(int label = 0; label < 10; label++) {
								 if(fc6_output[label] > max_val) {
									 max_label = label;
									 max_val = fc6_output[label];
								 }
							 }

							 if(max_label == lenet_labels[slots[i].batch_count]) {
								 lenet_num_correct++;
							 }
#ifdef DEBUG
							 print_fc6(0);
							 xil_printf("Label: %d max_label: %d\n\r", lenet_labels[slots[i].batch_count], max_label);
#endif
						 }

						 // Runtime scheduling
						 slots[i].batch_count++;
						 slots[i].cur->num_invoke++;
						 batch_done[slots[i].cur->app_id][slots[i].cur->id]++;

                         // Checking if we are done with all batches
                         if (slots[i].batch_count >= slots[i].cur->batch) {
                        	 // Measuring latency of entire application and run time
                        	 XTime_GetTime(&(slots[i].cur->tEnd_ip));
                        	 XTime_GetTime(&(slots[i].cur->tEnd_run));

                        	 // Adding IP to done list and setting slot to IDLE
                        	 done_list[slots[i].cur->app_id][num_done[slots[i].cur->app_id]] = slots[i].cur->id;
                        	 num_done[slots[i].cur->app_id]++;
                        	 slots[i].cur = NULL;
                        	 slots[i].status = IDLE;
                        	 slots[i].batch_count = 0;

                         }
                         else {
                        	 slots[i].status = WAITING;
                         }
					 }
					 break;
				 default: break;
			 }
		 }

		 // Check if we are finished
#ifdef MULTI_APP
		 if (total_cur_progress >= total_num_ips) {
#else
		 if (total_cur_progress >= APP_NUM_IPS) {
#endif
			 // check if all slots have finished
			 for (i = 0; i < NUM_SLOTS; i++) {
					 if (slots[i].status != IDLE){
							 flag = 1;
							 break;
					 }
			 }
			 // all slots have finished and all PR have finished so we are done!
			 if (flag == 0) {
#ifdef DEBUG
				 xil_printf("Setting all_done high\r\n");
#endif
				 all_done = 1;
				 break;
			 }
         }
		 // there is no pr in progress, schedule next pr
		 else if (pr_in_progress == 0) {
#ifdef MULTI_APP
			 next_app = choose_next_app(total_cur_progress);
#else
			 next_app = APP_ID;
#endif
			 if(next_app != -1) {
				 next_ip = &ip_list[next_app][cur_progress[next_app]]; // round robin scheduling of applications
				 // slot is IDLE, we can schedule the PR here

				 // Getting the current time
				 XTime_GetTime(&cur_time);

				 // Scheduling if slot is zero and the scheduler schedules a PR
				 if (slots[next_ip->slot].status == IDLE && (total_cur_progress == 0 || (float)(next_ip->pr_time * TIME_STEP) < 1.0 * (cur_time - first_pr_time) / (COUNTS_PER_SECOND / 1000000))) {
#ifdef DEBUG
					 xil_printf("Starting IP %d in app %d in slot %d with progress %d\r\n", next_ip->id, next_ip->app_id, next_ip->slot, total_cur_progress);
#endif

					 // Getting time IP starts
					 XTime_GetTime(&(next_ip)->tStart_ip);

					 // Getting the first PR time
					 if(total_cur_progress == 0) {
						 XTime_GetTime(&first_pr_time);
					 }

					 // Setting the slot to PR_READY to start PR'ing the IP into the slot
					 slots[next_ip->slot].cur = next_ip;
					 slots[next_ip->slot].status = PR_READY;
					 cur_progress[next_app]++;
					 total_cur_progress++;
				 }
			 }
		 }
     }
}


int main() {

	init_platform();

	for(int i = 0; i < 5; i++) {
	  sleep(1);
	  xil_printf("%d\r\n", i);
	}


	// Flush and disable Data Cache
	Xil_DCacheDisable();

	// Initialize SD controller and transfer partials to DDR
	SD_Init();

	// Configure GPIO pins
	init_gpio();

	// Initialize applications
	status = apps_init();
	if(status == XST_FAILURE) {
		return XST_FAILURE;
	}

	// Initialize XFpga object
	status = XFpga_Initialize(&XFpgaInstance);
	if(status != XST_SUCCESS) {
		xil_printf("[ERROR] Failed to initialize XFpgaInstance\n\r");
	}

	// Initialize schedule components
	sched_init(slots, ip_list);

	// Loading binary files into DRAM
	load_bins();

    // Initialize KNN data
	print("Initializing KNN buffers.\n\r");
	for(int batch = 0; batch < BATCH_SIZE_KNN; batch++) {
		buf_knn[batch].training_set = (WholeDigitType*)training_set_global[batch];
		if(buf_knn[batch].training_set == NULL) print("training malloc failed\n\r");
		buf_knn[batch].testing_set = (WholeDigitType*)testing_set_global[batch];
		if(buf_knn[batch].testing_set == NULL) print("testing malloc failed\n\r");
		buf_knn[batch].knn_set = (int*)knn_set_global[batch];
		if(buf_knn[batch].knn_set == NULL) print("knn malloc failed\n\r");
		buf_knn[batch].results = (LabelType*)results_global[batch];

#ifdef DEBUG
		xil_printf("training_set addr: %p\n\r", buf_knn[batch].training_set);
		xil_printf("testing_set addr unaligned: %p\n\r", buf_knn[batch].testing_set);
		xil_printf("knn_set addr unaligned: %p\n\r", buf_knn[batch].knn_set);
		xil_printf("results addr unaligned: %p\n\r", buf_knn[batch].results);
#endif

		// pack the data into a wide datatype
		for (int i = 0; i < NUM_TRAINING; i ++ )
		{
			buf_knn[batch].training_set[i].range(63 , 0  ) = training_data[i*DIGIT_WIDTH+0];
			buf_knn[batch].training_set[i].range(127, 64 ) = training_data[i*DIGIT_WIDTH+1];
			buf_knn[batch].training_set[i].range(191, 128) = training_data[i*DIGIT_WIDTH+2];
			buf_knn[batch].training_set[i].range(255, 192) = training_data[i*DIGIT_WIDTH+3];
		}

		for (int i = 0; i < NUM_TEST; i ++ )
		{
			buf_knn[batch].testing_set[i].range(63 , 0  ) = testing_data[i*DIGIT_WIDTH+0];
			buf_knn[batch].testing_set[i].range(127, 64 ) = testing_data[i*DIGIT_WIDTH+1];
			buf_knn[batch].testing_set[i].range(191, 128) = testing_data[i*DIGIT_WIDTH+2];
			buf_knn[batch].testing_set[i].range(255, 192) = testing_data[i*DIGIT_WIDTH+3];
		}

		for (int i = 0; i < 100; i ++ )
		{
			buf_knn[batch].testing_set[i].range(63 , 0  ) = training_data[i*DIGIT_WIDTH+0];
			buf_knn[batch].testing_set[i].range(127, 64 ) = training_data[i*DIGIT_WIDTH+1];
			buf_knn[batch].testing_set[i].range(191, 128) = training_data[i*DIGIT_WIDTH+2];
			buf_knn[batch].testing_set[i].range(255, 192) = training_data[i*DIGIT_WIDTH+3];
		}
		print("Buffers initialized.\n\r");
	}

	// Initialize 3DR data
	print("Initializing 3dr.\n\r");
	for(int batch = 0; batch < BATCH_SIZE_3DR; batch++) {
		buf_3dr[batch].input = (bit32*)malloc(3 * NUM_3D_TRI * sizeof(bit32));
		buf_3dr[batch].output = (bit32*)malloc(NUM_FB * sizeof(bit32));
		buf_3dr[batch].size_fragment = (bit16*)malloc(NUM_3D_TRI * sizeof(bit16));
		buf_3dr[batch].fragment_x = (bit8*)malloc(500 * NUM_3D_TRI * sizeof(bit8));
		buf_3dr[batch].fragment_y = (bit8*)malloc(500 * NUM_3D_TRI * sizeof(bit8));
		buf_3dr[batch].fragment_z = (bit8*)malloc(500 * NUM_3D_TRI * sizeof(bit8));
		buf_3dr[batch].fragment_color = (bit8*)malloc(500 * NUM_3D_TRI * sizeof(bit8));
		buf_3dr[batch].size_pixels = (bit16*)malloc(NUM_3D_TRI * sizeof(bit16));
		buf_3dr[batch].pixels_x = (bit8*)malloc(500 * NUM_3D_TRI * sizeof(bit8));
		buf_3dr[batch].pixels_y = (bit8*)malloc(500 * NUM_3D_TRI * sizeof(bit8));
		buf_3dr[batch].pixels_color = (bit8*)malloc(500 * NUM_3D_TRI * sizeof(bit8));

#ifdef DEBUG
		xil_printf("Initializing 3dr buffer %d\n\r", batch);
		xil_printf("buf_3dr[%d].input is at %p\r\n", batch, buf_3dr[batch].input);
		xil_printf("buf_3dr[%d].output is at %p\r\n", batch, buf_3dr[batch].output);
		xil_printf("buf_3dr[%d].size_fragment is at %p\r\n", batch, buf_3dr[batch].size_fragment);
		xil_printf("buf_3dr[%d].fragment_x is at %p\r\n", batch, buf_3dr[batch].fragment_x);
		xil_printf("buf_3dr[%d].fragment_x is at %p\r\n", batch, buf_3dr[batch].fragment_x);
		xil_printf("buf_3dr[%d].fragment_z is at %p\r\n", batch, buf_3dr[batch].fragment_z);
		xil_printf("buf_3dr[%d].fragment_color is at %p\r\n", batch, buf_3dr[batch].fragment_color);
		xil_printf("buf_3dr[%d].size_pixels is at %p\r\n", batch, buf_3dr[batch].size_pixels);
		xil_printf("buf_3dr[%d].pixels_x is at %p\r\n", batch, buf_3dr[batch].pixels_x);
		xil_printf("buf_3dr[%d].pixles_y is at %p\r\n", batch, buf_3dr[batch].pixels_y);
		xil_printf("buf_3dr[%d].pixels_color is at %p\r\n", batch, buf_3dr[batch].pixels_color);
#endif

		// pack input data for better performance
		for ( int i = 0; i < NUM_3D_TRI; i++) {
			buf_3dr[batch].input[3*i](7,0)     = triangle_3ds[i].x0;
			buf_3dr[batch].input[3*i](15,8)    = triangle_3ds[i].y0;
			buf_3dr[batch].input[3*i](23,16)   = triangle_3ds[i].z0;
			buf_3dr[batch].input[3*i](31,24)   = triangle_3ds[i].x1;
			buf_3dr[batch].input[3*i+1](7,0)   = triangle_3ds[i].y1;
			buf_3dr[batch].input[3*i+1](15,8)  = triangle_3ds[i].z1;
			buf_3dr[batch].input[3*i+1](23,16) = triangle_3ds[i].x2;
			buf_3dr[batch].input[3*i+1](31,24) = triangle_3ds[i].y2;
			buf_3dr[batch].input[3*i+2](7,0)   = triangle_3ds[i].z2;
			buf_3dr[batch].input[3*i+2](31,8)  = 0;
		}
		for(int i = 0; i < NUM_FB; i++) { // initializing the output to zero
			buf_3dr[batch].output[i](31, 0) = 0;
		}
	}

	// Create the frame buffer used for printing the results, purely for debugging
	bit8 frame_buffer_print[MAX_X][MAX_Y];

    // Initialize Lenet buffers
    print("Initializing Lenet\n\r");
    parse_mnist_labels("labels.bin", lenet_labels);
    parse_mnist_images("images.bin", lenet_images);
    parse_parameters("params.bin");

#ifdef DEBUG
    xil_printf("Conv1_weights: \n\r");
    print_conv1_weights();
    xil_printf("Conv1_bias: \n\r");
    print_conv1_bias();
    xil_printf("Image:\n\r");
    print_img(0);
#endif

    // Initializing image compression structures
    print("Initializing Image Compression\n\r");
    init_img_cmp_data(25, 10);

	// Initializing optical flow buffers
	print("Initializing Optical Flow\n\r");

	std::string dataPath("datasets/current");
	std::string outFile("out.flo");

	// create actual file names according to the datapath
	std::string frame_files[5];
	std::string reference_file;
	frame_files[0] = "frame1.ppm";
	frame_files[1] = "frame2.ppm";
	frame_files[2] = "frame3.ppm";
	frame_files[3] = "frame4.ppm";
	frame_files[4] = "frame5.ppm";
	reference_file = "ref.flo";

	CByteImage imgs[5];
	for (int i = 0; i < 5; i++)
	{
		CByteImage tmpImg;
		ReadImage(tmpImg, frame_files[i].c_str());
		imgs[i] = ConvertToGray(tmpImg);
	}



	// read in reference flow file
	printf("Reading reference output flow... \n");

	CFloatImage refFlow;
	ReadFlowFile(refFlow, reference_file.c_str());

	// pack the values
	for(int batch = 0; batch < BATCH_SIZE_OPTICAL_FLOW; batch++) {
		xil_printf("Optical Flow: On batch %d\n\r", batch);
		for (int i = 0; i < MAX_HEIGHT; i++) {
			for (int j = 0; j < MAX_WIDTH; j++) {
				buf_optical_flow[batch].frames[i][j](7 ,  0) = imgs[0].Pixel(j, i, 0);
				buf_optical_flow[batch].frames[i][j](15,  8) = imgs[1].Pixel(j, i, 0);
				buf_optical_flow[batch].frames[i][j](23, 16) = imgs[2].Pixel(j, i, 0);
				buf_optical_flow[batch].frames[i][j](31, 24) = imgs[3].Pixel(j, i, 0);
				buf_optical_flow[batch].frames[i][j](39, 32) = imgs[4].Pixel(j, i, 0);
				buf_optical_flow[batch].frames[i][j](63, 40) = 0;
			}
		}
	}

	// Initializing alexnet buffers
	for(int batch = 0; batch < BATCH_SIZE_ALEXNET; batch++) {
		xil_printf("AlexNet: On batch %d\n\r", batch);
		readimage("0.bin", buf_alexnet[batch].image);
		readweights("conv1.bin", buf_alexnet[batch].conv1_weights, buf_alexnet[batch].conv1_bias);
		readweights2("conv2.bin",buf_alexnet[batch].conv2_weights, buf_alexnet[batch].conv2_bias);
		readweights3("conv3.bin",buf_alexnet[batch].conv3_weights, buf_alexnet[batch].conv3_bias);
		readweights4("conv4.bin",buf_alexnet[batch].conv4_weights, buf_alexnet[batch].conv4_bias);
		readweights5("conv5.bin",buf_alexnet[batch].conv5_weights, buf_alexnet[batch].conv5_bias);
		readweights6("fc6.bin",buf_alexnet[batch].fc6_weights, buf_alexnet[batch].fc6_bias);
		readweights7("fc7.bin",buf_alexnet[batch].fc7_weights, buf_alexnet[batch].fc7_bias);
		readweights8("fc8.bin",buf_alexnet[batch].fc8_weights, buf_alexnet[batch].fc8_bias);
	}

    int total_num_ips = get_total_ips();
    xil_printf("Running a total of %d IPs\n\r", total_num_ips);

  	// Running entire pr flow
  	run_pr_flow(total_num_ips);
  	xil_printf("PR Flow complete\n\r");

    // read result from the 32-bit output buffer
    for(int batch = 0; batch < BATCH_SIZE_3DR; batch++) {
    	xil_printf("Processing 3dr batch %d\n\r", batch);
    	for (int i = 0, j = 0, n = 0; n < NUM_FB; n ++ ) {
    		bit32 temp = buf_3dr[batch].output[n];
    		frame_buffer_print[i][j++] = temp(7,0);
    		frame_buffer_print[i][j++] = temp(15,8);
    		frame_buffer_print[i][j++] = temp(23,16);
    		frame_buffer_print[i][j++] = temp(31,24);
    		if(j == MAX_Y) {
    			i++;
    			j = 0;
    		}
    	}

    	xil_printf("Batch %d: Image After Rendering: \n", batch);
    	for (int j = MAX_X - 1; j >= 0; j -- ) {
    		for (int i = 0; i < MAX_Y; i ++ ) {
    			int pix;
    			pix = frame_buffer_print[i][j];
    			if (pix)
    				print("1");
    			else
    				print("0");
    		}
    		print("\n\r");
    	}
    }

    xil_printf("Lenet result: %d/%d correct!\n\r", lenet_num_correct, BATCH_SIZE_LENET);

    print("KNN results:\n\r");
    for(int batch = 0; batch < BATCH_SIZE_KNN; batch++) {
    	int correct_cnt = 0;
    	for (int i = 0; i < NUM_TEST; i ++ )
    	{
    		int tmp = buf_knn[batch].results[i];
    		if (tmp != expected[i]){
    			//xil_printf("[%d]: Test = %d;  Expected = %d\n\r", i, tmp, expected[i]);
    		} else {
    			correct_cnt ++;
    		}
    	}
    	xil_printf("KNN batch %d result: %d/%d correct!\n\r", batch, correct_cnt, NUM_TEST);
    }

    xil_printf("Img Compression results: \n\r");
    for(int batch = 0; batch < BATCH_SIZE_IMG_CMP; batch++) {
    	printf("Img compression batch %d has a cumulative error of %f\n\r", batch, img_cmp_check_result(img_cmp_output_golden[batch], img_cmp_dst[batch], 25, 10));
    }

    xil_printf("Optical flow results: \n\r");
    for(int batch = 0; batch < BATCH_SIZE_OPTICAL_FLOW; batch++) {
		xil_printf("Batch %d\r\n", batch);
        optical_flow_check_results(buf_optical_flow[batch].outputs_x, buf_optical_flow[batch].outputs_y, refFlow, outFile);
    }

    xil_printf("Alexnet results: \n\r");

    for(int batch = 0; batch < BATCH_SIZE_ALEXNET; batch++) {
		softmax(buf_alexnet[batch].fc8_rst, buf_alexnet[batch].softmax_output, 1000);
		double max=0.00f;
		int index=0;
		for(int i=0; i<1000; i++)
		{
			if(buf_alexnet[batch].softmax_output[i] > max)
			{
				max = buf_alexnet[batch].softmax_output[i];
				index = i;
			}
		}
		printf("batch %d Result %d\n\r", batch, index);
    }

    // Print the timing information of all IPs
    print_timers();

	// cleanup
	cleanup_platform();
	return 0;
}
