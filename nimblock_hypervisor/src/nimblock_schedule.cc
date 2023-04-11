#include "nimblock_schedule.h"
#include <list>
#include <vector>
#include <unordered_map>

#define MAX_ARG_NUM 9
int offsets[MAX_ARG_NUM] = {0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58};

/*
 * Ideas for parameter sweeping:
 * 1.) Next task is selected based on priority as well as latency
 * 2.) Change how fast tokens are accumulated
 * 3.) Maximum occupency for each application (for pipelining)
 * 4.) Slot selection (choose slots that aren't on same memory channel/port)
 */


/*
 * Allocates buffers for a particular task's input/output arguments
 * //TODO Only allocate buffers for inputs that are "new" and handle batches correctly
 * Input buffers are deallocated when an application task finishes
 * Output buffers are deallocated (if consumed) by successor
 */
int allocate_buffers(Task* task) {
    int const_index = 0;
    if (task->arg_protocols == NULL) {
        // Old way where buffers are preserved each batch
        if (task->arg_buffers.size() > 0) {
            return 0; // Don't reallocate buffers
        }
        for (int i = 0; i < task->num_args; i++) {
            /* TODO if size is -2 its an integer parameter */
            if (task->arg_sizes[i] == -2) {
                task->arg_buffers.push_back((u64)(task->const_values[const_index]));
                const_index++;
            } else if (task->arg_sizes[i] > 0) {
                void* p = (void*)memalign(8, task->arg_sizes[i]); // Arg sizes include size of datatype
                task->arg_buffers.push_back((u64)p);
            } else {
                printf("[ERROR]: Somehow trying to allocate negative buffer size\n\r");
            }
        }
    } else {
        for (int i = 0; i < task->num_args; i++) {
            /* TODO if size is -2 its an integer parameter */
            if (task->arg_protocols[i] == PASSED) {
                // Arg is from old location
                UINTPTR old_arg = task->app_id->tasks[task->arg_locations[i].kernel]->arg_buffers[task->arg_locations[i].index];
                task->arg_buffers.push_back(old_arg);
            } else {
                // Arg needs to be freshly allocated
                if (task->arg_sizes[i] == -2) {
                    task->arg_buffers.push_back((u64)(task->const_values[const_index]));
                    const_index++;
                } else if (task->arg_sizes[i] > 0) {
                    void* p = (void*)memalign(8, task->arg_sizes[i]); // Arg sizes include size of datatype
                    /* This needs to be fixed...gross dirty fix for now */
                    if (task->id == 0 && i == 0) {
                        read_triangles((u32*)p);
                    }
                    task->arg_buffers.push_back((u64)p);
                } else {
                    printf("[ERROR]: Somehow trying to allocate negative buffer size");
                }
            }
        }
        // Remove old args from dependents
//        for (auto it = task->app_id->tasks.begin(); it != task->app_id->tasks.end(); ++it) {
//            Task* t = *it;
//            if ((t->id == (task->id -1)) && !(t->old_arg_buffers.empty())) {
//                t->old_arg_buffers.pop();
//            }
//        }
    }
    return 0;
}

/*
 * Assumes that all buffers have been allocated, only need to set the parameters to those buffers 
 * Assumes buffers for pipelined components have been set correctly.
 */
int set_params(Task* task, Slot* slot) {
    for (int i = 0; i < task->num_args; i++) {
    	u64 buffer_pointer = task->arg_buffers[i];
    	int my_offset = offsets[i];
    	XHls_ip* my_slot = &(slot->accelerator);
        XAccel_Set_Val(my_slot, buffer_pointer, my_offset);
    }
    return 0;
}

/*
 * Checks if a task in the pending tasks queue is ready to execute.
 * Ready tasks are moved to the ready tasks queue.
 * 
 * Tasks are ready if they:
 *  1.) Have no dependencies
 *  2.) All dependencies complete
 *  3.) At least 1 batch of all dependencies complete
 * 
 */
