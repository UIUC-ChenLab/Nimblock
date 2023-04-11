from tokenize import group
import matplotlib.pyplot as plt 
import numpy as np

def draw_box_plot(grouped_all, event_speed , batch_size, fliers, folder=".", mean=False):
    # Box plot to see spread of data
    plt.rcParams.update({'font.size' : 10})
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
    print("Data for "+event_speed)
    print("Nimblock median response time is", np.median(application)*100, "%% of the baseline")
    print("Nimblock average response time is", np.mean(application)*100, "%% of the baseline")
    print("Nimblock IQR response time is", np.percentile(application, 25, interpolation = 'midpoint')*100, "and", np.percentile(application, 75, interpolation = 'midpoint')*100, "%% of the baseline")
    fcfs = np.array(fcfs)
    base = np.array(base)
    better_base = np.array(better_base)
    nopreempt = np.array(nopreempt)
    print("PREMA median response time is", np.median(nopreempt)*100, "%% of the baseline")
    print("PREMA average response time is", np.mean(nopreempt)*100, "%% of the baseline")
    print("PREMA IQR response time is", np.percentile(nopreempt, 25, interpolation = 'midpoint')*100, "and", np.percentile(nopreempt, 75, interpolation = 'midpoint')*100, "%% of the baseline")
    print("RR median response time is", np.median(rr)*100, "%% of the baseline")
    print("RR average response time is", np.mean(rr)*100, "%% of the baseline")
    print("RR IQR response time is", np.percentile(rr, 25, interpolation = 'midpoint')*100, "and", np.percentile(rr, 75, interpolation = 'midpoint')*100, "%% of the baseline")
    print("FCFS median response time is", np.median(fcfs)*100, "%% of the baseline")
    print("FCFS average response time is", np.mean(fcfs)*100, "%% of the baseline")
    print("FCFS IQR response time is", np.percentile(fcfs, 25, interpolation = 'midpoint')*100, "and", np.percentile(fcfs, 75, interpolation = 'midpoint')*100, "%% of the baseline")
    #labels = ["Nimblock", "PREMA", "RR [18]", "FCFS",  "Base"]
    labels = ["Nimblock, no preemption or pipeline",  "Nimblock, no preemption",  "Nimblock, no pipeline", "Nimblock"]
    x_pos = np.arange(len(labels))
    #data = [application, nopreempt, rr, fcfs, better_base]
    data = [application, rr, nopreempt, better_base]
    fig, ax = plt.subplots()
    # fig, ax = plt.subplots()
    # ax.bar(x_pos, CTEs, yerr=error, align="center", alpha=1, ecolor='black', capsize=10)
    vals = ax.boxplot(data, labels=labels, whis=0.0, showfliers=fliers, notch=False, showmeans=mean, showcaps=True)
    ax.set_ylabel("Relative Response Time (lower is better)")
    # ax.set_xticks(x_pos)
    ax.set_xticklabels(labels)
    ax.set_title("Relative Response Time")
    ax.yaxis.grid(True)
    plt.legend([vals['medians'][0], vals['means'][0]],['Median', 'Mean'])
    plt.tight_layout()
    plt.savefig(folder+"/"+event_speed+"_ablation_"+batch_size+".png")

