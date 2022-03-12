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
    // param error checks
    if(!ready_queue || !result){
        return false;
    }

    uint32_t s = dyn_array_size(ready_queue);
    uint32_t runTime = 0;
    uint32_t turnaroundTime = 0;
    uint32_t waitingTime = 0;

    ProcessControlBlock_t pcb;

    for(uint32_t i = 0; i < s; i++){
        waitingTime += runTime;
        dyn_array_extract_back(ready_queue, (void *)&pcb);
        runTime += pcb.remaining_burst_time;
        turnaroundTime += runTime;
        turnaroundTime -= pcb.arrival; 
        waitingTime -= pcb.arrival;

        while(pcb.remaining_burst_time > 0){
            virtual_cpu(&pcb);
        }
    }

    // calculations
    result->average_waiting_time = ((float)waitingTime) / ((float)s);
    result->average_turnaround_time = ((float)turnaroundTime) / ((float)s);
    result->total_run_time = runTime;
     
    return true;
}

bool shortest_job_first(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
    // Ensures that parameters are valid
    if (ready_queue != NULL && result != NULL)
    {
        // Receives length from dyn_array_size and then verifies whether the ready_queue has a valid length/if an error occurred in the dyn_array_size funciton
        int length = dyn_array_size(ready_queue);
        if (length > 0)
        {
            // Iterates through ready_queue finding the PCB with the earliest arrival time
            int firstPCBIndex = -1;
            for (int counter = 0; counter < length; counter++)
            {
                // On the first iteration of the loop, the default value of -1 is overridden by the first PCB
                ProcessControlBlock_t* pcb = dyn_array_at(ready_queue, counter);
                if (firstPCBIndex == -1)
                {
                    firstPCBIndex = counter;
                }
                else 
                {
                    // Determines whether the current PCB being checked has an earlier arrival time than the stored index's PCB arrival time, 
                    // when it is earlier than this value the firstPCBIndex is reassigned to the new earliest PCB's index.
                    ProcessControlBlock_t* currentFirstPCB = dyn_array_at(ready_queue, firstPCBIndex);
                    if (pcb->arrival < currentFirstPCB->arrival)
                    {
                        firstPCBIndex = counter;
                    }
                    // In the case of a tie, the firstPCBIndex is assigned to the PCB with the lower remaining_burst_time as this is shortest_job_first.
                    // (In the case of a further tie, where both have an equal remaining_burst_time, the index of the PCB that was earlier in ready_queue remains the firstPCBIndex).
                    else if (pcb->arrival == currentFirstPCB->arrival)
                    {
                        if (pcb->remaining_burst_time < currentFirstPCB->remaining_burst_time)
                        {
                            firstPCBIndex = counter;
                        }
                    }
                }
            }
            // Now that we have our first PCB, we can load it onto the virtual CPU.
            // This requires a bit of initialization at first - we need to ensure the variables for our statistics are accurate.
            ProcessControlBlock_t* currentPCB = dyn_array_at(ready_queue, firstPCBIndex);
            int currentPCBRemainingTime = currentPCB->remaining_burst_time;
            unsigned int totalRunTime = currentPCB->arrival;
            int totalWaitingTime = 0;
            int totalTurnaroundTime = 0;
            int timeWithoutChange = 0;
            currentPCB->started = true;
            while (currentPCBRemainingTime > 0)
            {
                // Incrementing totalRunTime to ensure that our statistics are accurate.
                totalRunTime++;
                timeWithoutChange++;
                virtual_cpu(currentPCB);
                // Now, if our process has completed, we find a new PCB to take its place.
                // This process is similar to the process that we used to select our first PCB.
                if (currentPCB->remaining_burst_time == 0)
                {       
                    int shortestAvailablePCBIndex = -1;          
                    for (int counter = 0; counter < length; counter++)
                    {
                        // We iterate through each PCB, checking first if they are not our currentPCB, then checking whether they are already completed.
                        // We then determine whether they have arrived yet.
                        ProcessControlBlock_t* PCBToCheck = dyn_array_at(ready_queue, counter);
                        if (!PCBToCheck->started && PCBToCheck->remaining_burst_time > 0 && 
                            PCBToCheck->arrival <= totalRunTime)
                        {
                            // If they meet all of these conditions, then they are a contender for the PCB that we will swap to next.
                            // If we have not set another possible contender (shortestAvailablePCBIndex is equal to -1), this is stored as the best contender for this iteration.
                            if (shortestAvailablePCBIndex == -1)
                            {
                                shortestAvailablePCBIndex = counter;
                            }
                            else
                            {
                                // If, however, there is another PCB we've found that meets all of our above requirements, then we will 
                                // have to compare our previously found PCB with this new contender.
                                ProcessControlBlock_t* fastestPCB = dyn_array_at(ready_queue, shortestAvailablePCBIndex);
                                // If it is faster than the previous PCB we found, its index will be assigned to shortestAvailablePCBIndex.
                                if (PCBToCheck->remaining_burst_time < fastestPCB->remaining_burst_time)
                                {
                                    shortestAvailablePCBIndex = counter;
                                }
                                // In the case of a tie in their remaining_burst_times, the chosen PCB will be the one who has the earliest arrival time, so as to limit turnaround time.
                                // (In the case of a further tie, the first PCB of the two in ready_queue will be chosen)
                                else if (PCBToCheck->remaining_burst_time == fastestPCB->remaining_burst_time)
                                {
                                    if (PCBToCheck->arrival < fastestPCB->arrival)
                                    {
                                        shortestAvailablePCBIndex = counter;
                                    }
                                }
                            }
                        }
                    }
                    // If the PCB is completed, we can switch the started value to false as it is now complete, and we can update the totalTurnaroundTime and totalWaitingTime.
                    currentPCB->started = false;
                    // As turnaround time is always how long a process takes to be completed once it arrives, this should update totalTurnaroundTime correctly.
                    totalTurnaroundTime += (totalRunTime - currentPCB->arrival);
                    // totalWaitingTime can be updated as shown below, as the formula for the waiting time for a specific PCB can be defined as 
                    // The end time of a program - the total time that it was worked on no matter how many attempts it took - its arrival time.
                    totalWaitingTime += (totalRunTime - timeWithoutChange - currentPCB->arrival);
                    // Now we determine whether a replacement was found by checking shortestAvailablePCBIndex's value.
                    // If it is not its default value of -1, a replacement was found.
                    if (shortestAvailablePCBIndex != -1)
                    {
                        // We reset timeWithoutChange as we have now changed the PCB.
                        timeWithoutChange = 0;
                        // We then assign the currentPCB to the new PCB and set its start value to true.
                        currentPCB = dyn_array_at(ready_queue, shortestAvailablePCBIndex);
                        currentPCB->started = true;
                    }
                }
                // Lastly, we set currentPCBRemainingTime to its new value, whether our PCB was changed or not.
                currentPCBRemainingTime = currentPCB->remaining_burst_time; 
            }
            // Calculating average waiting time and turnaround time and assigning these to result, our Schedule_Result_t object.
            result->average_waiting_time = ((float)totalWaitingTime) / ((float)length);
            result->average_turnaround_time = ((float)totalTurnaroundTime) / ((float)length);
            // Also assigning totalRunTime as is, should require no difference.
            result->total_run_time = totalRunTime;
            // Algorithm executed as expected.
            return true;
        }
    }
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
    if (ready_queue != NULL && result != NULL && quantum > 0)
    {
        int length = dyn_array_size(ready_queue);
        if (length > 0)
        {
            // Iterates through ready_queue finding the PCB with the earliest arrival time
            int firstPCBIndex = -1;
            for (int counter = 0; counter < length; counter++)
            {
                // On the first iteration of the loop, the default value of -1 is overridden by the first PCB
                ProcessControlBlock_t* pcb = dyn_array_at(ready_queue, counter);
                if (firstPCBIndex == -1)
                {
                    firstPCBIndex = counter;
                }
                else 
                {
                    // Determines whether the current PCB being checked has an earlier arrival time than the stored index's PCB arrival time, 
                    // when it is earlier than this value the firstPCBIndex is reassigned to the new earliest PCB's index.
                    ProcessControlBlock_t* currentFirstPCB = dyn_array_at(ready_queue, firstPCBIndex);
                    if (pcb->arrival < currentFirstPCB->arrival)
                    {
                        firstPCBIndex = counter;
                    }
                    // In the case of a tie, the firstPCBIndex is assigned to the PCB that first appeared in the ready_queue.
                }
            }
            // Now that we have our first PCB, we can load it onto the virtual CPU.
            // This requires a bit of initialization at first - we need to ensure the variables for our statistics are accurate.
            // We also need to create a dyn_array that will act as a queue for our roundrobin queue.
            ProcessControlBlock_t* currentPCB = dyn_array_at(ready_queue, firstPCBIndex);
            int currentPCBRemainingTime = currentPCB->remaining_burst_time;
            unsigned int totalRunTime = currentPCB->arrival;
            int totalWaitingTime = 0;
            int totalTurnaroundTime = 0;
            int timeWithoutChange = 0;
            currentPCB->started = true;
            dyn_array_t* PCBQueue = dyn_array_create(length, sizeof(ProcessControlBlock_t), NULL);
            dyn_array_push_front(PCBQueue, currentPCB);
            // Lastly, we need to create our quantum variable.
            int currentQuantum = quantum;
            // We also need to do a last minute check in case there are PCBs that have the same arrival time as our currentPCB.
            for (int counter = 0; counter < length; counter++)
            {
                ProcessControlBlock_t* newPCB = dyn_array_at(ready_queue, counter);
                // We check if any PCBs are incomplete and arrived.
                // We also check to ensure it's not the PCB we already have.
                if (newPCB->remaining_burst_time > 0 && newPCB->arrival == totalRunTime && !newPCB->started)
                {
                    dyn_array_push_back(PCBQueue, newPCB);
                }
            }
            // Now we can start the schedule proper.
            while (currentPCBRemainingTime > 0)
            {
                // Incrementing totalRunTime to ensure that our statistics are accurate.
                totalRunTime++;
                timeWithoutChange++;
                currentQuantum--;
                virtual_cpu(currentPCB);
                
                // We now check if any new processes have arrived and need to be added to the queue.
                // This is similar to the earlier process.
                for (int counter = 0; counter < length; counter++)
                {
                    ProcessControlBlock_t* newPCB = dyn_array_at(ready_queue, counter);
                    // We check if any PCBs are incomplete and arrived.
                    if (newPCB->remaining_burst_time > 0 && newPCB->arrival == totalRunTime)
                    {
                        dyn_array_push_back(PCBQueue, newPCB);
                    }
                }
                // We then check if the process has finished, in which case we dequeue it and start the next process.
                if (currentPCB->remaining_burst_time == 0)
                {
                    ProcessControlBlock_t* oldPCB = malloc(sizeof(ProcessControlBlock_t*));
                    dyn_array_extract_front(PCBQueue, oldPCB);
                    // As turnaround time is always how long a process takes to be completed once it arrives, this should update totalTurnaroundTime correctly.
                    totalTurnaroundTime += (totalRunTime - oldPCB->arrival);
                    // totalWaitingTime can be updated as shown below, as the formula for the waiting time for a specific PCB can be defined as 
                    // The end time of a program - the total time that it was worked on no matter how many attempts it took - its arrival time.
                    totalWaitingTime += (totalRunTime - timeWithoutChange - oldPCB->arrival);
                    if (dyn_array_size(PCBQueue) > 0)
                    {
                        currentPCB = dyn_array_front(PCBQueue);
                        currentPCB->started = true;
                    }
                    // We also have to refresh the quantum and the timeWithoutChange fields.
                    currentQuantum = quantum;
                    timeWithoutChange = 0;
                }
                // Now we determine if the quantum has expired, in which case we pop the first process off the front and move it to the back.
                if (currentQuantum == 0)
                {
                    ProcessControlBlock_t* oldPCB = malloc(sizeof(ProcessControlBlock_t*));
                    dyn_array_extract_front(PCBQueue, oldPCB);
                    // Ensures the remaining_burst_time and started values are correct.
                    oldPCB->remaining_burst_time = currentPCB->remaining_burst_time;
                    oldPCB->started = true;
                    dyn_array_push_back(PCBQueue, oldPCB);
                    // We then start the process at the front of the queue and refresh our quantum.
                    currentPCB = dyn_array_front(PCBQueue);
                    if (!currentPCB->started) currentPCB->started = true;
                    currentQuantum = quantum;
                    // We then update our totalWaitingTime to subtract how long the process ran, which should always be the quantum.
                    totalWaitingTime -= timeWithoutChange;
                    timeWithoutChange = 0;
                }
                // Lastly, we set currentPCBRemainingTime to its new value, whether our PCB was changed or not.
                currentPCBRemainingTime = currentPCB->remaining_burst_time; 
            }
            // Calculating average waiting time and turnaround time and assigning these to result, our Schedule_Result_t object.
            result->average_waiting_time = ((float)totalWaitingTime) / ((float)length);
            result->average_turnaround_time = ((float)totalTurnaroundTime) / ((float)length);
            // Also assigning totalRunTime as is, should require no difference.
            result->total_run_time = totalRunTime;
            // The algorithm should have executed correctly if it gets to this point.
            return true;
        }
    }
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