bool is_ready(Task* task) {
    Application* parent_app = task->app_id;
    int num_deps = parent_app->num_deps[task->id];
    for (int i = 0; i < num_deps; i++) {
        int dep_index = parent_app->dependency_list[task->id][i];
        Task* dep_task = parent_app->tasks[dep_index];
        #ifdef PIPELINE
        if (dep_task->batch_completed <= task->batch_completed) {
            // In this case the dependency is not ahead
            // @TODO might need to change this for piplining...
            return false;
        }
        #else
        if (dep_task->batch_completed < dep_task->batch) {
            // if dependency is not done
            return false;
        }
        #endif
    }
    return true;
}

bool is_done(Task* task) {
    return (task->batch <= task->batch_completed);
}

/*
 * Moves tasks that are marked as ready to the ready task queue.
 */
int update_ready_tasks(std::list<Task*>& pending_tasks, std::list<Task*>& ready_tasks) {
    for (auto it = pending_tasks.begin(); it!=pending_tasks.end();) {
        Task* cur_task = *it;
        if (is_ready(cur_task)){
            // Remove from list and move to other
            it = pending_tasks.erase(it);
            ready_tasks.push_back(cur_task);
        } else {
            ++it;
        }
    }
    return 0;
}
/*
 * Task comparison for pointers to tasks
 */
bool compare_tasks(Task* t1, Task* t2) {
    return t1->priority < t2->priority;
}

/*
 * Moves tasks that are marked as ready to the ready task queue.
 */
double update_ready_tasks_nimblock(std::list<Task*>& pending_tasks, std::list<Task*>& ready_tasks, double threshold) {
    double t_new = threshold;
    for (auto it = pending_tasks.begin(); it!=pending_tasks.end();) {
        Task* cur_task = *it;
        if (is_ready(cur_task)){
            // Remove from list and move to other
            it = pending_tasks.erase(it);
            ready_tasks.push_back(cur_task);
            if (cur_task->priority > threshold) {
                t_new = cur_task->priority;
            }
            // std::push_heap(ready_tasks.begin(), ready_tasks.end(), compare_tasks);
        } else {
            ++it;
        }
    }
    return t_new;
}

/*
 * Select a ready task from ready queue.
 * Task is removed from ready queue and returned.
 * Returns NULL if there are no ready tasks.
 */
Task* select_task(std::list<Task*>& ready_tasks) {
    if (ready_tasks.empty()) {
        return NULL;
    }
//    Task* my_task;
//    Task* smallest_task = NULL;
//    auto ready_itr = ready_tasks.end();
//    for (auto it = ready_tasks.begin(); it!=ready_tasks.end(); ++it) {
//        my_task = *it;
//        if (smallest_task == NULL) {
//            smallest_task = my_task;
//            ready_itr = it;
//        } else {
//            if (my_task->priority < smallest_task->priority) {
//                smallest_task = my_task;
//                ready_itr = it;
//            }
//        }
//    }
//    if(ready_itr != ready_tasks.end()) {
//        ready_tasks.erase(ready_itr);
//    }
     Task* my_task = ready_tasks.front();
     ready_tasks.pop_front();
    return my_task;
}

/*
 * Select a ready task from the heap using the PREMA technique 
 */
Task* select_task_nimblock(std::list<Task*>& ready_tasks, double threshold) {
    if (ready_tasks.empty()) {
        return NULL;
    }
    Task* my_task;
    Task* smallest_task = NULL;
    auto ready_itr = ready_tasks.end(); // 
    for (auto it = ready_tasks.begin(); it!=ready_tasks.end(); ++it) {
        my_task = *it;
        if (my_task->priority > threshold) {
            // This task is valid
            if (smallest_task == NULL) {
                smallest_task = my_task;
                ready_itr = it;
            } else {
                // Choose task to schedule next based on heuristic
                // TODO a possible place for performance improvements
                if (my_task->latency*(my_task->batch - my_task->batch_completed) < smallest_task->latency*(smallest_task->batch - smallest_task->batch_completed)) {
                    // We're choosing the shortest task to maximize the throughput of the system
                    smallest_task = my_task;
                    ready_itr = it;
                }
            }
        }
    }
    if (ready_itr != ready_tasks.end()) {
        ready_tasks.erase(ready_itr);
    }
    return smallest_task;
}