def draw_box_plot_per_bmark(grouped_all, event_speed, fliers, folder=".", mean=False):
    # Box plot to see spread of data
    rr = [[],[],[],[],[],[]]
    base = [[],[],[],[],[],[]]
    better_base = [[],[],[],[],[],[]]
    fcfs = [[],[],[],[],[],[]]
    nopreempt = [[],[],[],[],[],[]]
    application = [[],[],[],[],[],[]]
    for grouping in grouped_all:
        #print(grouping)
        app = 0
        if grouping.application.name == "Lenet":
            app = 0
        elif grouping.application.name == "KNN":
            app = 1
        elif grouping.application.name == "Optical Flow":
            app = 2
        elif grouping.application.name == "Image Compression":
            app = 3
        elif grouping.application.name == "3DR":
            app = 4
        elif grouping.application.name == "Alexnet":
            app = 5
        #print(app)
        application[app].append(grouping.application.response_time/grouping.optimal.response_time)
        rr[app].append(grouping.rr.response_time/grouping.optimal.response_time)
        fcfs[app].append(grouping.fcfs.response_time/grouping.optimal.response_time)
        better_base[app].append(grouping.optimal.response_time/grouping.optimal.response_time)
        base[app].append(grouping.base.response_time/grouping.optimal.response_time)
        nopreempt[app].append(grouping.nopreempt.response_time/grouping.optimal.response_time)
    application = np.array(application,dtype=object)
    rr = np.array(rr,dtype=object)
    #print("Nimblock median response time is", np.median(application)*100, "%% of the baseline")
    #print("Nimblock average response time is", np.mean(application)*100, "%% of the baseline")
    #print("Nimblock IQR response time is", np.percentile(application, 25, interpolation = 'midpoint')*100, "and", np.percentile(application, 75, interpolation = 'midpoint')*100, "%% of the baseline")
    fcfs = np.array(fcfs,dtype=object)
    base = np.array(base,dtype=object)
    better_base = np.array(better_base,dtype=object)
    nopreempt = np.array(nopreempt,dtype=object)
    print(application)
    #print("PREMA median response time is", np.median(nopreempt)*100, "%% of the baseline")
    #print("PREMA average response time is", np.mean(nopreempt)*100, "%% of the baseline")
    #print("FCFS average response time is", np.mean(fcfs)*100, "%% of the baseline")
    #print("RR average response time is", np.mean(rr)*100, "%% of the baseline")
    #print("FCFS median response time is", np.median(fcfs)*100, "%% of the baseline")
    #print("RR median response time is", np.median(rr)*100, "%% of the baseline")
    #print("PREMA IQR response time is", np.percentile(nopreempt, 25, interpolation = 'midpoint')*100, "and", np.percentile(nopreempt, 75, interpolation = 'midpoint')*100, "%% of the baseline")
    #print("FCFS IQR response time is", np.percentile(fcfs, 25, interpolation = 'midpoint')*100, "and", np.percentile(fcfs, 75, interpolation = 'midpoint')*100, "%% of the baseline")
    #print("RR IQR response time is", np.percentile(rr, 25, interpolation = 'midpoint')*100, "and", np.percentile(rr, 75, interpolation = 'midpoint')*100, "%% of the baseline")
    labels = ["Nimblock", "PREMA", "RR [18]", "FCFS"]#,  "Base"]
    app_labels = ["LeNet","KNN","Optical Flow","Image Compression","3DR","AlexNet"]
    x_pos = np.arange(len(labels))
    data = [application, nopreempt, rr, fcfs, better_base]
    fig, ax = plt.subplots()
    # fig, ax = plt.subplots()
    # ax.bar(x_pos, CTEs, yerr=error, align="center", alpha=1, ecolor='black', capsize=10)
    def set_box_colors(bp):
        plt.setp(bp['boxes'][0],color='blue')
        plt.setp(bp['caps'][0],color='blue')
        plt.setp(bp['caps'][1],color='blue')
        plt.setp(bp['medians'][0],color='blue')
        plt.setp(bp['boxes'][1],color='red')
        plt.setp(bp['caps'][2],color='red')
        plt.setp(bp['caps'][3],color='red')
        plt.setp(bp['medians'][1],color='red')
        plt.setp(bp['boxes'][2],color='cyan')
        plt.setp(bp['caps'][4],color='cyan')
        plt.setp(bp['caps'][5],color='cyan')
        plt.setp(bp['medians'][2],color='cyan')
        plt.setp(bp['boxes'][3],color='purple')
        plt.setp(bp['caps'][6],color='purple')
        plt.setp(bp['caps'][7],color='purple')
        plt.setp(bp['medians'][3],color='purple')
        plt.setp(bp['boxes'][4],color='orange')
        plt.setp(bp['caps'][8],color='orange')
        plt.setp(bp['caps'][9],color='orange')
        plt.setp(bp['medians'][4],color='orange')
        plt.setp(bp['boxes'][5],color='magenta')
        plt.setp(bp['caps'][10],color='magenta')
        plt.setp(bp['caps'][11],color='magenta')
        plt.setp(bp['medians'][5],color='magenta')
    
    vals = ax.boxplot(application, positions = [0, 1, 2, 3, 4, 5], widths = 0.9, whis=0.0, showfliers=fliers, notch=False, showmeans=mean, showcaps=True)
    set_box_colors(vals)
    vals = ax.boxplot(nopreempt, positions = [6, 7, 8, 9, 10, 11], widths = 0.9, whis=0.0, showfliers=fliers, notch=False, showmeans=mean, showcaps=True)
    set_box_colors(vals)
    vals = ax.boxplot(rr, positions = [12, 13, 14, 15, 16, 17], widths = 0.9, whis=0.0, showfliers=fliers, notch=False, showmeans=mean, showcaps=True)
    set_box_colors(vals)
    vals = ax.boxplot(fcfs,  positions = [ 18, 19, 20, 21, 22, 23], widths = 0.9, whis=0.0, showfliers=fliers, notch=False, showmeans=mean, showcaps=True)
    set_box_colors(vals)
    
    #vals = ax.boxplot(better_base, positions = [25, 26, 27, 28, 29, 30], whis=0.0, showfliers=fliers, notch=False, showmeans=mean, showcaps=True)
    ax.set_ylabel("Relative Response Time (lower is better)")
    ax.set_xticks([2.5, 8.5, 14.5, 20.5])
    ax.set_xticklabels(labels)
    ax.set_title("Relative Response Time")
    ax.yaxis.grid(True)
    A, = ax.plot([1,1],'blue')
    B, = ax.plot([1,1],'red')
    C, = ax.plot([1,1],'cyan')
    D, = ax.plot([1,1],'purple')
    E, = ax.plot([1,1],'orange')
    F, = ax.plot([1,1],'magenta')
    ax.legend((A,B,C,D,E,F),("LeNet","KNN","Optical Flow","Image Compression","3DR","AlexNet"))
    A.set_visible(False)
    B.set_visible(False)
    C.set_visible(False)
    D.set_visible(False)
    E.set_visible(False)
    F.set_visible(False)
    plt.tight_layout()
    plt.savefig(folder+"/"+event_speed+"response_time_box.png")
