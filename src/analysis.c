#include <stdio.h>
#include <stdlib.h>

#include "dyn_array.h"
#include "processing_scheduling.h"

#define FCFS "FCFS"
#define P "P"
#define RR "RR"
#define SJF "SJF"

// Add and comment your analysis code in this function.
// THIS IS NOT FINISHED.
int main(int argc, char **argv) 
{
    if (argc < 3) 
    {
        printf("%s <pcb file> <schedule algorithm> [quantum]\n", argv[0]);
        return EXIT_FAILURE;
    }

    //1. Load process control blocks from binary file passed at the command line into a dyn_array (this is your ready queue).
    dyn_array_t* ready_queue = load_process_control_blocks(argv[1]);
    
    //2. Execute your scheduling algorithm to collect the statistics
    //4. Report your times to STDOUT (copy these to the readme.md file)
    //IMPORTANT: Will need to change the path of the readme file to your own!
    FILE* fp;
    fp = fopen("/homes/ruffini/CIS520Project2/readme.md", "w");
    
    ScheduleResult_t scheduleResult;
    
    // Check to see what algorithm we are using, and act accordingly
    if(strcmp("FCFS", argv[2]) == 0) {
        // prints to our readme.md file!
        fprintf(fp, "First Come First Serve \n");
        
        bool result = first_come_first_serve(ready_queue, &scheduleResult);
        // if the algorithm fails for whatever reason
        if (result == false) {
            fprintf(fp, "***Algorithm failed to complete*** \n");
            fclose(fp);
            free(ready_queue);
            return EXIT_FAILURE;
        }
    }
    else if(strcmp("P", argv[2]) == 0) {
        fprintf(fp, "Priority Scheduling \n");
    }
    else if(strcmp("RR", argv[2]) == 0) {
        fprintf(fp, "Round Robin: \n");
        
        bool result = round_robin(ready_queue, &scheduleResult, (size_t)atoi(argv[3]));
        // if the algorithm fails for whatever reason
        if (result == false) {
            fprintf(fp, "***Algorithm failed to complete*** \n");
            fclose(fp);
            free(ready_queue);
            return EXIT_FAILURE;
        }
    }
    else if(strcmp("SJF", argv[2]) == 0) {
        fprintf(fp, "Shortest Job First \n");
        
        bool result = shortest_job_first(ready_queue, &scheduleResult);
        // if the algorithm fails for whatever reason
        if (result == false) {
            fprintf(fp, "***Algorithm failed to complete*** \n");
            fclose(fp);
            free(ready_queue);
            return EXIT_FAILURE;
        }
    }
    else if(strcmp("SRT", argv[2]) == 0) {
        fprintf(fp, "Shortest Remaining Time \n");
        
        bool result = shortest_remaining_time_first(ready_queue, &scheduleResult);
        // if the algorithm fails for whatever reason
        if (result == false) {
            fprintf(fp, "***Algorithm failed to complete*** \n");
            fclose(fp);
            free(ready_queue);
            return EXIT_FAILURE;
        }
    }
    else {
        printf("Not a valid algorithm type!");
        return EXIT_FAILURE;
    }
    
    // Now, just print the 
    // 1. float Average Waiting Time
    // 2. float Average Turnaround Time
    // 3. unsigned long Total Clock Time
    // to the readme!
    
    fprintf(fp, "Average Waiting Time: %.3f \n", (float)scheduleResult.average_waiting_time);
    fprintf(fp, "Average Turnaround Time: %.3f \n", (float)scheduleResult.average_turnaround_time);
    fprintf(fp, "Total Clock Time: %.3f \n", (float)scheduleResult.total_run_time);
    
    //3. Clean up any allocations
    free(ready_queue);

    return EXIT_SUCCESS;
}
