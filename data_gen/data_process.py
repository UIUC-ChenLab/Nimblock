from dataclasses import dataclass, field
import matplotlib.pyplot as plt 
import numpy as np

@dataclass()
class makespans:
    rr: float
    fcfs: float
    optimal: float
    base: float
    nopreempt: float
    application: float

def calculate_makespans(grouped_runs):
    makespans_grouped = []
    for run in grouped_runs:
        min_arrival_fcfs = None
        min_arrival_rr = None
        min_arrival_base = None
        min_arrival_better_base = None
        min_arrival_np = None
        min_arrival_app = None
        max_finish_fcfs = None
        max_finish_rr = None
        max_finish_base = None
        max_finish_better_base = None 
        max_finish_np = None
        max_finish_app = None
        for g in run:
            if min_arrival_fcfs is None:
                min_arrival_fcfs = g.fcfs.arrival_time
                min_arrival_rr = g.rr.arrival_time
                min_arrival_base = g.base.arrival_time
                min_arrival_better_base = g.optimal.arrival_time
                min_arrival_np = g.nopreempt.arrival_time
                min_arrival_app = g.application.arrival_time

                max_finish_fcfs = g.fcfs.finish_time
                max_finish_rr =g.rr.finish_time
                max_finish_base = g.base.finish_time
                max_finish_better_base = g.optimal.finish_time
                max_finish_np = g.nopreempt.finish_time
                max_finish_app = g.application.finish_time

            if g.fcfs.arrival_time < min_arrival_fcfs:
                min_arrival_fcfs = g.fcfs.arrival_time
            if g.rr.arrival_time < min_arrival_rr:
                min_arrival_rr = g.rr.arrival_time
            if g.base.arrival_time < min_arrival_base:
                min_arrival_base = g.base.arrival_time
            if g.optimal.arrival_time < min_arrival_better_base:
                min_arrival_better_base = g.optimal.arrival_time
            if g.nopreempt.arrival_time < min_arrival_np:
                min_arrival_np = g.nopreempt.arrival_time
            if g.application.arrival_time < min_arrival_app:
                min_arrival_app = g.application.arrival_time

            if g.fcfs.finish_time > max_finish_fcfs:
                max_finish_fcfs = g.fcfs.finish_time
            if g.rr.finish_time > max_finish_rr:
                max_finish_rr= g.rr.finish_time
            if g.base.finish_time > max_finish_base:
                max_finish_base = g.base.finish_time
            if g.optimal.finish_time > max_finish_better_base:
                max_finish_better_base = g.optimal.finish_time
            if g.nopreempt.finish_time > max_finish_np:
                max_finish_np = g.nopreempt.finish_time
            if g.application.finish_time > max_finish_app:
                max_finish_app = g.application.finish_time
        makespans_grouped.append(makespans(max_finish_rr-min_arrival_rr, max_finish_fcfs-min_arrival_fcfs, max_finish_better_base - min_arrival_better_base, max_finish_base - min_arrival_base, max_finish_np - min_arrival_np, max_finish_app-min_arrival_app))
    return makespans_grouped