def draw_box_plot_per_bmark1(grouped_all, event_speed, fliers, folder=".", mean=False):
    # Box plot to see spread of data
    lenet = [[],[],[],[],[]]
    knn = [[],[],[],[],[]]
    optflow = [[],[],[],[],[]]
    imgcmp = [[],[],[],[],[]]
    thrdr = [[],[],[],[],[]]
    alexnet = [[],[],[],[],[]]
    for grouping in grouped_all:
        #print(grouping)
        app = 0
        if grouping.application.name == "Lenet":
            app = lenet
        elif grouping.application.name == "KNN":
            app = knn
        elif grouping.application.name == "Optical Flow":
            app = optflow
        elif grouping.application.name == "Image Compression":
            app = imgcmp
        elif grouping.application.name == "3DR":
            app = thrdr
        elif grouping.application.name == "Alexnet":
            app = alexnet
        #print(app)
        app[0].append(grouping.application.response_time/grouping.optimal.response_time)
        app[2].append(grouping.rr.response_time/grouping.optimal.response_time)
        app[3].append(grouping.fcfs.response_time/grouping.optimal.response_time)
        #better_base[app].append(grouping.optimal.response_time/grouping.optimal.response_time)
        #base[app].append(grouping.base.response_time/grouping.optimal.response_time)
        app[1].append(grouping.nopreempt.response_time/grouping.optimal.response_time)
    lenet = np.array(lenet,dtype=object)
    knn = np.array(knn,dtype=object)
    #print("Nimblock median response time is", np.median(application)*100, "%% of the baseline")
    #print("Nimblock average response time is", np.mean(application)*100, "%% of the baseline")
    #print("Nimblock IQR response time is", np.percentile(application, 25, interpolation = 'midpoint')*100, "and", np.percentile(application, 75, interpolation = 'midpoint')*100, "%% of the baseline")
    optflow = np.array(optflow,dtype=object)
    imgcmp = np.array(imgcmp,dtype=object)
    thrdr = np.array(thrdr,dtype=object)
    alexnet = np.array(alexnet,dtype=object)
    #print(application)
    #print("PREMA median response time is", np.median(nopreempt)*100, "%% of the baseline")
    #print("PREMA average response time is", np.mean(nopreempt)*100, "%% of the baseline")
    #print("FCFS average response time is", np.mean(fcfs)*100, "%% of the baseline")
    #print("RR average response time is", np.mean(rr)*100, "%% of the baseline")
    #print("FCFS median response time is", np.median(fcfs)*100, "%% of the baseline")
    #print("RR median response time is", np.median(rr)*100, "%% of the baseline")
    #print("PREMA IQR response time is", np.percentile(nopreempt, 25, interpolation = 'midpoint')*100, "and", np.percentile(nopreempt, 75, interpolation = 'midpoint')*100, "%% of the baseline")
    #print("FCFS IQR response time is", np.percentile(fcfs, 25, interpolation = 'midpoint')*100, "and", np.percentile(fcfs, 75, interpolation = 'midpoint')*100, "%% of the baseline")
    #print("RR IQR response time is", np.percentile(rr, 25, interpolation = 'midpoint')*100, "and", np.percentile(rr, 75, interpolation = 'midpoint')*100, "%% of the baseline")
    labels = ["Nimblock", "PREMA", "RR [18]", "FCFS"]#,  "Base"]
    app_labels = ["LeNet","KNN","Optical Flow","Img Compr.","3DR","AlexNet"]
    x_pos = np.arange(len(labels))
    #data = [application, nopreempt, rr, fcfs, better_base]
    fig, ax = plt.subplots()
    # fig, ax = plt.subplots()
    # ax.bar(x_pos, CTEs, yerr=error, align="center", alpha=1, ecolor='black', capsize=10)
    def set_box_colors(bp):
        plt.setp(bp['boxes'][0],color='blue')
        plt.setp(bp['caps'][0],color='blue')
        plt.setp(bp['caps'][1],color='blue')
        plt.setp(bp['medians'][0],color='blue')
        plt.setp(bp['boxes'][1],color='red')
        plt.setp(bp['caps'][2],color='red')
        plt.setp(bp['caps'][3],color='red')
        plt.setp(bp['medians'][1],color='red')
        plt.setp(bp['boxes'][2],color='cyan')
        plt.setp(bp['caps'][4],color='cyan')
        plt.setp(bp['caps'][5],color='cyan')
        plt.setp(bp['medians'][2],color='cyan')
        plt.setp(bp['boxes'][3],color='purple')
        plt.setp(bp['caps'][6],color='purple')
        plt.setp(bp['caps'][7],color='purple')
        plt.setp(bp['medians'][3],color='purple')
        plt.setp(bp['boxes'][4],color='orange')
        plt.setp(bp['caps'][8],color='orange')
        plt.setp(bp['caps'][9],color='orange')
        plt.setp(bp['medians'][4],color='orange')
    
    vals = ax.boxplot(lenet, positions = [0, 1, 2, 3, 4], widths = 0.9, whis=0.0, showfliers=fliers, notch=False, showmeans=mean, showcaps=True)
    set_box_colors(vals)
    vals = ax.boxplot(knn, positions = [6, 7, 8, 9, 10], widths = 0.9, whis=0.0, showfliers=fliers, notch=False, showmeans=mean, showcaps=True)
    set_box_colors(vals)
    vals = ax.boxplot(optflow, positions = [12, 13, 14, 15, 16], widths = 0.9, whis=0.0, showfliers=fliers, notch=False, showmeans=mean, showcaps=True)
    set_box_colors(vals)
    vals = ax.boxplot(imgcmp,  positions = [ 18, 19, 20, 21, 22], widths = 0.9, whis=0.0, showfliers=fliers, notch=False, showmeans=mean, showcaps=True)
    set_box_colors(vals)
    vals = ax.boxplot(thrdr,  positions = [ 24, 25, 26, 27, 28], widths = 0.9, whis=0.0, showfliers=fliers, notch=False, showmeans=mean, showcaps=True)
    set_box_colors(vals)
    vals = ax.boxplot(alexnet,  positions = [ 30, 31, 32, 33, 34], widths = 0.9, whis=0.0, showfliers=fliers, notch=False, showmeans=mean, showcaps=True)
    set_box_colors(vals)
    
    #vals = ax.boxplot(better_base, positions = [25, 26, 27, 28, 29, 30], whis=0.0, showfliers=fliers, notch=False, showmeans=mean, showcaps=True)
    ax.set_ylabel("Relative Response Time (lower is better)")
    ax.set_xticks([2, 8, 14, 20, 26, 32])
    ax.set_xticklabels(app_labels)
    ax.set_title("Relative Response Time")
    ax.yaxis.grid(True)
    A, = ax.plot([1,1],'blue')
    B, = ax.plot([1,1],'red')
    C, = ax.plot([1,1],'cyan')
    D, = ax.plot([1,1],'purple')
    #E, = ax.plot([1,1],'orange')
    ax.legend((A,B,C,D),("Nimblock", "PREMA", "RR [18]", "FCFS",  "Base"))
    A.set_visible(False)
    B.set_visible(False)
    C.set_visible(False)
    D.set_visible(False)
    #E.set_visible(False)
    plt.tight_layout()
    plt.savefig(folder+"/"+event_speed+"response_time_box_2_no_pipeline.png")
    
