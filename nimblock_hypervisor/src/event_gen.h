#ifndef EVENT_GEN_H
#define EVENT_GEN_H

#include "nimblock_types.h"
#include <stdlib.h>
// #include <unordered_map>
#include <list>
#include <vector>
#include <math.h>


int pick_new_app();
int generate_batch_size();
int generate_priority_level();
Application* add_new_event(std::list<Application*>& pending_applications, std::list<Task*>& pending_tasks);
int generate_priority_level();
double next_event_delay(int min, int max);
int calculate_slots(float coeffs[FUNC_COEFFS], int batch);

#ifdef REPLAY
double next_event_delay_replay(int i, int j);
Application* add_new_event_replay(std::list< Application*>& pending_applications, std::list<Task*>& pending_tasks, int i, int j);
#endif
#endif /* EVENT_GEN_H */