def report_stats(grouped_all):
    rr = []
    base = []
    better_base = []
    fcfs = []
    nopreempt = []
    application = []
    for grouping in grouped_all:
        application.append(grouping.application.response_time/grouping.optimal.response_time)
        rr.append(grouping.rr.response_time/grouping.optimal.response_time)
        fcfs.append(grouping.fcfs.response_time/grouping.optimal.response_time)
        better_base.append(grouping.optimal.response_time/grouping.optimal.response_time)
        base.append(grouping.base.response_time/grouping.optimal.response_time)
        nopreempt.append(grouping.nopreempt.response_time/grouping.optimal.response_time)
    application = np.array(application)
    rr = np.array(rr)
    fcfs = np.array(fcfs)
    base = np.array(base)
    better_base = np.array(better_base)
    nopreempt = np.array(nopreempt)
    for dataset, name in zip([application, nopreempt, fcfs, rr], ["Nimblock", "PREMA", "FCFS", "RR"]):
        # print(name + " median response time is", np.median(dataset)*100, "%% of the baseline")
        # print(name + " average response time is", np.mean(dataset)*100, "%% of the baseline")
        # print(name + " first quartile response time is", np.percentile(dataset, 25, interpolation="linear")*100, "%% of the baseline")
        # print(name + " third quartile response time is", np.percentile(dataset, 75, interpolation="linear")*100, "%% of the baseline")
        print(name + " 95 percentile response time is ", np.percentile(dataset, 95, interpolation="linear")*100, "%% of the baseline")
        print(name + " 99 percentile response time is ", np.percentile(dataset, 99, interpolation="linear")*100, "%% of the baseline")
        # print(name + " worst case response time is", np.max(dataset)*100, "%% of the baseline")
    # print("PREMA median response time is", np.median(nopreempt)*100, "%% of the baseline")
    # print("PREMA average response time is", np.mean(nopreempt)*100, "%% of the baseline")