def draw_violin_plot(grouped_all, event_speed, fliers, folder="."):
    # Box plot to see spread of data
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
    labels = ["Nimblock", "PREMA", "RR [18]", "FCFS", "Base"]
    x_pos = np.arange(len(labels))
    x_pos = np.array([i+1 for i in x_pos])
    data = [application, nopreempt,rr, fcfs, better_base]
    fig, ax = plt.subplots()
    # fig, ax = plt.subplots()
    # ax.bar(x_pos, CTEs, yerr=error, align="center", alpha=1, ecolor='black', capsize=10)
    ax.violinplot(data, showmedians=True, showmeans=True)
    ax.set_ylabel("Relative Response Time (lower is better)")
    ax.set_xticks(x_pos)
    ax.set_xticklabels(labels)
    ax.set_title("Relative Response Time")
    ax.yaxis.grid(True)
    # plt.tight_layout()
    plt.savefig(folder+"/"+event_speed+"response_time_violin.png")

def draw_priority_violin_plot(grouped_all, event_speed, fliers, folder="."):  
    grouped_low = [j for j in grouped_all if j.application.priority==1]
    grouped_med = [j for j in grouped_all if j.application.priority==3]
    grouped_high = [j for j in grouped_all if j.application.priority==9]
    # Box plots to see spread for each priority
    fig, axes = plt.subplots(1,3, figsize=(20, 5))
    for priority, dat, ax in zip(["Low_Priority", "Medium_Priority", "High_Priority"], [grouped_low, grouped_med, grouped_high], axes.flat):
        rr = []
        base = []
        better_base = []
        fcfs = []
        nopreempt = []
        application = []
        for grouping in dat:
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
        labels = ["Nimblock", "PREMA", "RR [18]", "FCFS", "Advanced Base"]
        x_pos = np.arange(len(labels))
        x_pos = np.array([i+1 for i in x_pos])
        data = [application, nopreempt, rr, fcfs, better_base]
        # ax.bar(x_pos, CTEs, yerr=error, align="center", alpha=1, ecolor='black', capsize=10)
        ax.violinplot(data, showmedians=True, showmeans=True)
        ax.set_ylabel("Relative Response Time (lower is better)")
        ax.set_xticks(x_pos)
        ax.set_xticklabels(labels)
        ax.set_title("Relative Response Time for " +priority)
        ax.yaxis.grid(True)
        # plt.tight_layout()
    fig.savefig(folder+"/"+event_speed + "respone_time_violin_priorities.png")

