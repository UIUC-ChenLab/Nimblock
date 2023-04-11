import re
import numpy as np
import math
import matplotlib.pyplot as plt
from dataclasses import dataclass, field

PR_DELAY = 34
APPLICATION_OPTIMAL_LATENCY = {
    "Lenet": {"offset": 149, "factor": 47},
    "Optical Flow": {"offset": 4787, "factor": 1638},
    "Image Compression": {"offset": 79, "factor": 20},
    "3DR": {"offset": 473, "factor": 273},
    "KNN": {"offset": 655612, "factor": 655517},
    "Alexnet": {"offset": 100, "factor": 5} #TODO need to fix this
}
APPLICATIONS = {"Lenet": {"lenet_k01": 46, "lenet_k02": 33, "lenet_k03": 34
                },
                "Optical Flow": {'of_k01': 586,
                                'of_k02': 97,
                                'of_k03': 96,
                                'of_k04': 545,
                                'of_k05': 555,
                                'of_k06': 574,
                                'of_k07': 575,
                                'of_k08': 576,
                                'of_k09': 1148
                },
                "Image Compression": { "ic_k01": 17,
                                        "ic_k02": 8,
                                        "ic_k03": 5,
                                        "ic_k04": 5,
                                        "ic_k05": 8,
                                        "ic_k06": 1,
                },
                "KNN": {'dr_k01': 52,
                                        'dr_k02': 655517,
                                        'dr_k03': 8
                },
                "3DR": {'3dr_k01': 273,
                                    '3dr_k02': 111,
                                    '3dr_k03': 54
                },
                "Alexnet": {
                    "TODO": 34, # TODO need to fix this too
                }
}
APPLICATIONS_BOARD = {"Lenet": {"lenet_k01": 112687, "lenet_k02": 115486, "lenet_k03": 115501
                },
                "Optical Flow": {'of_k01': 730678,
                                'of_k02': 123812,
                                'of_k03': 122429,
                                'of_k04': 446138,
                                'of_k05': 447628,
                                'of_k06': 730681,
                                'of_k07': 731025,
                                'of_k08': 731127,
                                'of_k09': 398521
                },
                "Image Compression": { "ic_k01": 19595,
                                        "ic_k02": 19595,
                                        "ic_k03": 19594,
                                        "ic_k04": 19593,
                                        "ic_k05": 19593,
                                        "ic_k06": 19593,
                },
                "KNN": {'dr_k01': 65534,
                                        'dr_k02': 196898762,
                                        'dr_k03': 7971
                },
                "3DR": {'3dr_k01': 273,
                                    '3dr_k02': 111,
                                    '3dr_k03': 54
                },
                "Alexnet": {
                    "TODO": 276, # TODO need to fix this
                }
}

@dataclass()
class app:
    arrival_time: float
    finish_time: float
    response_time: float
    priority: int
    batch_size: int
    name: str
    latency: float=0
    latency_rep: float=0
    run_time: float=0
    pr_time: float=0
    wait_time: float=0

@dataclass()
class grouped_app:
    rr: app
    fcfs: app
    optimal: app
    base: app
    nopreempt: app
    application: app
    latency: float

