#ifndef NIMBLOCK_TYPES_H
#define NIMBLOCK_TYPES_H

#include "xhls_ip.h"
#include "xtime_l.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <queue>
/*
 *  The following are valid configurations
 *  Base:
 *      define REPLAY, SYSTEM_TEST, BASELINE
 *  OPT Base:
 *      define REPLAY, SYSTEM_TEST, BASELINE, BETTER_BASELINE, PIPELINE
 *  FCFS:
 *      define REPLAY, SYSTEM_TEST, (PIPELINE)
 *  Round Robin (Coytoe):
 *      define REPLAY, SYSTEM_TEST, RR, (PIPELINE)
 *  PREMA:
 *      define REPLAY, SYSTEM_TEST, NIMBLOCK, (PIPELINE)
 *  NIMBLOCK:
 *      define REPLAY, SYSTEM_TEST, NIMBLOCK, PREEMPT, PIPELINE, APP
 */
#define REPLAY
#define SYSTEM_TEST
//#define BASELINE
//#define BETTER_BASELINE
//#define RR
#define NIMBLOCK
//#define PREEMPT
//#define PIPELINE
#define APP
//#define DEBUG


// #define BATCH_BOUNDS 6
#define FUNC_COEFFS 3
#define NUM_APPS 4
#define MAX_BATCH 30
#define MAX_BATCH_LENET MAX_BATCH
#define MAX_BATCH_KNN 1
#define MAX_BATCH_3DR MAX_BATCH
#define MAX_BATCH_ALEXNET 3
#define MAX_BATCH_IMG_CMP MAX_BATCH
#define MAX_BATCH_OPTICAL_FLOW MAX_BATCH


#define SCHED_INTERVAL 400000.0 /* Defined in us */
#define MAX_APPS 20
#define LEN_REPLAY MAX_APPS
#define REPLAY_RUNS 10

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

#define IDLE        0
#define PR_READY    1
#define PENDING_LAUNCH      2
#define PR_CFG      3
#define WAITING     4
#define RUNNING     5
#define PENDING_CLEANUP 6

#define NUM_SLOTS 10
#ifdef BETTER_BASELINE
	#define NUM_SLOTS_ACTIVE NUM_SLOTS
#else
	# ifdef BASELINE
		#define NUM_SLOTS_ACTIVE 1
	#else
		#define NUM_SLOTS_ACTIVE NUM_SLOTS
	#endif
#endif

#define NUM_IP_3DR 3
#define NUM_IP_LENET 3
#define NUM_IP_KNN 3
#define NUM_IP_IMG_CMP 6
#define NUM_IP_OPTICAL_FLOW 9
#define NUM_IP_ALEXNET 38
typedef struct
{
  u8   x0;
  u8   y0;
  u8   z0;
  u8   x1;
  u8   y1;
  u8   z1;
  u8   x2;
  u8   y2;
  u8   z2;
} Triangle_3D;
#define NUM_3D_TRI 3192

enum arg_proto {
    PASS,
    PASSED,
    INVALID,
    FREE,
};

struct arg_location {
    int kernel;
    int index;
};

class Application; // Forward declaration for now...might move this later
class Slot; // Forward declaration for now...might move this later

class Task {
    public:
        int id; /* This task's index in its application's list */
        Application* app_id; /* Used to access app information */
        double priority;
        int batch;
        char **bin_name;
        int batch_completed;
        // int num_inputs;
        // int* input_sizes;
        // int num_outputs;
        // int* output_sizes;
        int num_args;
        int* arg_sizes;
        std::vector<UINTPTR> arg_buffers;
        std::vector<int> const_values;
//        std::queue<std::vector<UINTPTR>> old_arg_buffers; // Preserved for future batches if necessary
        u64 latency;
        enum arg_proto* arg_protocols;
        struct arg_location* arg_locations;
        Slot* machine;
        bool queued;
        XTime tStart_ip, tEnd_ip;
        XTime tStart_pr, tEnd_pr;
        XTime tStart_run, tEnd_run;

        Task(int id, Application* app_id, unsigned long priority, int batch, char** bin_name, int batch_completed, int num_args, int* arg_sizes) : 
            id(id),
            app_id(app_id),
            priority(priority),
            batch(batch),
            bin_name(bin_name),
            batch_completed(batch_completed),
            num_args(num_args),
            arg_sizes(arg_sizes),
            arg_protocols(NULL),
            arg_locations(NULL),
            machine(NULL),
            queued(false)
            {}
        
};

class Application {
    public: 
        std::vector<Task*> tasks;
        std::vector<int*> dependency_list;
        std::vector<int> num_deps;
        XTime arrival_time;
        XTime start_time, end_time;
        XTime pr_start_time, pr_end_time;
        double priority;
        char* app_name;
        int priority_level; // Real time priority, not queue priority
        int batch;
        int id;
        int occupancy;
        u64 latency;
        int threshold;
        int tasks_completed;
        int parallelism; // defines the parallism factor for this application's DFG
    Application() : threshold(0), tasks_completed(0) {}
};

struct task_compare {
    bool operator()(const Task* task1, const Task* task2) {
        if(task1->app_id->priority_level > task2->app_id->priority_level) {
            return true;
        }
        if (task1->app_id == task2->app_id) {
            if (task1->id < task2->id) {
                return true; // return the earliest task in the DFG if same app
            }
        }
        // TODO how do I implement the tiebreaking stuff??? Not in Coyote source code
        return false;
    }
};

class Slot {
    public:
        int id;
        int batch_count;
        Task* cur;
        XHls_ip accelerator;
        int status;
        int decouple_gpio;
        int reset_gpio;
        u32 bin_len;
        std::priority_queue<Task*, std::vector<Task*>, struct task_compare> tasks; // 
};

#endif /* NIMBLOCK_TYPES_H */