int configure_task(XGpioPs& psGpioInstancePtr, XFpga XFpgaInstance, Task* my_task, Slot* slot) {
    XTime_GetTime(&(my_task->tStart_pr));
    int status = reconfigure_slot(XFpgaInstance, psGpioInstancePtr, my_task, slot);
    XTime_GetTime(&(my_task->tEnd_pr));
    slot->status = PENDING_LAUNCH;
	if (status) {
		printf("[ERROR]: Failed to reconfigure a slot...\n\r");
		return 1;
	}
#ifdef DEBUG
	printf("[DEBUG]: Successfully Configured event to a slot!!!\n\r");
#endif
    allocate_buffers(my_task);
#ifdef DEBUG
	printf("[DEBUG]: Allocated and assigned arguments with dynamic techniques!!\n\r");
#endif
	return 0;
}

void launch_task(Task* my_task, Slot* slot) {
    set_params(my_task, slot);
#ifdef DEBUG
    printf("[DEBUG]: Successfully set params!\n\r");
#endif
    slot->status = RUNNING;
    slot->cur = my_task;
    slot->cur->app_id->occupancy += 1; // Increase app occupancy
    my_task->machine = slot; // Link both ways
    slot->batch_count = my_task->batch;
    XTime_GetTime(&(my_task->tStart_ip));
#ifdef DEBUG
    printf("[DEBUG]: Preparing to launch accelerator\n\r");
#endif
	XHls_ip_Start(&(slot->accelerator));
#ifdef DEBUG
    printf("[DEBUG]: Launched Accelerator!!\n\r");
#endif
}

Slot* select_slot(std::vector<Slot*>& machines) {
    for (auto it = machines.begin(); it != machines.end(); ++it) {
        Slot* slot = *it;
        if (slot -> status == IDLE) {
            // Select This slot
            return slot;
        }
    }
    /* No slots are available */
    return NULL;
}

Slot* preempt(std::vector<Slot*>& machines, Task* ready_task, u64 cur_time) {
    // TODO Only preempt latest task in a pipeline
	// TODO don't preempt self unless task is dead
    // Base latency of task
    // u64 base_lat = ready_task->latency*ready_task->batch; // base latency in ms
	std::vector<Slot*> target_slots;
//    Slot* target_slot = NULL;
    int prio = ready_task->app_id->priority_level;
    // Iterate through machines to find a good candidate to evict
    // TODO need to modify this so it alwyas evicts the last element of a pipeline
    // If not possible, right now, don't evict.
    for (auto it = machines.begin(); it!=machines.end(); ++it) {
        Slot* my_slot = *it;
        Task* slot_task = my_slot->cur;
		if (dead_task(machines, slot_task)&& (my_slot->status == WAITING)) {
			// Dead applications are evicted first
			return my_slot;
		}
        if (slot_task -> app_id == ready_task->app_id) {
            // Don't preempt own application
            continue;
        }
        if (my_slot->cur->app_id->priority_level < 7 && prio > 7 && (my_slot->status == WAITING)) {
            // Always disrupt decision. Immediately prepare for reconfiguration
#ifdef DEBUG
        	printf("[DEBUG]: Preempting\n\r");
#endif
			target_slots.push_back(my_slot);
        }
    }
    // Choose the one later in the DFG
    // Slot* final_slot = NULL;
    for (auto itr = target_slots.begin(); itr != target_slots.end(); ++itr) {
    	Slot* target_slot = *itr;
    	// Dirty O(n^2) approach which is actually O(1) ;)
    	bool latest = true;
    	for (auto it = machines.begin(); it!=machines.end(); ++it) {
    		Slot* comp_slot = *it;
    		if (target_slot->cur->app_id == comp_slot->cur->app_id) {
                if (target_slot->cur->id < comp_slot->cur->id) {
                    latest = false;
                }
            }
    	}
        if (latest) {
            return target_slot;
        }
	}
    return NULL;
}

