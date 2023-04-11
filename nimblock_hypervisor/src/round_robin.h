#ifndef ROUND_ROBIN_H
#define ROUND_ROBIN_H
#include "nimblock_types.h"
#include "nimblock_schedule.h"
#include "nimblock_schedule_apps.h"

Slot* select_slot_rr(std::vector<Slot*>& machines);
Task* select_task_rr(Slot* selected_slot);
size_t q_size(Slot* my_slot);
Task* next_task_arb(Application* target_app);
Slot* smallest_machine_queue(std::vector<Slot*>& machines);
void arbitrate(std::vector<Slot*>& machines, std::list<Application*>& pending_apps);
#endif /* ROUND_ROBIN_H */