def draw_priority_box_plot(grouped_all, event_speed, fliers, folder=".", mean=False):  
    grouped_low = [j for j in grouped_all if j.application.priority==1]
    grouped_med = [j for j in grouped_all if j.application.priority==3]
    grouped_high = [j for j in grouped_all if j.application.priority==9]
    # Box plots to see spread for each priority
    fig, axes = plt.subplots(1,3, figsize=(20, 5))
    for priority, dat, ax in zip(["Low_Priority", "Medium_Priority", "High_Priority"], [grouped_low, grouped_med, grouped_high], axes.flat):
        rr = []
        base = []
        better_base = []
        fcfs = []
        nopreempt = []
        application = []
        for grouping in dat:
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
        labels = ["Nimblock", "PREMA", "RR [18]", "FCFS", "Base"]
        x_pos = np.arange(len(labels))
        data = [application, nopreempt, rr, fcfs, better_base]
        # ax.bar(x_pos, CTEs, yerr=error, align="center", alpha=1, ecolor='black', capsize=10)
        ax.boxplot(data, labels=labels, showfliers=fliers, notch=False, showmeans=mean)
        ax.set_ylabel("Relative Response Time (lower is better)")
        # ax.set_xticks(x_pos)
        ax.set_xticklabels(labels)
        ax.set_title("Relative Response Time for " +priority)
        ax.yaxis.grid(True)
        # plt.tight_layout()
    fig.savefig(folder+"/"+event_speed + "respone_time_box_priorities.png")