bool dead_task(std::vector<Slot*>& machines, Task* task) {
    Application* parent_app = task->app_id;
    int num_deps = parent_app->num_deps[task->id];
    for (int i = 0; i < num_deps; i++) {
        int dep_index = parent_app->dependency_list[task->id][i];
        Task* dep_task = parent_app->tasks[dep_index];
        if (dep_task->batch_completed < dep_task->batch) {
            // this dependent is not done
            // Check if in a machine
            bool found = false;
            for (auto it = machines.begin(); it != machines.end(); ++it){
                Slot* my_slot = *it;
                if (my_slot -> cur == dep_task) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                return true;
            }
        }
    }
    return false;
}

Slot* preempt_nimblock(std::vector<Slot*>& machines, Task* ready_task, u64 cur_time) {
    u64 wait_time = ready_task->app_id->arrival_time - cur_time;
    u64 time_left = ready_task->latency*(ready_task->batch - ready_task->batch_completed);
    u64 base_time = ready_task->latency*(ready_task->batch);
    u64 projected = wait_time + time_left;
    Slot* preempt_slot = NULL;
    double worst_score;
    double ready_score = 1.0*projected/base_time*ready_task->app_id->priority_level;
    for (auto it = machines.begin(); it != machines.end(); ++it) {
        Slot* my_slot = *it;
        Task* slot_task = my_slot->cur;
        if (my_slot->status == WAITING) {
			if (dead_task(machines, slot_task)) {
				// Dead applications are evicted first
				return my_slot;
			}
			// If same application, don't preempt this slot
			if (slot_task -> app_id == ready_task->app_id) {
				continue;
			}
			// Calculate current slots score
			wait_time = slot_task->app_id->arrival_time - cur_time;
			time_left = slot_task->latency*(slot_task->batch - slot_task->batch_completed);
			base_time = slot_task->latency*(slot_task->batch);
			projected = wait_time + time_left;
			double slot_score = 1.0*projected/base_time*slot_task->app_id->priority_level;
			if (preempt_slot == NULL) {
				preempt_slot = my_slot;
				worst_score = slot_score;
			} else if (slot_score < worst_score) {
				preempt_slot = my_slot;
				worst_score = slot_score;
			}
        }
    }
    // TODO modify threshold so that the gap has to be larger than some alpha value
    if (worst_score*6.0 < ready_score) {
    	// Double check preempt slot is the lowest one
        return preempt_slot;
    }
    return NULL;
}

/*
 * Deallocates buffers for a task and updates book keeping for a single batch
 */
void finish_batch(Slot* slot) {
    slot->cur->batch_completed += 1;
    Task* my_task = slot->cur;
    // TODO at some point need to handle buffers changing here
    if (my_task->arg_protocols != NULL) {
        // New way where buffers are changed/preserved at each batch
        // my_task->old_arg_buffers.push(my_task->arg_buffers);
        // for (int i = 0; i < my_task->num_args; i++) {
        //     UINTPTR buf = my_task->arg_buffers[i];
        //     int my_size = my_task->arg_sizes[i];
        //     if(my_size > 0 && my_task->arg_protocols[i] != PASS) {
        //         free((void*) buf);
        //     }
        // }
    } else {
        // Do nothing, only one set of buffers is going to be used
    }
}

/*
 * Updates priorities for all tasks in the ready queue based on waiting time and on priority
 * Returns token threshold
 */
double update_priorities(std::list<Task*>& tasks, std::vector<Slot*> machines) {
    u64 cur_time;
    XTime_GetTime(&cur_time);
    double max_tokens = 0;
    for (auto it = tasks.begin(); it != tasks.end(); ++it) {
        Task* cur_task = *it;
        double current_tokens = cur_task -> priority;
        int priority_class = cur_task->app_id->priority_level;
        u64 degradation = cur_time - cur_task->app_id->arrival_time;
        // Convert degradation to us
        double deg = 1.0 * degradation / (COUNTS_PER_SECOND / 1000000);
        u64 latency = cur_task->latency*cur_task->batch; // Latencies are in us
        if (cur_task->latency > 0) {
            cur_task->priority = current_tokens + priority_class*deg/(latency);
        }
        if (cur_task->priority > max_tokens) {
            max_tokens = cur_task->priority;
        }
    }
    // Give tokens to currently running tasks as well
    for (auto it = machines.begin(); it != machines.end(); ++it) {
        Slot* cur_slot = *it;
        Task* cur_task = cur_slot->cur;
        double current_tokens = cur_task -> priority;
        int priority_class = cur_task->app_id->priority_level;
        u64 degradation = cur_time - cur_task->app_id->arrival_time;
        // Convert degradation to us
        double deg = 1.0 * degradation / (COUNTS_PER_SECOND / 1000000);
        u64 latency = cur_task->latency*cur_task->batch; // Latencies are in us
        if (cur_task->latency > 0) {
            cur_task->priority = current_tokens + priority_class*deg/(latency);
        }
    }
    if (max_tokens > 9.0) {
        return 9.0;
    } else if (max_tokens > 3.0) {
        return 3.0;
    }
    return 1.0;
}

