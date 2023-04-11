/*
 * Empty C++ Application
 */
/* File inclusions */
#include "sd_card.h"
#include "gpio_user.h"
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xilfpga.h"
#include "nimblock_types.h"
#include <stdlib.h>
#include <malloc.h>
#include "xhls_ip.h"
#include <unordered_map>
#include "event_gen.h"
#include <list>
#include "configure.h"
#include "nimblock_schedule.h"
#include "nimblock_schedule_apps.h"
#include <algorithm>
#include "round_robin.h"

/* Static objects */
// static objects for Filesystem
static FIL fil;
static FATFS fatfs;
static char *SD_File;
// Static objects for GPIO
XGpioPs psGpioInstancePtr;
XGpioPs_Config *GpioConfigPtr;
int xStatus;
// Static objects for PR
XFpga XFpgaInstance = {0U};
// Static increasing id to identify new events

int main()
{
	init_platform();
	int status; 
	printf("[INFO]: Beginning Setup\n\r");
	for(int i = 0; i < 1; i++) {
	  sleep(1);
	  printf("%d\r\n", i);
	}

	// Flush and disable Data Cache
	Xil_DCacheDisable();

	// Initialize SD controller
	SD_Init(&fatfs);

	// Configure GPIO pins
	init_gpio(GpioConfigPtr, &psGpioInstancePtr, xStatus);

	// Initialize XFpga object
	status = XFpga_Initialize(&XFpgaInstance);
	if(status != XST_SUCCESS) {
		printf("[ERROR] Failed to initialize XFpgaInstance\n\r");
		cleanup_platform();
		return 1;
	}

	/* Initialize Slot state */
	std::vector<Slot*> machines;
	status = init_slots(&psGpioInstancePtr, machines);
	if(status != XST_SUCCESS) {
		printf("[ERROR] Failed to initialize Slots\n\r");
		cleanup_platform();
		return 1;
	}
#ifdef NIMBLOCK
	printf("[INFO]: ****** LAUNCHING Nimblock run*****\n\r");
#else
	printf("[INFO]: ****** LAUNCHING FCFS run*****\n\r");
#endif

#ifdef REPLAY
	printf("[INFO]: Starting Slow Replay Iterations! Running %d iterations!\n\r", REPLAY_RUNS);
	for(int j = 0; j < REPLAY_RUNS; j++) {
	printf("*******Starting iteration %d******\n\r", j);
#endif
	/* Initialize Queues */
	XTime i_time;
	// Initialize Application queue to empty
	std::list<Application*> pending_apps;
	#ifdef APP
	// Initialize candidate app list to empty
	std::list<Application*> candidate_apps;
	#endif
	// Initialize Pending task list to empty
	std::list<Task*> pending_tasks;
	// Initialize ready task queue to empty
	std::list<Task*> ready_tasks;
	// Initialize completed lists to empty
	#ifdef BASELINE
	std::list<Application*> waiting_apps;
	std::list<Task*> waiting_tasks;
	#endif
	std::vector<Application*> completed_apps;
	std::vector<double> event_delays; // For replay
	std::vector<int> batch_nums; // For replay
	std::vector<int> event_nums; // For replay
	std::vector<int> priority_levels; // For replay
	printf("[INFO]: Finished Nimblock Startup, generating first event ...\n\r");
	printf("[INFO] %f ms: Adding new event, %d\n\r", 0.0, 1);
	int replay_id = 0;
	// Generate First event
	#ifdef REPLAY
	Application* status_app = add_new_event_replay(pending_apps, pending_tasks, replay_id, j);
	if (status_app == NULL) {
		printf("[ERROR]: Tried to generate an invalid event\n\r");
		cleanup_platform();
		return 1;
	}
	#else
	Application* status_app = add_new_event(pending_apps, pending_tasks);
	if (status_app == NULL) {
		printf("[ERROR]: Tried to generate an invalid event\n\r");
		cleanup_platform();
		return 1;
	}
	#endif
	XTime_GetTime(&i_time);
	double initial_time = 1.0 * i_time / (COUNTS_PER_SECOND / 1000000);
	double true_initial = initial_time;
	double initial_time_event = initial_time;
	int apps_added = 1;
	volatile int spin = 0;
	double threshold = 1.0;
	#ifdef REPLAY
	double event_delay = next_event_delay_replay(replay_id, j);
	replay_id++;
	#else
	double event_delay = next_event_delay(150, 200);
	event_delays.push_back(event_delay);
	batch_nums.push_back(status_app->batch);
	event_nums.push_back(status_app->id);
	priority_levels.push_back(status_app->priority_level);
	#endif
	while (spin < 100000) {
		// Timer information
		bool updated_already = false;
		XTime c_time;
		XTime_GetTime(&c_time);
		double cur_time = 1.0 * c_time / (COUNTS_PER_SECOND / 1000000); // time in us
		// Determine if it is time for a scheduling interval
		double time_diff = cur_time - initial_time;
		double time_diff_event = cur_time - initial_time_event;
		if (time_diff > SCHED_INTERVAL) {
			// Update priorities b/c scheduling
			initial_time = cur_time;
			#ifdef NIMBLOCK
				#ifdef APP
				threshold = update_priorities_app(pending_apps, candidate_apps);
				allocate_resources(candidate_apps);
				#else 
				threshold = update_priorities(ready_tasks, machines);
				#endif
				updated_already = true;
			#endif
		}
		// Add new events to queue
		#ifdef SYSTEM_TEST
		if (apps_added < MAX_APPS && time_diff_event > event_delay) 
		{
			#ifdef REPLAY
			#ifdef BASELINE
			// Baseline adds to a waiting app queue instead
			status_app = add_new_event_replay(waiting_apps, waiting_tasks, replay_id, j);
			#else
			status_app = add_new_event_replay(pending_apps, pending_tasks, replay_id, j);
			#endif
			#else
			status_app = add_new_event(pending_apps, pending_tasks);
			#endif
			#ifdef NIMBLOCK
			if (!updated_already) {
				#ifdef APP
				threshold = update_priorities_app(pending_apps, candidate_apps);
				#else 
				threshold = update_priorities(ready_tasks, machines);
				#endif
				updated_already = true;
			}
			#endif
			if (status_app == NULL) {
				printf("[ERROR]: Tried to generate an invalid event\n\r");
				cleanup_platform();
				return 1;
			}
			initial_time_event = cur_time;
			++apps_added;
			printf("[INFO] %f ms: Adding new event, %d\n\r", (cur_time - true_initial)/1000.0, apps_added);
//			printf("[INFO]: Pending tasks length is %d\n\r", pending_tasks.size());
			#ifdef REPLAY
			event_delay = next_event_delay_replay(replay_id, j);
			replay_id++;
			#else
			event_delay = next_event_delay(80, 150);
			event_delays.push_back(event_delay);
			batch_nums.push_back(status_app->batch);
			event_nums.push_back(status_app->id);
			priority_levels.push_back(status_app->priority_level);
			#endif
		}
		#endif
		// Check slot status/retire tasks
		update_machine_state(machines);
		// Make new tasks ready
		#ifdef NIMBLOCK
			// Make new tasks ready
			#ifdef APP
			bool rethresh = update_candidate_apps(pending_apps, candidate_apps, threshold);
			if (rethresh) {
				allocate_resources(candidate_apps);
			}
			// TODO if resource excess, lower threshold????
			#else 
			threshold = update_ready_tasks_nimblock(pending_tasks, ready_tasks, threshold);
			#endif
			// Retire Application
			#ifdef APP
			// Retire from both queues as a failsafe
			bool ret = retire_app(pending_apps, completed_apps);
			ret = ret || retire_app(candidate_apps, completed_apps);
			if (ret) {
				// change thresholds because we have more slots available
				allocate_resources(candidate_apps);
			}
			#else
			bool ret = retire_app(pending_apps, completed_apps);
			#endif
			if (ret && !updated_already) {
				#ifdef APP
				threshold = update_priorities_app(pending_apps, candidate_apps);
				#else 
				threshold = update_priorities(ready_tasks, machines);
				#endif
				updated_already = true;
			}
			// Select new task to schedule
			#ifdef APP
			Task* new_task = select_task_app(candidate_apps);
			// TODO if this is NULL lower threshold
			#else
			Task* new_task = select_task_nimblock(ready_tasks, threshold);
			#endif
			// Select tile to schedule to
			Slot* new_tile = select_slot(machines);
			// Begin Schedule Process
		#else
			#ifdef RR
			// Update task queues
			arbitrate(machines, pending_apps);
			// Retire application
			retire_app(pending_apps, completed_apps);
			// Select Slot to schedule
			Slot* new_tile = select_slot_rr(machines);
			// Select Task to schedule
			Task* new_task = select_task_rr(new_tile);
			#else
			// Make new tasks ready
			update_ready_tasks(pending_tasks, ready_tasks);
			// Retire Application
			retire_app(pending_apps, completed_apps);
			// Select new task to schedule
			Task* new_task = select_task(ready_tasks);
			// Select tile to schedule to
			Slot* new_tile = select_slot(machines);
			// Begin Schedule Process
			#endif
		#endif
		if (new_task != NULL && new_tile != NULL) {
			// no available tasks right now...
			// Task is good to go
			status = configure_task(psGpioInstancePtr, XFpgaInstance, new_task, new_tile);
			if (status) {
				printf("[ERROR]: Failed to drive partial reconfiguration\n\r");
				cleanup_platform();
				return 1;
			}
			launch_task(new_task, new_tile);
//			printf("[INFO] %f ms: Successfully Configured event %s_%d to slot %d and Launched!!!\n\r", (cur_time - true_initial)/1000.0, new_task->app_id->app_name, new_task->id, new_tile->id);
		} else if (new_tile == NULL && new_task != NULL) {
			#ifdef NIMBLOCK
			#ifdef PREEMPT
			#ifdef APP
			Slot* preempt_tile = preempt_slot_apps(machines, new_task);
			#else
			Slot* preempt_tile = preempt(machines, new_task, cur_time);
			#endif
//			Slot* preempt_tile = preempt_nimblock(machines, new_task, cur_time);
			#else
			Slot* preempt_tile = NULL;
			#endif
			if (preempt_tile != NULL) {
				// Add to pending_tasks queue to detect a dead task.
				// By moving preempted tasks to pending tasks, successors that have more tokens
				// than predecessors won't be ready to execute and we won't deadlock
				#ifdef APP
				// Don't need to move task to ready/pending queue if doing app facing approach
				#else
				pending_tasks.push_front(preempt_tile->cur);
				#endif
				preempt_tile->cur->app_id->occupancy -= 1;
				preempt_tile->cur->machine = NULL;
				status = configure_task(psGpioInstancePtr, XFpgaInstance, new_task, preempt_tile);
				if (status) {
					printf("[ERROR] Failed to drive partial reconfiguration\n\r");
					cleanup_platform();
					return 1;
				}
				launch_task(new_task, preempt_tile);
				printf("[INFO] %f ms: Successfully Preempted event %s_%d with priority %d to slot %d and Launched!!!\n\r", (cur_time - true_initial)/1000.0, new_task->app_id->app_name, new_task->id, new_task->app_id->priority_level,  preempt_tile->id);
				printf("[INFO] %f ms: Evicted event %s_%d with priority %d \n\r", (cur_time - true_initial)/1000.0, pending_tasks.front()->app_id->app_name, pending_tasks.front()->id, pending_tasks.front()->app_id->priority_level);
			} else {
				// Not preempting, put task back in queue
				#ifndef APP
				ready_tasks.push_front(new_task);
				#endif
			}
			#else
				#ifndef RR
				ready_tasks.push_front(new_task); // Need to re-add task to queue in this case
				#endif
			#endif
		} else if (new_tile == NULL && new_task != NULL) {
			// no available slots right now...
			// Add task back to ready tasks
			#ifndef APP
				#ifndef RR
				ready_tasks.push_front(new_task);
				#endif
			#endif 
		}
		// End condition
		#ifdef BASELINE
		if (pending_apps.empty()) {
			if (!waiting_apps.empty()) {
				// add new event to queue
				printf("[INFO]: Replenishing pending_apps with %s\n\r", waiting_apps.front()->app_name);
				// Moves relevant tasks from waiting tasks to pending_tasks
				Application* a = waiting_apps.front();
				for (auto it = waiting_tasks.begin(); it != waiting_tasks.end(); ++it) {
					Task* t = *it;
					if (t->app_id == a) {
						pending_tasks.push_back(t);
					}
				}
				pending_apps.push_back(a);
				waiting_apps.pop_front();
			}
		}
		// End condition for baseline
		if (pending_apps.empty() && waiting_apps.empty() && apps_added >= MAX_APPS) {
			printf("[INFO]: Finished executing all apps in queue and added enough apps, shutting down\n\r");
			break;
		}
		#else 
		if (completed_apps.size() >= MAX_APPS) {
			#ifdef SYSTEM_TEST
			if (apps_added >= MAX_APPS) {
				printf("[INFO]: Finished executing all apps in queue and added enough apps, shutting down\n\r");
				break;
			}
			#else
			printf("[INFO]: Finished executing all Apps in queue, shutting down\n\r");
			break;
			#endif
		}
		#endif
	}
	printf("[INFO]: Length of completed apps is: %d\n\r", completed_apps.size());
	print_timers(completed_apps);
	#ifndef REPLAY
	print_snapshot(event_delays, batch_nums, event_nums, priority_levels);
	#endif
	printf("[INFO]: Cleaning up structures\n\r");
	#ifdef BASELINE
	waiting_tasks.clear();
	#endif
	cleanup_structures(completed_apps, machines);
#ifdef REPLAY
	printf("*******Finished iteration %d******\n\r\n\r\n\r", j);
	}
#endif
	printf("[INFO]: Cleaning up platform \n\r");
	cleanup_platform();
	return 0;
}