def response_time_bar(grouped_all, event_speed, batch_size, folder="."):
    # Second graph compares FCFS vs Nimblock for response time (no baseline yet, need to benchmark) No optimal yet, need to benchmark
    nimblock = []
    base = []
    better_base = []
    fcfs = []
    nopreempt = []
    for grouping in grouped_all:
        nimblock.append(grouping.application.response_time/grouping.optimal.response_time)
        fcfs.append(grouping.fcfs.response_time/grouping.optimal.response_time)
        better_base.append(grouping.optimal.response_time/grouping.optimal.response_time)
        base.append(grouping.base.response_time/grouping.optimal.response_time)
        nopreempt.append(grouping.nopreempt.response_time/grouping.optimal.response_time)
    nimblock = np.array(nimblock)
    fcfs = np.array(fcfs)
    base = np.array(base)
    better_base = np.array(better_base)
    nimblock_mean = np.mean(nimblock)
    nopreemptmean = np.mean(nopreempt)
    fcfs_mean = np.mean(fcfs)
    base_mean = np.mean(base)
    better_base_mean = np.mean(better_base)
    nimblock_std = np.std(nimblock)
    fcfs_std = np.std(fcfs)
    base_std = np.std(base)
    better_base_std = np.std(better_base)
    nopreempt_std = np.std(nopreempt)
    labels = ["Nimblock", "PREMA", "FCFS", "Base", "Base"]
    x_pos = np.arange(len(labels))
    CTEs = [nimblock_mean, nopreemptmean, fcfs_mean, better_base_mean, base_mean]
    error = [nimblock_std, nopreempt_std, fcfs_std, better_base_std, base_std]
    fig, ax = plt.subplots()
    # ax.bar(x_pos, CTEs, yerr=error, align="center", alpha=1, ecolor='black', capsize=10)
    ax.bar(x_pos, CTEs, align="center", alpha=1, ecolor='black', capsize=10)
    ax.set_ylabel("Relative Latency (lower is better)")
    ax.set_xticks(x_pos)
    ax.set_xticklabels(labels)
    ax.set_title("Average Response time")
    ax.yaxis.grid(True)
    plt.tight_layout()
    plt.savefig(folder+"/"+event_speed+"_"+batch_size+"_"+"average_response_time.png")

def response_time_bar_priority(grouped_all, event_speed, folder="."):
    grouped_low = [j for j in grouped_all if j.nimblock.priority==1]
    grouped_med = [j for j in grouped_all if j.nimblock.priority==3]
    grouped_high = [j for j in grouped_all if j.nimblock.priority==9]
    # Second set of graphs same deal but separate for priority levels
    fig, axes = plt.subplots(1,3)
    for priority, dat, ax in zip(["Low_Priority", "Medium_Priority", "High_Priority"], [grouped_low, grouped_med, grouped_high], axes):
        nimblock = []
        base = []
        better_base = []
        fcfs = []
        nopreempt = []
        for grouping in dat:
            nimblock.append(grouping.nimblock.response_time/grouping.optimal.response_time)
            fcfs.append(grouping.fcfs.response_time/grouping.optimal.response_time)
            better_base.append(grouping.optimal.response_time/grouping.optimal.response_time)
            base.append(grouping.base.response_time/grouping.optimal.response_time)
            nopreempt.append(grouping.nopreempt.response_time/grouping.optimal.response_time)
        nimblock = np.array(nimblock)
        fcfs = np.array(fcfs)
        base = np.array(base)
        better_base = np.array(better_base)
        nimblock_mean = np.mean(nimblock)
        nopreemptmean = np.mean(nopreempt)
        fcfs_mean = np.mean(fcfs)
        base_mean = np.mean(base)
        better_base_mean = np.mean(better_base)
        nimblock_std = np.std(nimblock)
        fcfs_std = np.std(fcfs)
        base_std = np.std(base)
        better_base_std = np.std(better_base)
        nopreempt_std = np.std(nopreempt)
        labels = ["Nimblock", "PREMA", "FCFS", "Base", "Base"]
        x_pos = np.arange(len(labels))
        CTEs = [nimblock_mean, nopreemptmean, fcfs_mean, better_base_mean, base_mean]
        error = [nimblock_std, nopreempt_std, fcfs_std, better_base_std, base_std]

        # ax.bar(x_pos, CTEs, yerr=error, align="center", alpha=1, ecolor='black', capsize=10)
        ax.bar(x_pos, CTEs, align="center", alpha=1, ecolor='black', capsize=10)
        ax.set_ylabel("Relative Response Time (lower is better)")
        ax.set_xticks(x_pos)
        ax.set_xticklabels(labels)
        ax.set_title("Average Response Time for " +priority)
        ax.yaxis.grid(True)
    plt.tight_layout()
    plt.savefig(folder + "/" + event_speed + "average_respone_time_priorities.png")

