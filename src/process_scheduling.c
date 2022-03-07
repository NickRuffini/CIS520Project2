#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "dyn_array.h"
#include "processing_scheduling.h"


// You might find this handy.  I put it around unused parameters, but you should
// remove it before you submit. Just allows things to compile initially.
#define UNUSED(x) (void)(x)

// private function
void virtual_cpu(ProcessControlBlock_t *process_control_block) 
{
    // decrement the burst time of the pcb
    --process_control_block->remaining_burst_time;
}

bool first_come_first_serve(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
    UNUSED(ready_queue);
    UNUSED(result);
    return false;
}

bool shortest_job_first(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
    UNUSED(ready_queue);
    UNUSED(result);
    return false;   
}

bool priority(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
    UNUSED(ready_queue);
    UNUSED(result);
    return false;   
}

bool round_robin(dyn_array_t *ready_queue, ScheduleResult_t *result, size_t quantum) 
{
    UNUSED(ready_queue);
    UNUSED(result);
    UNUSED(quantum);
    return false;
}

// Reads the PCB burst time values from the binary file into ProcessControlBlock_t remaining_burst_time field
// for N number of PCB burst time stored in the file.
// \param input_file the file containing the PCB burst times
// \return a populated dyn_array of ProcessControlBlocks if function ran successful else NULL for an error
dyn_array_t *load_process_control_blocks(const char *input_file) 
{
    //UNUSED(input_file);
    
    // First need to open the PCB file so that we can read from it!
    // REFERENCE: https://www.tutorialspoint.com/c_standard_library/c_function_fopen.htm
    FILE * fp;
    // Open file in read only mode ("r")
    fp = fopen(input_file, "r");
    
    // If we failed to open the PCB file, return NULL
    if(fp == NULL) {
        return NULL;
    }
    
    // Now, we can read the number of elements in a file into an array using FREAD
    // Need this so we know how big to make the array later
    // (Also, this number is the first element we get from the input_file!)
    // REFERENCE: https://www.tutorialspoint.com/c_standard_library/c_function_fread.htm
    uint32_t numItems;
    fread(&numItems, sizeof(uint32_t), 1, fp);
    
    // Need to allocate an array of the correct size now! (array of PCB items)
    ProcessControlBlock_t* pcbArray = malloc(sizeof(ProcessControlBlock_t) * numItems);
    
    // Now that we have an array of the correct size, need to populate it with remaining values
    // For each iteration of i, we are reading a PCB's burst time, priority and arrival time
    // typedef struct 
    //{
    //    uint32_t remaining_burst_time; 
    //    uint32_t priority;			
    //    uint32_t arrival;				
    //    bool started;              	    
    //} 
    //ProcessControlBlock_t;		    
    for(uint32_t i = 0; i < numItems; i++) {
        fread(&(pcbArray[i]).remaining_burst_time, sizeof(uint32_t), 1, fp);
        fread(&(pcbArray[i]).priority, sizeof(uint32_t), 1, fp);
        fread(&(pcbArray[i]).arrival, sizeof(uint32_t), 1, fp);
        
        // Also need to set the started value of a PCB, even though we aren't reading it
        pcbArray[i].started = 0;
    }
    
    // Now that we have a normal array of PCB items, we need to import it into a dynamic array
    // dyn_array_import from /src/dyn_array.c
    dyn_array_t* pcbDynamicArray = dyn_array_import(pcbArray, numItems, sizeof(ProcessControlBlock_t), NULL);
    
    // If dyn_array_import returns NULL, it failed so return NULL
    if(pcbDynamicArray == NULL) {
        return NULL;
    }
    
    return pcbDynamicArray;
}

bool shortest_remaining_time_first(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
    UNUSED(ready_queue);
    UNUSED(result);
    return false;
}
