#include "nimblock_schedule_apps.h"

#include <list>
#include <vector>
#include <algorithm>
#include <math.h>

/*
 * Moves apps from pending_apps to candidate apps when their priority is above the threshold
 * Divide resources amongst candidate apps
 */
bool update_candidate_apps(std::list<Application*>& pending_apps, std::list<Application*>& candidate_apps, double threshold) {
    bool rethresh = false;
    for (auto it = pending_apps.begin(); it!=pending_apps.end();) {
        Application* cur_app = *it;
        if (cur_app->priority >= threshold){
            // Remove from list and move to other
            it = pending_apps.erase(it);
            candidate_apps.push_back(cur_app);
            // std::push_heap(ready_tasks.begin(), ready_tasks.end(), compare_tasks);
            rethresh = true;
            printf("[TESTING]: Moving application from pending to candidate with priority %f, threshold was %f\n\r", cur_app->priority, threshold);
        } else {
            ++it;
        }
    }
    // Move apps that are no longer above the threshold need to be moved out
    // This only happens if a new high priority application arrives and suddenly
    // Shifts the threshold up. Only could affect new, low priority apps
    for (auto it = candidate_apps.begin(); it!=candidate_apps.end();) {
        Application* cur_app = *it;
        if (cur_app->priority < threshold){
            // Remove from list and move to other
            it = candidate_apps.erase(it);
            // Reset threshold for app, its no longer a candidate
            cur_app -> threshold =  0; //-1*NUM_SLOTS_ACTIVE; // By using a large negative number, this one is always the most overutilized
            pending_apps.push_back(cur_app);
            rethresh = true;
            // std::push_heap(ready_tasks.begin(), ready_tasks.end(), compare_tasks);
        } else {
            ++it;
        }
    }
    return rethresh;
}

/* 
 * Define how many slots this application should get
 * This should quantify the benefit from having many slots
 */
// I don't think goal should depend on priority...maybe only depend on
// Characteristics
int calculate_goal(Application* my_app) {
	// return 10; // This performs the best so far
    int slot_goal = 0;
    slot_goal += my_app->parallelism;
    return slot_goal;
}

/*
 * Reset thresholds for resource usage based on current candidate apps
 */