def draw_makespan(makespans_all, event_speed, folder="."):
    # Set of graphs for makespan
    nimblock = []
    base = []
    better_base = []
    fcfs = []
    nopreempt = []
    application = []
    for grouping in makespans_all:
        nimblock.append(grouping.application/grouping.optimal)
        fcfs.append(grouping.fcfs/grouping.optimal)
        better_base.append(grouping.optimal/grouping.optimal)
        base.append(grouping.base/grouping.optimal)
        nopreempt.append(grouping.nopreempt/grouping.optimal)
        application.append(grouping.application/grouping.optimal)
    application = np.array(application)
    nimblock = np.array(nimblock)
    fcfs = np.array(fcfs)
    base = np.array(base)
    better_base = np.array(better_base)
    nimblock_mean = np.mean(nimblock)
    application_mean = np.mean(application)
    nopreemptmean = np.mean(nopreempt)
    fcfs_mean = np.mean(fcfs)
    base_mean = np.mean(base)
    better_base_mean = np.mean(better_base)
    nimblock_std = np.std(nimblock)
    application_std = np.mean(application)
    fcfs_std = np.std(fcfs)
    base_std = np.std(base)
    better_base_std = np.std(better_base)
    nopreempt_std = np.std(nopreempt)
    labels = ["Nimblock", "PREMA", "FCFS", "Base", "Base"]
    x_pos = np.arange(len(labels))
    CTEs = [application_mean, nopreemptmean, fcfs_mean, better_base_mean, base_mean]
    error = [application_std, nopreempt_std, fcfs_std, better_base_std, base_std]
    fig, ax = plt.subplots()
    # ax.bar(x_pos, CTEs, yerr=error, align="center", alpha=1, ecolor='black', capsize=10)
    ax.bar(x_pos, CTEs, align="center", alpha=1, ecolor='black', capsize=10)
    ax.set_ylabel("Relative Makespan (lower is better)")
    ax.set_xticks(x_pos)
    ax.set_xticklabels(labels)
    ax.set_title("Average Relative Makespan time")
    ax.yaxis.grid(True)
    plt.tight_layout()
    plt.savefig(folder+ "/" + event_speed + "makespan_tuned_basic.png")

def latency_degradation(grouped_all, event_speed, fliers, folder=".", mean=False):
    # Box plot to see spread of data
    nimblock = []
    base = []
    better_base = []
    fcfs = []
    nopreempt = []
    application = []
    for grouping in grouped_all:
        application.append(grouping.application.response_time/grouping.latency)
        nimblock.append(grouping.nimblock.response_time/grouping.latency)
        fcfs.append(grouping.fcfs.response_time/grouping.latency)
        better_base.append(grouping.optimal.response_time/grouping.latency)
        base.append(grouping.base.response_time/grouping.latency)
        nopreempt.append(grouping.nopreempt.response_time/grouping.latency)
    application = np.array(application)
    nimblock = np.array(nimblock)
    fcfs = np.array(fcfs)
    base = np.array(base)
    better_base = np.array(better_base)
    nopreempt = np.array(nopreempt)
    labels = ["Nimblock", "PREMA"]#, "FCFS", "Opt Base"]
    x_pos = np.arange(len(labels))
    data = [application, nopreempt]#, fcfs, better_base]
    fig, ax = plt.subplots()
    # fig, ax = plt.subplots()
    # ax.bar(x_pos, CTEs, yerr=error, align="center", alpha=1, ecolor='black', capsize=10)
    ax.boxplot(data, labels=labels, showfliers=fliers, notch=False, showmeans=mean)
    ax.set_ylabel("Slowdown")
    # ax.set_xticks(x_pos)
    ax.set_xticklabels(labels)
    ax.set_title("Slowdown")
    ax.yaxis.grid(True)
    # plt.tight_layout()
    plt.savefig(folder+"/"+event_speed+"slowdown_box.png")

