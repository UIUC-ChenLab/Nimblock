# Nimblock
Scheduling for Fine-grained FPGA Sharing through Virtualization

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.7818841.svg)](https://doi.org/10.5281/zenodo.7818841)

Nimblock employs an overlay architecture which enables arbitrary, independent user logic to share portions of a single FPGA by dividing the FPGA into independently reconfigurable slots. We then explore scheduling possibilities to effectively time- and space-multiplex the virtualized FPGA by introducing Nimblock. The Nimblock scheduling algorithm balances application priorities and performance degradation to improve response time and reduce deadline violations. Unlike other algorithms, Nimblock explores preemption as a scheduling parameter to dynamically change resource allocations, and automatically allocates resources to enable suitable parallelism for an application without additional user input. We demonstrate system feasibility by realizing the complete system on a Xilinx ZCU106 FPGA and evaluating on a set of real-world benchmarks. In our results, we achieve lower average response times, fewer deadline violations, and lower tail response times when compared to other high-performance scheduling algorithms.

For more details, please see our ISCA'23 paper:
```
@inproceedings{nimblock,
  author = {Mandava, Meghna and Reckamp, Paul and Chen, Deming},
  title = {Nimblock: Scheduling for Fine-grained FPGA Sharing through Virtualization},
  year = {2023},
  isbn = {979-8-4007-0095-8/23/06},
  publisher = {Association for Computing Machinery},
  url = {https://doi.org/10.1145/3503222.3507732},
  doi = {10.1145/3579371.3589095},
  keywords = {Virtualization, Reconfigurable Computing, Real-time Scheduling, Overlay Architectures},
  location = {Orlando, FL, USA},
  series = {ISCA '23}
}
```
  
## List of directories/files
  
- **multi_app_ten_slots**: contains full and partial bitstreams generated for Xilinx ZCU106, as well as hardware description file to create new applications in Xilinx SDK
- **working_nimblock_sw.zip**: archived SDK project, containing board support packages and imported applications to run the Nimblock hypervisor.
- **nimblock_hypervisor**: software for nimblock hypervisor to run baremetal via Xilinx SDK
- **data_gen**: scripts to generate event sequences and parse output data

## To run Nimblock:

1. Download and install Xilinx SDK 2019.1: [Vivado Design Suite - HLx Editions - 2019.1  Full Product Installation](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vivado-design-tools/archive.html). Only SDK is needed to run the code.
2. Start a new SDK workspace and select "Import Project".
3. Select "Select archive file" and choose working_nimblock_sw.zip. The Board Support Packages for the Zynq FSBL and Nimblock hypervisor have been generated along with the applications. The hardware platform has already been created and linked to the BSPs/applications.
4. The workspace should load. Right click to build applications if they do not automatically build. If there are build errors in the nimblock_zcu_sw application, right click to "Change referenced BSP" to nimblock_zcu_sw_bsp.
5. Load the partial bitstreams in Nimblock/multi_app_ten_slots/bitstreams_bin/ onto the ZCU106 SD card.
6. Open a serial console, such as putty, with the port corresponding to the FPGA and baud rate 115200.
7. Turn on the board/reset the board. Program the FPGA with the initial full bitstream: multi_app_ten_slots/bitstreams/Config_0.bit
8. Due to an [error](https://support.xilinx.com/s/article/72210?language=en_US) in ZCU102/106 boards, the zynq_FSBL application must first be run on the board. After turning on the board/resetting the board and programming the board, run the zynq_FSBL application. (Right click --> Run As --> Launch on Hardware (System Debugger)). This should print output from the First Stage Boot Loaded. 
9. Run nimblock_zcu_sw (Right click --> Run As --> Launch on Hardware (System Debugger)). If the program is running, you should see something like

```
[INFO]: Beginning Setup

0

PS GPIO INIT SUCCEED 

[INFO]: ****** LAUNCHING Nimblock run*****

```

Example output can be found in data_gen/output_data.

10. Save the output to data_gen/output_data for parsing and report generation.

## To create new tests and generate reports

1. Modify data_gen/data_gen.py with the minimum and maximum delays between application requests (in milliseconds) and define the filename at which the generated sequences should be saved. Minimum and maximum delays for test cases are 1500-2000 ms for "slow"/standard, 150-200 ms for "full"/stress, and 50 ms for "realtime".
  If you wish to modify the applications which are included in the test sequences, event numbers are as follows:
  
  ```
  0. LeNet
  1. KNN
  2. Optical Flow
  3. Image Compression
  4. 3DR
  5. AlexNet
  ```
2. Copy the sequences generate in the txt file into event_gen.cc in the SDK application.

  "replay_event_offset" = Delay Numbers
  
  "replay_batch" = Batch Numbers
  
  "replay_new_app" = Event Numbers
  
  "replay_priority_level" = Priority Numbers
  
  ```
  #ifdef REPLAY
	double replay_event_offset[REPLAY_RUNS][LEN_REPLAY] = {
		    ...
		};

    int replay_batch[REPLAY_RUNS][LEN_REPLAY] = {
    	    ...
    	};
    int replay_new_app[REPLAY_RUNS][LEN_REPLAY] = {
    	    ...
    	};
    int replay_priority_level[REPLAY_RUNS][LEN_REPLAY] = {
    	    ...
    	};
#endif

```
3. To change the scheduling algorithm, modify nimblock_types.h 

```
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
#define PREEMPT
#define PIPELINE
#define APP
//#define DEBUG
```
5. Rebuild the application project.
6. Run as described in above steps. (You should not need to reset the board/run FSBL if you only change the code in nimblock_zcu_sw)
7. Save the serial output from the test case into data_gen/output_data.
8. Run data_analysis.py (modify script if using custom test cases). Comment/uncomment desired reports/graphs at the bottom of data_analysis.py.
  
### For any questions or issues, email me (meghnam4@illinois.edu)
