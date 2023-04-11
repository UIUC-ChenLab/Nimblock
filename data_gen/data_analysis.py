import data_graph
import data_process
import data_collect
from dataclasses import dataclass, field


def main():
    grouped_data = {}
    for event_speed in ["slow", "full", "realtime"]:
        fliers=False
        runs_FCFS = {}
        #runs_nimblock = {}
        runs_PREMA = {}
        runs_base = {}
        runs_betterbase = {}
        runs_app = {}
        runs_RR = {}
        print("Running analysis for", event_speed, "runs")
    
        # runs_FCFS[n] = data_collect.parse_runs("data_gen/output_data/FCFS/FCFS_"+event_speed+"_"+n+"_"+pipe+".txt")
        runs_FCFS[n] = data_collect.parse_runs("output_data/FCFS/FCFS_"+event_speed+".txt")
        runs_PREMA[n] = data_collect.parse_runs("output_data/PREMA/PREMA_"+event_speed+".txt")
        # runs_PREMA[n] = data_collect.parse_runs("data_gen/output_data/PREMA/Nimblock_"+event_speed+"_"+n+"_no_preempt2.txt")
        # runs_nimblock[n] = data_collect.parse_runs("data_gen/output_data/Nimblock/Nimblock_"+event_speed+"_"+n+"_app_tuned6.txt") #data_collect.parse_runs("data_gen/output_data/Nimblock_old/Nimblock_"+event_speed+"_"+n+"_tuned_basic.txt")
        #runs_base[n] = data_collect.parse_runs("output_data/Base/Base_"+event_speed+"_knn_"+batch_size+".txt")
        runs_betterbase[n] = data_collect.parse_runs("output_data/BetterBase/BetterBase_"+event_speed+".txt")
        runs_base[n] = data_collect.parse_runs("output_data/BetterBase/BetterBase_"+event_speed+"_"+".txt")
        # runs_app[n] = data_collect.parse_runs("data_gen/output_data/Nimblock/Nimblock_"+event_speed+"__"+n+"_app_tuned_sat2.txt")
        runs_app[n] = data_collect.parse_runs("output_data/Nimblock/Nimblock_"+event_speed+"_"+".txt")
        runs_RR[n] = data_collect.parse_runs("output_data/RR/RR_"+event_speed+"_"+".txt")
        
        print("Finished parse")
        
        # create graphs for grouped applications
        grouped_knn = data_collect.group_apps(runs_RR["knn"], runs_FCFS["knn"], runs_base["knn"], runs_betterbase["knn"], runs_PREMA["knn"], runs_app["knn"])
        
        print("Finished Grouping")
        
        temp = grouped_knn
        grouped_all = [j for sub in temp for j in sub]
        grouped_data[event_speed] = grouped_all
    
        #DEADLINE VIOLATIONS
        data_process.calculate_deadline_violations(grouped_all, "high", event_speed)

        #RESPONSE TIMES
        #data_graph.draw_box_plot(grouped_all, event_speed, fliers, mean=True)
        
    #TAIL LATENCY    
    #data_graph.draw_tail_latency(grouped_data, 95)
    #data_graph.draw_tail_latency(grouped_data, 99)



if __name__ == "__main__":
    main()