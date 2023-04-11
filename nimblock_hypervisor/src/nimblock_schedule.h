#ifndef NIMBLOCK_SCHEDULE_H
#define NIMBLOCK_SCHEDULE_H
#include "nimblock_types.h"
#include "configure.h"
#include "3dr_input.h"
#include <list>
#include <vector>
#include <algorithm>
/* Should probably make this a method of the task class ... */
bool is_ready(Task* task);
bool is_done(Task* task);
int update_ready_tasks(std::list<Task*>& pending_tasks, std::list<Task*>& ready_tasks) ;
Task* select_task(std::list<Task*>& ready_tasks);
int configure_task(XGpioPs& psGpioInstancePtr, XFpga XFpgaInstance, Task* my_task, Slot* slot);
Slot* select_slot(std::vector<Slot*>& machines);
void launch_task(Task* my_task, Slot* slot);
int allocate_buffers(Task* task);
int set_params(Task* task, Slot* slot);
void update_machine_state(std::vector<Slot*>& machines);
bool retire_app(std::list<Application*>& pending_apps, std::vector<Application*>& finished_apps);
void cleanup_structures(std::vector<Application*>& completed_apps, std::vector<Slot*>& machines);
double update_ready_tasks_nimblock(std::list<Task*>& pending_tasks, std::list<Task*>& ready_tasks, double threshold);
bool compare_tasks(Task* t1, Task* t2);
Task* select_task_nimblock(std::list<Task*>& ready_tasks, double threshold);
int init_slots(XGpioPs* psGpioInstancePtr, std::vector<Slot*>& machines);
double update_priorities(std::list<Task*>& tasks, std::vector<Slot*> machines);
void print_timers(std::vector<Application*>& completed_apps);
Slot* preempt(std::vector<Slot*>& machines, Task* ready_task, u64 cur_time);
bool next_batch_ready(Task* task);
void print_snapshot(std::vector<double> event_delays, std::vector<int> batch_nums, std::vector<int> event_nums, std::vector<int> priority_levels);
Slot* preempt_nimblock(std::vector<Slot*>& machines, Task* ready_task, u64 cur_time);
bool dead_task(std::vector<Slot*>& machines, Task* task);
#endif /*NIMBLOCK_SCHEDULE_H*/