/*
 * At this point, the task has completed fully.
 * Buffers can be deallocated (if not passed to successor)
 * Timing information is updated and slot is cleaned out
 */
void finish_task(Slot* slot) {
    slot->status = IDLE;
    slot->cur->app_id->occupancy -= 1;
    slot->cur->machine = NULL;
    slot->cur->app_id->tasks_completed +=1;
    // TODO need to preserve some buffers
    Task* my_task = slot->cur;
    XTime_GetTime(&(my_task->tEnd_ip));
    if (my_task->arg_protocols == NULL) {
        // Old way of handling arguments, with only 1 set of buffers for each app
        // And no passing. To be removed later
        for (int i = 0; i < my_task->num_args; i++) {
            UINTPTR buf = my_task->arg_buffers[i];
            int my_size = my_task->arg_sizes[i];
            if(my_size > 0) {
                free((void*) buf);
            }
        }
    } else {
        for (int i = 0; i < my_task->num_args; i++) {
            UINTPTR buf = my_task->arg_buffers[i];
            int my_size = my_task->arg_sizes[i];
            if(my_size > 0 && my_task->arg_protocols[i] != PASS) {
                free((void*) buf);
            }
        }
    }
}

/*
 * Returns true if an application is retired
 */
bool retire_app(std::list<Application*>& pending_apps, std::vector<Application*>& finished_apps) {
    bool ret_val = false;
    for (auto it = pending_apps.begin(); it != pending_apps.end();) {
        Application* cur_app = *it;
        bool flag = true;
        for (auto t = cur_app->tasks.begin(); t != cur_app->tasks.end(); ++t) {
            Task* cur_task = *t;
            if (!is_done(cur_task)) {
                flag = false;
            }
        }
        if (flag){
            // Remove from list and move to other
            printf("[INFO]: Retiring application with name %s and batch count %d and priority %d\n\r", cur_app->app_name, cur_app->batch, cur_app->priority_level);
            it = pending_apps.erase(it);
            finished_apps.push_back(cur_app);
            XTime_GetTime(&(cur_app->end_time));
            ret_val = true;
        } else {
            ++it;
        }
    }
    return ret_val;
}

bool next_batch_ready(Task* task) {
    Application* parent_app = task->app_id;
    int batch_left = task->batch - task->batch_completed;
    int num_deps = parent_app->num_deps[task->id];
    for (int i = 0; i < num_deps; i++) {
        int dep_index = parent_app->dependency_list[task->id][i];
        Task* dep_task = parent_app->tasks[dep_index];
        int batch_left_dep = dep_task->batch - dep_task->batch_completed;
        if (batch_left_dep >= batch_left) {
            // In this case the dependency is not ahead
            // @TODO might need to change this for piplining...
            return false;
        }
    }
    return true;
}

/*
 * Iterates through all of the slots and checks if the task in that slot is running or not.
 * Completely finished tasks are removed from the ready_queue.
 */