def latency_degradation_priority(grouped_all, event_speed, fliers, folder=".", mean=False):  
    grouped_low = [j for j in grouped_all if j.nimblock.priority==1]
    grouped_med = [j for j in grouped_all if j.nimblock.priority==3]
    grouped_high = [j for j in grouped_all if j.nimblock.priority==9]
    # Box plots to see spread for each priority
    fig, axes = plt.subplots(1,3, figsize=(20, 5))
    for priority, dat, ax in zip(["Low_Priority", "Medium_Priority", "High_Priority"], [grouped_low, grouped_med, grouped_high], axes.flat):
        nimblock = []
        base = []
        better_base = []
        fcfs = []
        nopreempt = []
        application = []
        for grouping in dat:
            application.append(grouping.application.response_time/grouping.latency)
            nimblock.append(grouping.nimblock.response_time/grouping.latency)
            fcfs.append(grouping.fcfs.response_time/grouping.latency)
            better_base.append(grouping.optimal.response_time/grouping.latency)
            base.append(grouping.base.response_time/grouping.latency)
            nopreempt.append(grouping.nopreempt.response_time/grouping.latency)
        application = np.array(application)
        nimblock = np.array(nimblock)
        fcfs = np.array(fcfs)
        base = np.array(base)
        better_base = np.array(better_base)
        nopreempt = np.array(nopreempt)
        labels = ["Nimblock", "PREMA"]#, "FCFS"]#, "Opt Base"]
        x_pos = np.arange(len(labels))
        data = [application, nopreempt]#, fcfs]#, better_base]
        # ax.bar(x_pos, CTEs, yerr=error, align="center", alpha=1, ecolor='black', capsize=10)
        ax.boxplot(data, labels=labels, showfliers=fliers, notch=False, showmeans=mean)
        ax.set_ylabel("Slowdown")
        # ax.set_xticks(x_pos)
        ax.set_xticklabels(labels)
        ax.set_title("Slowdown for " +priority)
        ax.yaxis.grid(True)
        # plt.tight_layout()
    fig.savefig(folder+"/"+event_speed + "slowdown_box_priorities.png")

def draw_tail_latency(grouped_all, perce=99, folder="."):
    # Second graph compares FCFS vs Nimblock for response time (no baseline yet, need to benchmark) No optimal yet, need to benchmark
    fig, ax = plt.subplots()
    plt.rcParams.update({'font.size' : 10})
    labels = ["95th %ile", "\n\nStandard Test", "99th %ile", "95th %ile", "\n\nStress Test", "99th %ile", "95th %ile", "\n\nReal-time Test", "99th %ile"]
    bar_colors = ["Nimblock", "PREMA", "RR [18]", "FCFS"]#,  "Base"]
    data_nimblock  = []
    data_prema = []
    data_rr = []
    data_fcfs = []
    
    for speed, group in grouped_all.items():
        for p in [95, 99]:
            rr = []
            base = []
            better_base = []
            fcfs = []
            nopreempt = []
            application = []
            for grouping in group:
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
            data_nimblock.append(np.percentile(application, p, interpolation="linear"))
            data_prema.append(np.percentile(nopreempt, p, interpolation="linear"))
            data_rr.append(np.percentile(rr, p, interpolation="linear"))
            data_fcfs.append(np.percentile(fcfs, p, interpolation="linear"))
            # np.percentile(better_base, 99, interpolation="midpoint")
            # labels.append(speed)
    print(data_nimblock)        
    print(data_prema)               
    print(data_rr)        
    print(data_fcfs)        
              
    x_pos = np.array([0, 1, 2, 3, 4, 5]) #np.arange(len(labels))
    labels_pos = [0, 0.5, 1, 2, 2.5, 3, 4, 4.5, 5]
    width = 0.2
    bars = ax.bar(x_pos-3*width/2, data_nimblock, width, label="Nimblock")
    # ax.bar_label(bars)
    bars = ax.bar(x_pos-width/2, data_prema, width, label="PREMA")
    # ax.bar_label(bars)
    bars = ax.bar(x_pos+width/2, data_rr, width, label="RR [18]")
    # ax.bar_label(bars)
    bars = ax.bar(x_pos+3*width/2, data_fcfs, width, label="FCFS")
    # ax.bar_label(bars)
    
    ax.set_xticks(labels_pos)
    ax.set_xticklabels(labels)
    ax.legend()
    ax.set_ylabel("Relative Response Time")
    # ax.set_xticks(x_pos, speeds)
    ax.tick_params(axis='x', which='both', length=0)
    ax.set_title("Tail Response Times")
    # plt.tight_layout()
    plt.savefig(folder+"/no_pipe_line_tail_latency.png")