#define THRESHOLD_LEVEL 1
void allocate_resources(std::list<Application*>& candidate_apps) {
    int assigned_slots = 0;
    // first pass gives away lowest number of slots possible
    for (auto it = candidate_apps.begin(); it != candidate_apps.end(); ++it) {
        Application* my_app = *it;
        int tasks_left = my_app->tasks.size() - my_app->tasks_completed;
        int slots = std::min(THRESHOLD_LEVEL, tasks_left);
        my_app->threshold = slots;
        assigned_slots += slots;
    }

    // second pass updates to goal slots
      if (assigned_slots < NUM_SLOTS_ACTIVE) {
          // Assign extra slots as needed
          for (auto it = candidate_apps.begin(); it != candidate_apps.end(); ++it) {
              Application* my_app = *it;
              int tasks_left = my_app->tasks.size() - my_app->tasks_completed;
              int my_goal = calculate_goal(my_app);
              int goal_slots = std::min(my_goal, tasks_left);
              int slot_diff = goal_slots - my_app->threshold;
              my_app->threshold += slot_diff;
              assigned_slots += slot_diff;
              // Do I need to break here?
          }
      }

    // second pass that allocates one at a time until we reach threshold
//    while(assigned_slots < NUM_SLOTS_ACTIVE) {
//        int assigned_slots_prev = assigned_slots;
//        for (auto it = candidate_apps.begin(); it != candidate_apps.end(); ++it) {
//            Application* my_app = *it;
//            int tasks_left = my_app->tasks.size() - my_app->tasks_completed;
//            int my_goal = calculate_goal(my_app);
//            int goal_slots = std::min(my_goal, tasks_left);
//            int slot_diff = goal_slots - my_app->threshold;
//            my_app->threshold += slot_diff;
//            if (slot_diff > 0) {
//                // Slots are given one at a time
//                assigned_slots += 1;
//            }
//            if (assigned_slots >= NUM_SLOTS_ACTIVE) {
//                // Surpassed threshold
//                break;
//            }
//        }
//        if (assigned_slots == assigned_slots_prev) {
//            // No more apps need slots
//            break;
//        }
//    }
    // Second pass that normalizes expectations and divides slots appropriately
    // calculate total goal slots
//    int total_goal_slots = 0;
//    assigned_slots = 0;
//    for (auto it = candidate_apps.begin(); it != candidate_apps.end(); ++it) {
//        Application* my_app = *it;
//        int tasks_left = my_app->tasks.size() - my_app -> tasks_completed;
//        int my_goal = calculate_goal(my_app);
//        int goal_slots = std::min(my_goal, tasks_left);
//        total_goal_slots += goal_slots;
//    }
//    // Divide and normalize
//    for (auto it = candidate_apps.begin(); it != candidate_apps.end(); ++it) {
//        Application* my_app = *it;
//        int tasks_left = my_app->tasks.size() - my_app -> tasks_completed;
//        int my_goal = calculate_goal(my_app);
//        int goal_slots = std::min(my_goal, tasks_left);
//        int s = static_cast<int>(ceil(1.0*goal_slots/total_goal_slots));
//        my_app->threshold = s;
//        assigned_slots += s;
//    }

    // TODO Deadline aware second pass? Closest app to deadline gets more resources

    // final pass allocates any remaining slots by normalizing and proportioning?
    if (assigned_slots < NUM_SLOTS_ACTIVE) {
        // Assign extra slots as needed
        for (auto it = candidate_apps.begin(); it != candidate_apps.end(); ++it) {
            Application* my_app = *it;
            int tasks_left = my_app->tasks.size() - my_app->tasks_completed;
            int slot_diff = tasks_left - my_app->threshold;
            if (slot_diff > 0) {
                // extra slots are available so lets try to use them!
                my_app->threshold += slot_diff;
                assigned_slots += slot_diff;
                // Do I need to break here?
            }
        }
    }
}

/*
 * Update priorities for all apps in pending apps queue and candidate apps queue
 * Returns a new threshold based on token counts.
 */
double update_priorities_app(std::list<Application*>& pending_apps, std::list<Application*>& candidate_apps) {
    // Get most up to date time
    u64 cur_time;
    XTime_GetTime(&cur_time);
    double max_tokens = 0;
    // Update tokens in pending_app queue
    for (auto it = pending_apps.begin(); it != pending_apps.end(); ++it) {
        Application* cur_app = *it;
        double current_tokens = cur_app -> priority;
        int priority_class = cur_app->priority_level;
        u64 degradation = cur_time - cur_app->arrival_time;
        // Convert degradation to us
        double deg = 0.125 * 1.0 * degradation / (COUNTS_PER_SECOND / 1000000);
        // Calculate latency of this application
        u64 latency = 0; //cur_task->latency*cur_task->batch; // Latencies are in us
        if (cur_app->latency == 0) {
            for (auto itr = cur_app->tasks.begin(); itr!=cur_app->tasks.end(); ++itr) {
                Task* cur_task = *itr;
                latency += cur_task->latency*cur_task->batch;
            }
        } else {
            latency = cur_app->latency;
        }
        cur_app->priority = current_tokens + priority_class*deg/(latency);
        if (cur_app->priority > max_tokens) {
            max_tokens = cur_app->priority;
        }
    }
    // Update tokens in candidate apps
    for (auto it = candidate_apps.begin(); it != candidate_apps.end(); ++it) {
        Application* cur_app = *it;
        double current_tokens = cur_app -> priority;
        int priority_class = cur_app->priority_level;
        u64 degradation = cur_time - cur_app->arrival_time;
        // Convert degradation to us
        double deg = 0.125 * 1.0 * degradation / (COUNTS_PER_SECOND / 1000000);
        // Calculate latency of this application
        u64 latency = 0; //cur_task->latency*cur_task->batch; // Latencies are in us
        if (cur_app->latency == 0) {
            for (auto itr = cur_app->tasks.begin(); itr!=cur_app->tasks.end(); ++itr) {
                Task* cur_task = *itr;
                latency += cur_task->latency*cur_task->batch;
            }
        } else {
            latency = cur_app->latency;
        }
        cur_app->priority = current_tokens + priority_class*deg/(latency);
        if (cur_app->priority > max_tokens) {
            max_tokens = cur_app->priority;
        }
    }
    // TODO maybe add an extra priority level for old high priority applications
    // Give tokens to currently running tasks as well
    if (max_tokens > 9.0) {
        return 9.0;
    } else if (max_tokens > 3.0) {
        return 3.0;
    }
    return 1.0;
}

