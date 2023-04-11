#ifndef NIMBLOCK_SCHEDULE_APPS_H
#define NIMBLOCK_SCHEDULE_APPS_H
#include "nimblock_types.h"
#include "nimblock_schedule.h"

bool update_candidate_apps(std::list<Application*>& pending_apps, std::list<Application*>& candidate_apps, double threshold);
void allocate_resources(std::list<Application*>& candidate_apps);
double update_priorities_app(std::list<Application*>& pending_apps, std::list<Application*>& candidate_apps);
bool available_tasks(Application* my_app);
Application* select_app(std::list<Application*> candidate_apps);
Slot* preempt_slot_apps(std::vector<Slot*> machines, Task* my_task);
int calculate_goal(Application* my_app);
Task* select_task_app(std::list<Application*>& candidate_apps);
#endif