def parse_runs(file):
    new_run = False
    runs = []
    run = []
    pr_times = []
    run_times = []
    my_app = None
    with open(file, "r") as f:
        
        for line in f:
            r = re.search("(.*)Starting iteration (.*)\n", line)
            if r is not None:
                new_run = True
                run = []
            
            if new_run:
                r = re.search("App is named (.*)\n", line)
                if r is not None:
                  
                    my_app = app(0, 0, 0, 0, 0, r.group(1))
                    pr_times = []
                    run_times = []
                r = re.search("App batch size is: (.*), App user priority is (.*)\n", line)
                if r is not None:
                    my_app.batch_size = int(r.group(1))
                    my_app.priority = int(r.group(2))
                r = re.search("(.*)Run time: (.*)us - (.*)us = (.*)us\n", line)
                if r is not None:
                    run_time = float(r.group(4))
                    my_app.run_time = my_app.run_time + run_time
                r = re.search("(.*)PR time: (.*)us - (.*)us = (.*)us\n", line)
                if r is not None:
                    pr_start = float(r.group(3))
                    pr_time = float(r.group(4))
                    pr_times.append(pr_start)
                    my_app.pr_time = my_app.pr_time + pr_time
                r = re.search("Response time: (.*)us - (.*)us = (.*)us\n", line)
                if r is not None:
                    
                    my_app.finish_time = float(r.group(1))
                    my_app.arrival_time = float(r.group(2))
                    my_app.response_time = float(r.group(3))
                    min_pr = min(pr_times)
                    my_app.latency = my_app.finish_time - min_pr 
                r = re.search("Latency is: (.*)us - (.*)us = (.*)us\n", line)
                if r is not None:     
                    my_app.latency_rep = float(r.group(3))
                    #print(my_app.name, " ", pr_times)
                    #my_app.run_time = float(sum(run_times))
                    #my_app.pr_time = float(sum(pr_times))
                    my_app.wait_time = float(my_app.response_time-my_app.latency_rep)     
                    run.append(my_app)
                
                r = re.search("(.*)Finished iteration (.*)\n", line)
                if r is not None:
                    new_run = False
                    runs.append(run)
    return runs

def calculate_base(a: app):
    """
    Application response time if it can be scheduled instantly and there's only 1 tile/slot
    No pipelining
    PR between each stage
    Worst case estimate if no waiting time
    Uses isolated board benchmarking instead of HLS estimates
    """
    name = a.name
    batch = a.batch_size
    sum = 0
    for _, value in APPLICATIONS_BOARD[name].items():
        sum+=batch*value
        sum+=PR_DELAY
    return sum #(sum*15500)*(10)/1000 # convert to us format

def calculate_optimal(a: app):
    """
    Application response time if it can be scheduled instantly and there is infinite hardware
    Pipelining included
    PR can be covered
    Best case estimate if no waiting time
    """
    name = a.name
    batch = a.batch_size
    lat = APPLICATION_OPTIMAL_LATENCY[name]['offset'] + APPLICATION_OPTIMAL_LATENCY[name]['factor']*batch
    return (lat*15500)*(10)/1000 # convert to us format

def group_apps(rr_runs, FCFS_runs, base_runs, better_base_runs, no_preempt, app_runs):
    grouped_runs = [] # array of arrays of grouped apps
    for i in range(len(rr_runs)):
        n_run = rr_runs[i]
        f_run = FCFS_runs[i]
        b_run = base_runs[i]
        bb_run = better_base_runs[i]
        np_run = no_preempt[i]
        a_run = app_runs[i]
        grouped_apps = []
    
        for n_app in n_run:
            
            # find index of matching event in FCFS
            apps = []
            check = 0
            for run in [f_run, bb_run, b_run, np_run, a_run]:
                check+=1
               
                matching_indicies = []
                for i, f_app in enumerate(run):
                  
                    if f_app.priority == n_app.priority and f_app.batch_size == n_app.batch_size and f_app.name == n_app.name:
                        matching_indicies.append(i)
                        
                #print(len(matching_indicies))
                #print(run)
                assert(len(matching_indicies) > 0)
                if len(matching_indicies) > 1:
                    # pick closest and match
                    my_arrival = n_app.arrival_time
                    diffs = []
                    for i in matching_indicies:
                        diffs.append(math.fabs(run[i].arrival_time - my_arrival))
                    min_value = min(diffs)
                    min_index = diffs.index(min_value)
                    match = matching_indicies[min_index]
                    f_app = run.pop(match)
                    apps.append(f_app)
                    # g = grouped_app(n_app, f_app, calculate_optimal(n_app), calculate_base(n_app))
                    # grouped_apps.append(g)
                else:
                    f_app = run.pop(matching_indicies[0])
                    apps.append(f_app)
                    # g = grouped_app(n_app, f_app, calculate_optimal(n_app), calculate_base(n_app))
                    # grouped_apps.append(g)
            g = grouped_app(n_app, apps[0], apps[1], apps[2], apps[3], apps[4], apps[2].latency)
            assert(g.base.latency == g.latency)
            grouped_apps.append(g)
        grouped_runs.append(grouped_apps)
    return grouped_runs

def main():
    pass

if __name__ == "__main__":
    main()