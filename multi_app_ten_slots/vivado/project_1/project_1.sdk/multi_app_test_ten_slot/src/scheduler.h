#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xil_io.h"
#include "xil_types.h"
#include "ff.h"
#include "xtime_l.h"
#include "xgpiops.h"

#include "xhls_ip.h"
#include "globals.h"

extern XGpioPs psGpioInstancePtr;
extern XGpioPs_Config *GpioConfigPtr;


// Structure to define an IP
struct ip {
    int id;
    int app_id;
    int *dep;
    int num_dep;
    int batch;
    int kern;
    int slot;
    u32 BitAddr;
    u32 Bitlen;
    int decouple;
    int reset;
    int num_invoke;
    char *bin_name;
    int batch_offset;
    int run_time;
    int pr_time;
    XTime tStart_ip, tEnd_ip;
    XTime tStart_pr, tEnd_pr;
    XTime tStart_wait, tEnd_wait;
    XTime tStart_run, tEnd_run;
};


struct slot {
	int id;
    int batch_count;
    struct ip *cur;
    XHls_ip accelerator;
    int status;
    int num_invoke;
};

// Partial bitfile lengths in number of words
#define PARTIAL_BITFILE_LEN_0   0xE6BA8
#define PARTIAL_BITFILE_LEN_1   0xF08F8
#define PARTIAL_BITFILE_LEN_2   0xF1AF8
#define PARTIAL_BITFILE_LEN_3   0xFB1E8
#define PARTIAL_BITFILE_LEN_4   0xFABE8
#define PARTIAL_BITFILE_LEN_5   0xFC6D0
#define PARTIAL_BITFILE_LEN_6   0xFCBE8
#define PARTIAL_BITFILE_LEN_7   0xFCBE8
#define PARTIAL_BITFILE_LEN_8   0xFCBE8
#define PARTIAL_BITFILE_LEN_9   0x112278
#define PARTIAL_BITFILE_ALLOCATED_SPACE 0x230000 // Needs to have a certain alignment so I just set this

#define BATCH_SIZE_3DR 16
#define BATCH_SIZE_LENET 16
#define BATCH_SIZE_KNN 1
#define BATCH_SIZE_IMG_CMP 1
#define BATCH_SIZE_OPTICAL_FLOW 16
#define BATCH_SIZE_ALEXNET 1

//#define DEBUG


void sched_init(struct slot *slots, struct ip ip_list[NUM_APPLICATIONS][MAX_NUM_IP]);
int apps_init();
void init_ip_entry(int id, int app_id, u32 len, int *dep, int num_dep, int batch_offset, int batch,
		int slot, int kern, int pr_time, int run_time, int decouple, int reset, char* bin_name, char* pr_address, struct ip *ret);
void init_slot(int id, struct slot *ret);
void print_timers();
void free_all();
void load_bins();
int get_total_ips();
int choose_next_app(int next_app_idx);

#endif
