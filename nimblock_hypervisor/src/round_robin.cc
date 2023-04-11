#include "round_robin.h"

Slot* select_slot_rr(std::vector<Slot*>& machines) {
    for(auto it = machines.begin(); it!= machines.end(); ++it) {
        Slot* my_slot = *it;
        if (!my_slot->tasks.empty() && my_slot -> status == IDLE) {
            return my_slot;
        }
    }
    return NULL;
}

Task* select_task_rr(Slot* selected_slot) {
    if (selected_slot == NULL) {
        return NULL;
    }
    if (selected_slot->tasks.empty()) {
        return NULL;
    }
    Task* my_task = selected_slot->tasks.top();
    selected_slot->tasks.pop();
    return my_task;
}

size_t q_size(Slot* my_slot) {
    size_t my_size = my_slot -> tasks.size();
    if (my_slot->status != IDLE) {
        my_size++;
    }
    return my_size;
}

Task* next_task_arb(Application* target_app) {
    Task* my_task;
    for (auto it = target_app->tasks.begin(); it!=target_app->tasks.end(); ++it) {
        my_task = *it;
        if (!is_done(my_task) && is_ready(my_task) && (my_task->machine == NULL) && !my_task->queued) {
            return my_task;            
        }
    }
    return NULL;
}

Slot* smallest_machine_queue(std::vector<Slot*>& machines) {
    Slot* smallest_slot = NULL;
    size_t smallest_size = 0;
    for(auto it = machines.begin(); it!=machines.end(); ++it) {
        Slot* my_slot = *it;
        size_t my_size = q_size(my_slot);
        if (smallest_slot == NULL)  {
            smallest_slot = my_slot;
            smallest_size = my_size;
        } else if (my_size < smallest_size) {
            smallest_size = my_size;
            smallest_slot = my_slot;
        }
    }
    return smallest_slot;
}

/*
 * Iterate through pending applications and move one ready task from each to the smallest queue
 * queue size is equal to size of priority queue object + 1 if status is not IDLE
 */
void arbitrate(std::vector<Slot*>& machines, std::list<Application*>& pending_apps) {
    for (auto it = pending_apps.begin(); it != pending_apps.end(); ++it) {
        Application* my_app = *it;
        Task* my_task = next_task_arb(my_app);
        if (my_task != NULL) {
            Slot* my_slot = smallest_machine_queue(machines);
            if (my_slot != NULL) {
                my_slot -> tasks.push(my_task);
                my_task->queued = true;
            } else {
                printf("[ERROR]: No valid queue to add to");
                return;
            }
        }
    }
}