def calculate_deadline_violations(grouped_all, priority_level="high", event_speed="full"):
    plt.rcParams.update({'font.size' : 14})
    plt.rcParams.update({'font.family' : 'serif'})
    if priority_level == "high":
        grouped = [j for j in grouped_all if j.application.priority==9]
        t = "High"
    elif priority_level == "med":
        grouped = [j for j in grouped_all if j.application.priority==3]
        t = "Medium"
    elif priority_level == "low":
        grouped = [j for j in grouped_all if j.application.priority==1]
        t = "Low"
    else:
        raise ValueError("Invalid Priority level argument set")
    prema_fails = []
    nimblock_fails = []
    opt_base_fails = []
    fcfs_fails = []
    base_fails = []
    rr_fails = []
    cutoffs = [1+ (x*0.25) for x in range(80)]
    # cutoffs = [(x+1)*0.25 for x in range(80)]
    for latency_cutoff in cutoffs:
        prema_fail = 0
        nimblock_fail = 0
        fcfs_fail = 0
        opt_base_fail = 0
        base_fail = 0
        rr_fail = 0
        for grouping in grouped:
            deadline = grouping.latency*latency_cutoff
            if grouping.application.response_time > deadline:
                nimblock_fail +=1
            if grouping.fcfs.response_time > deadline:
                fcfs_fail +=1
            if grouping.base.response_time > deadline:
                base_fail += 1
            if grouping.optimal.response_time > deadline:
                opt_base_fail += 1
            if grouping.nopreempt.response_time > deadline:
                prema_fail += 1
            if grouping.rr.response_time > deadline:
                rr_fail += 1
        # print("Nimblock violation rate ", nimblock_fail/len(grouped)*100, "%")
        # print("Base violation rate ", base_fail/len(grouped)*100, "%")
        # print("FCFS violation rate ", fcfs_fail/len(grouped)*100, "%")
        # print("PREMA violation rate ", prema_fail/len(grouped)*100, "%")
        # print("Optimal Base violation rate ", opt_base_fail/len(grouped)*100, "%")
        nimblock_fails.append(nimblock_fail/len(grouped)*100)
        prema_fails.append(prema_fail/len(grouped)*100)
        opt_base_fails.append(opt_base_fail/len(grouped)*100)
        fcfs_fails.append(fcfs_fail/len(grouped)*100)
        base_fails.append(base_fail/len(grouped)*100)
        rr_fails.append(rr_fail/len(grouped)*100)
    fig, ax = plt.subplots()
    # fig, ax = plt.subplots()
    # ax.bar(x_pos, CTEs, yerr=error, align="center", alpha=1, ecolor='black', capsize=10)
    if priority_level == "high":
        for name, val in zip(["Nimblock", "PREMA", "RR", "FCFS", "BASE"], [nimblock_fails, prema_fails, rr_fails, fcfs_fails, opt_base_fails]):
            print(name, "violation rate at tight deadlines is ", val[0])
            for v, c in zip(val, cutoffs):
                if (v <= 10):
                    print("****", name, "reaches 10%% error rate at", c)
                    break

    diff = 0
    diff_d = None
    for nim, prem, d_s in zip(nimblock_fails, prema_fails, cutoffs):
        d = prem - nim
        if d > diff:
            diff = d
            diff_d = d_s
    if priority_level == "high":
        print("Greatest PREMA difference in failure rate for ", priority_level, " priority is: ", diff, " at D_s value: ", diff_d)
        print("PREMA Difference at tightest deadlines (D_s = 1.0) for", priority_level, "priority is", prema_fails[0] - nimblock_fails[0])

    diff = 0
    diff_d = None
    for nim, prem, d_s in zip(nimblock_fails, rr_fails, cutoffs):
        d = prem - nim
        if d > diff:
            diff = d
            diff_d = d_s
    if priority_level == "high":
        print("Greatest RR difference in failure rate for ", priority_level, " priority is: ", diff, " at D_s value: ", diff_d)
        print("RR Difference at tightest deadlines (D_s = 1.0) for", priority_level, "priority is", rr_fails[0] - nimblock_fails[0])
    
    for l, label in zip([nimblock_fails, prema_fails, rr_fails, fcfs_fails, opt_base_fails], ["Nimblock", "PREMA", "RR [18]", "FCFS", "Base"]):#, "Base"]):
        ax.plot(cutoffs, l, label=label)
    if (event_speed=="slow"):
        ax.plot(7.8, 10, marker = "o", markersize=6, markeredgecolor="blue", markerfacecolor="blue")
        ax.plot(3.75, 10, marker = "o", markersize=6, markeredgecolor="orange", markerfacecolor="orange")
        ax.plot(2.25, 10, marker = "o", markersize=6, markeredgecolor="green", markerfacecolor="green")
        ax.plot(2.25, 10, marker = "o", markersize=6, markeredgecolor="red", markerfacecolor="red")
        ax.axvline(x=0.94, color='gray', linestyle='dashed')
    if (event_speed=="full"):
        ax.plot(3.5, 10, marker = "o", markersize=6, markeredgecolor="blue", markerfacecolor="blue")
        ax.plot(6.0, 10, marker = "o", markersize=6, markeredgecolor="orange", markerfacecolor="orange")
        ax.axvline(x=1, color='gray', linestyle='dashed')
        #ax.plot(2.25, 10, marker = "o", markersize=4, markerfacecolor="green")
        #ax.plot(2.25, 10, marker = "o", markersize=4, markerfacecolor="red")
    if (event_speed=="realtime"):
        ax.plot(4.25, 10, marker = "o", markersize=6, markeredgecolor="blue", markerfacecolor="blue")
        ax.plot(5.75, 10, marker = "o", markersize=6, markeredgecolor="orange", markerfacecolor="orange")
        ax.plot(16.25, 10, marker = "o", markersize=6, markeredgecolor="green", markerfacecolor="green")
        ax.axvline(x=1, color='gray', linestyle='dashed')
        ax.axvline(x=1.75, color='yellow', linestyle='dashed')
        ax.axvline(x=3.5, color='pink', linestyle='dashed')
        ax.axvline(x=1.78, color='yellow', linestyle='dashed')
        ax.axvline(x=3.53, color='pink', linestyle='dashed')
        #ax.plot(2.25, 10, marker = "o", markersize=4, markerfacecolor="red")
    #ax.plot(8, 10, marker = "o", markersize=4, markerfacecolor="blue")
    ax.set_ylabel("Violation Rate (%)")
    # ax.set_xticks(x_pos)
    # ax.set_xticklabels(labels)
    ax.set_xlabel("$D_s$")
    ax.set_title("Deadline Violation Rate ("+t+" Priority)")
    # ax.yaxis.grid(True)
    # plt.tight_layout()
    ax.legend()
    plt.savefig(event_speed+"_deadline_violations_no_pipeline"+priority_level+"_lf.png")