bool available_tasks(Application* my_app) {
    // bool avail = false
    for (auto it = my_app->tasks.begin(); it != my_app->tasks.end(); ++it) {
        Task* my_task = *it;
        if (is_ready(my_task) && !is_done(my_task) && (my_task->machine == NULL)) {
            return true;
        }
    }
    return false;
}

/* 
 * Select next task in the app to schedule
 */
Task* next_task(Application* target_app) {
    Task* my_task;
    for (auto it = target_app->tasks.begin(); it!=target_app->tasks.end(); ++it) {
        my_task = *it;
        if (!is_done(my_task) && is_ready(my_task) && (my_task->machine == NULL)) {
            return my_task;            
        }
    }
    return NULL;
}

/* 
 * Select which app to give resources to from candidate_apps
 */ 
Application* select_app(std::list<Application*> candidate_apps) {
    Application* candidate = NULL;
    for (auto it = candidate_apps.begin(); it != candidate_apps.end(); ++it) {
        // Break ties by choosing the high priority application
        Application* my_app = *it;
        if (my_app->occupancy < my_app->threshold && available_tasks(my_app)) {
            if (candidate == NULL) {
                candidate = my_app;
                return candidate;
            } else if (my_app -> priority_level > candidate-> priority_level) {
                candidate = my_app;
            }
        }
    }
    return candidate;
}

/* 
 * Returns the slot to preempt to if an application has overused its resource allocation
 */
Slot* preempt_slot_apps(std::vector<Slot*> machines, Task* my_task) {
    // TODO is it better to just preempt anything right away???
    int largest_diff = 0;
    Slot* diff_slot = NULL;
    for (auto it = machines.begin(); it != machines.end(); ++it) {
        // pick slot that exceeds its threshold by the most to take away slots
        Slot* cur_slot = *it;
        if (cur_slot ->status == WAITING) {
            int diff = cur_slot->cur->app_id->occupancy - cur_slot->cur->app_id->threshold;
            if (diff > largest_diff) {
                largest_diff = diff;
                diff_slot = cur_slot;
            } else if (diff == largest_diff && diff_slot != NULL) {
                // same application
                if(cur_slot->cur->app_id == diff_slot->cur->app_id){
                    if (cur_slot->cur->id > diff_slot->cur->id) {
                        // If id is later, this is the task from this app that should be preempted
                        // Always preempt the end of the DFG
                        diff_slot = cur_slot;
                    }
                }
            }
        } 
        // if slot is not waiting, but is later in dfg, we shouldn't preempt now!
        if (diff_slot != NULL && cur_slot->status == RUNNING) {
            if (diff_slot->cur->app_id == cur_slot->cur->app_id) {
                if(cur_slot->cur->id > diff_slot->cur->id) {
                    diff_slot = NULL;
                }
            }
        }
    }
    
    return diff_slot;
}

/*
 * Select task from candidate app
 */
Task* select_task_app(std::list<Application*>& candidate_apps) {
    Application* target_app = select_app(candidate_apps);
    if (target_app == NULL) {
        return NULL;
    }
    Task* my_task;
    for (auto it = target_app->tasks.begin(); it!=target_app->tasks.end(); ++it) {
        my_task = *it;
        if (!is_done(my_task) && is_ready(my_task) && (my_task->machine == NULL)) {
            return my_task;            
        }
    }
    return NULL;
}