void update_machine_state(std::vector<Slot*>& machines) {
    for (auto it = machines.begin(); it != machines.end(); ++it) {
        Slot* cur_slot = *it;
        Task* cur_task = cur_slot->cur;
        if (cur_slot->status == RUNNING) {
            if(XHls_ip_IsDone(&(cur_slot->accelerator)) || XHls_ip_IsReady(&(cur_slot->accelerator))) {
                // This slot is finished running!!!
//                printf("[INFO]: Finished execution of task %s_%d in slot %d\n\r", cur_task->app_id->app_name, cur_task->id, cur_slot->id);
                finish_batch(cur_slot);
                // If all batch is done, finish task, else launch next batch or stall
                if (is_done(cur_task)) {
#ifdef DEBUG
					printf("[INFO]: Finished execution of task %s_%d in slot %d\n\r", cur_task->app_id->app_name, cur_task->id, cur_slot->id);
#endif
                    finish_task(cur_slot);
                    cur_slot->cur = NULL;
                } else {
                    cur_slot->status = WAITING;
#ifdef DEBUG
                    printf("[DEBUG]: Unable to launch next batch immediately, sending application to wait...\n\r");
#endif
                }
            }
        }
        else if (cur_slot->status == WAITING && XHls_ip_IsReady(&(cur_slot->accelerator))) {
            // See if can continue with next batch
            if (next_batch_ready(cur_slot->cur)) {
                set_params(cur_slot->cur, cur_slot); // TODO may need to fix buffers and things...
                cur_slot->status = RUNNING;
#ifdef DEBUG
                printf("[DEBUG]: Relaunching previously stalled accelerator: %s, %d for batch %d\n\r", cur_slot->cur->app_id->app_name, cur_slot->cur->id, cur_slot->cur->batch_completed);
#endif
                XHls_ip_Start(&(cur_slot->accelerator));
            }
        }
    }
}

/*
 * Display timing information
 */
void print_timers(std::vector<Application*>& completed_apps) {
    printf("****** TIMING ******\n\r");
    for (auto it = completed_apps.begin(); it!=completed_apps.end(); ++it) {
        Application* my_app = *it;
        printf("****** New App ******\n\r");
        // Task buffers are freed when task retires
        printf("App is named %s\n\r", my_app->app_name);
        printf("App batch size is: %d, App user priority is %d\n\r", my_app->batch, my_app->priority_level);
        // u64 last_end = 0;
        u64 first_start = 0;
        for (auto itr = my_app->tasks.begin(); itr!=my_app->tasks.end(); ++itr){
            Task* my_task = *itr;
            printf("\tTask id: %d\n\r", my_task->id);
            printf("\t\tRun time: %fus - %fus = %fus\n\r",
					1.0 * (my_task->tEnd_ip) / (COUNTS_PER_SECOND / 1000000),
					1.0 * (my_task->tStart_ip) / (COUNTS_PER_SECOND / 1000000),
					1.0 * (my_task->tEnd_ip - my_task->tStart_ip) / (COUNTS_PER_SECOND / 1000000));
            printf("\t\tPR time: %fus - %fus = %fus\n\r",
					1.0 * (my_task->tEnd_pr) / (COUNTS_PER_SECOND / 1000000),
					1.0 * (my_task->tStart_pr) / (COUNTS_PER_SECOND / 1000000),
					1.0 * (my_task->tEnd_pr - my_task->tStart_pr) / (COUNTS_PER_SECOND / 1000000));
            // if (my_task->tEnd_ip > last_end) {
            //     last_end = my_task->tEnd_ip;
            // }
            if (first_start == 0) {
                first_start = my_task->tStart_pr;
            } else if(first_start > my_task->tStart_pr) {
                first_start = my_task->tStart_pr;
            }
        }
        printf("Application info: \n\r");
        printf("Response time: %fus - %fus = %fus\n\r",
					1.0 * (my_app->end_time) / (COUNTS_PER_SECOND / 1000000),
					1.0 * (my_app->arrival_time) / (COUNTS_PER_SECOND / 1000000),
					1.0 * (my_app->end_time - my_app->arrival_time) / (COUNTS_PER_SECOND / 1000000));
        printf("Latency is: %fus - %fus = %fus\n\r", 
                    1.0 * (my_app->end_time) / (COUNTS_PER_SECOND / 1000000),
					1.0 * (first_start) / (COUNTS_PER_SECOND / 1000000),
					1.0 * (my_app->end_time - first_start) / (COUNTS_PER_SECOND / 1000000));
    }
}

void print_snapshot(std::vector<double> event_delays, std::vector<int> batch_nums, std::vector<int> event_nums, std::vector<int> priority_levels) {
    printf("Event delays:\n\r");
    printf("{");
    for (int i = 0; i < event_delays.size(); ++i) {
        if (i > 0) {
            printf(", %f", event_delays[i]);
        } else {
            printf("%f", event_delays[i]);
        }
    }
    printf("}\n\r");

    printf("Batch_nums:\n\r");
    printf("{");
    for (int i = 0; i < batch_nums.size(); ++i) {
        if (i > 0) {
            printf(", %d", batch_nums[i]);
        } else {
            printf("%d", batch_nums[i]);
        }
    }
    printf("}\n\r");

    printf("Event Nums:\n\r");
    printf("{");
    for (int i = 0; i < event_nums.size(); ++i) {
        if (i > 0) {
            printf(", %d", event_nums[i]);
        } else {
            printf("%d", event_nums[i]);
        }
    }
    printf("}\n\r");

    printf("Priority Levels:\n\r");
    printf("{");
    for (int i = 0; i < priority_levels.size(); ++i) {
        if (i > 0) {
            printf(", %d", priority_levels[i]);
        } else {
            printf("%d", priority_levels[i]);
        }
    }
    printf("}\n\r");
}

/*
 * Avoid memory leaks by freeing all memory for applications
 */
void cleanup_structures(std::vector<Application*>& completed_apps, std::vector<Slot*>& machines) {
    for (auto it = completed_apps.begin(); it!=completed_apps.end(); ++it) {
        Application* my_app = *it;
        // Task buffers are freed when task retires
         for (auto itr = my_app->tasks.begin(); itr!=my_app->tasks.end(); ++itr){
             Task* my_task = *itr;
        //     // Free buffers for task
        //     for (int i = 0; i < my_task->arg_buffers.size(); i++) {
        //         if (my_task->arg_sizes[i] > 0) {
        //             free((void*)(my_task->arg_buffers[i])); // Using free here because we used aligned alloc before
        //         }
        //     }
            delete my_task;
        }
        delete my_app;
    }
}

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
/*
 * Initialize slots
 */

int init_slots(XGpioPs* psGpioInstancePtr, std::vector<Slot*>& machines) {
    for (int i = 0; i < NUM_SLOTS_ACTIVE; i++) {
		// Initializing reset and setting to one
		Slot* my_slot = new Slot();
		my_slot->id = i;
		my_slot->batch_count = -1;
		my_slot->cur = NULL;
		my_slot->status = IDLE;
		my_slot->decouple_gpio = decouple_gpio[i];
		my_slot->reset_gpio = reset_gpio[i];
		my_slot->bin_len = bin_len[i];

		XGpioPs_SetDirectionPin(psGpioInstancePtr, my_slot->reset_gpio, 0x1);
		XGpioPs_SetOutputEnablePin(psGpioInstancePtr, my_slot->reset_gpio, 1); // Setting direction to output
		XGpioPs_WritePin(psGpioInstancePtr, my_slot->reset_gpio, 1);

		// Initializing decouple and setting to zero
		XGpioPs_SetDirectionPin(psGpioInstancePtr, my_slot->decouple_gpio, 0x1);
		XGpioPs_SetOutputEnablePin(psGpioInstancePtr, my_slot->decouple_gpio, 1);
		XGpioPs_WritePin(psGpioInstancePtr, my_slot->decouple_gpio, 0);

		// Initializing accelerator
		XHls_ip_Config accelerator_config;
		accelerator_config.DeviceId = 0; // Can this just be zero? Or do we need to set it to the value in xparams?
		accelerator_config.Axilites_BaseAddress = slot_baseaddr[i];
		int status = XHls_ip_CfgInitialize(&(my_slot->accelerator), &accelerator_config);
		if(status != XST_SUCCESS) {
			printf("[ERROR] Initialization of slot %d failed.\n\r", i);
            return 1;
		}
		machines.push_back(my_slot);
	}
    return 0;
}
