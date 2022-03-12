#include <fcntl.h>
#include <stdio.h>
#include "gtest/gtest.h"
#include <pthread.h>
#include "../include/processing_scheduling.h"

// Using a C library requires extern "C" to prevent function managling
extern "C" 
{
#include <dyn_array.h>
}


#define NUM_PCB 30
#define QUANTUM 5 // Used for Robin Round for process as the run time limit

unsigned int score;
unsigned int total;

/*
*
* TEST CASES
* bool first_come_first_serve(dyn_array_t *ready_queue, ScheduleResult_t *result)
*
*/

TEST (first_come_first_serve, NullReadyQueue){
    ScheduleResult_t *sr = new ScheduleResult_t;
    dyn_array_t *array = NULL;
    bool result = first_come_first_serve(array, sr);
    ASSERT_EQ(false, result);
    delete sr;

    score += 20;
}

TEST (first_come_first_serve, NullScheduleResult){
    ScheduleResult_t *sr = NULL;
    dyn_array_t *array = dyn_array_create(0, sizeof(ProcessControlBlock_t), NULL);
    bool result = first_come_first_serve(array, sr);
    ASSERT_EQ(false, result);
    dyn_array_destroy(array);
}

TEST (first_come_first_serve, validInput) 
{
    ScheduleResult_t *sr = new ScheduleResult_t;
    dyn_array_t* dynArray = dyn_array_create(0,sizeof(ProcessControlBlock_t),NULL);
    memset(sr,0,sizeof(ScheduleResult_t));
    // add PCBs now
    ProcessControlBlock_t data[3] = 
    {
        [0] = {24,2,0,0},
        [1] = {3,3,0,0},
        [2] = {3,1,0,0}
    };
    // back loading dyn_array, pull from the back
    dyn_array_push_back(dynArray,&data[2]);
    dyn_array_push_back(dynArray,&data[1]);
    dyn_array_push_back(dynArray,&data[0]);	

    bool res = first_come_first_serve (dynArray,sr);	
    ASSERT_EQ(true,res);

    float answers[3] = {27,17,30};
    ASSERT_EQ(answers[0],sr->average_turnaround_time);
    ASSERT_EQ(answers[1],sr->average_waiting_time);
    ASSERT_EQ(answers[2],sr->total_run_time);
    dyn_array_destroy(dynArray);
    
    delete sr;

    score+=20;
}

/*
*
* TEST CASES
* bool shortest_job_first(dyn_array_t *ready_queue, ScheduleResult_t *result)
*
*/

TEST (shortest_job_first, NullReadyQueue)
{
    ScheduleResult_t* sr = new ScheduleResult_t;
    dyn_array_t* readyQueue = NULL;
    bool result = shortest_job_first(readyQueue, sr);
    ASSERT_EQ(false, result);
    delete sr;
}

TEST (shortest_job_first, GoodSchedule)
{
    ScheduleResult_t* sr = new ScheduleResult_t;
    dyn_array_t* readyQueue = dyn_array_create((sizeof(ProcessControlBlock_t)*3), sizeof(ProcessControlBlock_t), NULL);
    ProcessControlBlock_t* process1 = (ProcessControlBlock_t*)malloc(sizeof(ProcessControlBlock_t));
    process1->arrival = 1;
    process1->remaining_burst_time = 10;
    process1->started = false;
    dyn_array_push_back(readyQueue, process1);
    ProcessControlBlock_t* process2 = (ProcessControlBlock_t*)malloc(sizeof(ProcessControlBlock_t));
    process2->arrival = 4;
    process2->remaining_burst_time = 5;
    process2->started = false;
    dyn_array_push_back(readyQueue, process2);
    ProcessControlBlock_t* process3 = (ProcessControlBlock_t*)malloc(sizeof(ProcessControlBlock_t));
    process3->arrival = 6;
    process3->remaining_burst_time = 1;
    process3->started = false;
    dyn_array_push_back(readyQueue, process3);
    bool result = shortest_job_first(readyQueue, sr);
    ASSERT_EQ(true, result);
    ASSERT_EQ(((float)29/3), sr->average_turnaround_time);
    ASSERT_EQ(((float)13/3), sr->average_waiting_time);
    ASSERT_EQ((unsigned long)17, sr->total_run_time);
}

/*
*
* TEST CASES
* bool round_robin(dyn_array_t *ready_queue, ScheduleResult_t *result, size_t quantum);
*
*/

TEST (round_robin, InvalidQuantum)
{
    ScheduleResult_t* sr = new ScheduleResult_t;
    dyn_array_t* readyQueue = dyn_array_create((sizeof(ProcessControlBlock_t)*4), sizeof(ProcessControlBlock_t), NULL);
    size_t quantum = 0;
    bool result = round_robin(readyQueue, sr, quantum);
    ASSERT_EQ(false, result);
}

TEST (round_robin, GoodSchedule)
{
    ScheduleResult_t* sr = new ScheduleResult_t;
    dyn_array_t* readyQueue = dyn_array_create((sizeof(ProcessControlBlock_t)*4), sizeof(ProcessControlBlock_t), NULL);
    ProcessControlBlock_t* process1 = (ProcessControlBlock_t*)malloc(sizeof(ProcessControlBlock_t));
    process1->arrival = 0;
    process1->remaining_burst_time = 10;
    process1->started = false;
    dyn_array_push_back(readyQueue, process1);
    ProcessControlBlock_t* process2 = (ProcessControlBlock_t*)malloc(sizeof(ProcessControlBlock_t));
    process2->arrival = 4;
    process2->remaining_burst_time = 5;
    process2->started = false;
    dyn_array_push_back(readyQueue, process2);
    ProcessControlBlock_t* process3 = (ProcessControlBlock_t*)malloc(sizeof(ProcessControlBlock_t));
    process3->arrival = 6;
    process3->remaining_burst_time = 6;
    process3->started = false;
    dyn_array_push_back(readyQueue, process3);
    ProcessControlBlock_t* process4 = (ProcessControlBlock_t*)malloc(sizeof(ProcessControlBlock_t));
    process4->arrival = 11;
    process4->remaining_burst_time = 1;
    process4->started = false;
    dyn_array_push_back(readyQueue, process4);
    bool result = round_robin(readyQueue, sr, QUANTUM);
    ASSERT_EQ(true, result);
    ASSERT_EQ((float)47/4, sr->average_turnaround_time);
    ASSERT_EQ((float)25/4, sr->average_waiting_time);
    ASSERT_EQ((unsigned long)22, sr->total_run_time);
}

/*
*
* TEST CASES
* dyn_array_t *load_process_control_blocks(const char *input_file) 
*
*/

TEST(load_process_control_blocks, NullInputFile){    
    dyn_array_t* process_control_blocks = (dyn_array_t*)load_process_control_blocks(NULL);
    ASSERT_EQ(NULL, process_control_blocks);
    free(process_control_blocks);
    
    score += 20;
}

TEST(load_process_control_blocks, ValidFile) {
    const char* pcbfile = "mypcbfile.bin";
    
    FILE* fp = fopen(pcbfile, "rb");
    uint32_t array[10] = {3,10,0,0,20,0,1,5,0,2};
    fwrite(array, sizeof(array),1,fp);
    fclose(fp);
    
    dyn_array_t* d = (dyn_array_t*)load_process_control_blocks(pcbfile);
    ASSERT_NE(nullptr, d);   //NE = not equal
    
    free(d);
    
    score += 20;
    
    // check d->capacity
    // traverse through d->array and see if it is properly loaded
    // check d->data_size
}

/*
*
* TEST CASES
* bool shortest_remaining_time_first(dyn_array_t *ready_queue, ScheduleResult_t *result) 
*
*/

TEST (shortest_remaining_time_first, NullReadyQueue){
    ScheduleResult_t *sr = new ScheduleResult_t;
    dyn_array_t *dynArray = NULL;
    bool result = shortest_remaining_time_first(dynArray, sr);
    ASSERT_EQ(false, result);
    delete sr;

    score += 20;
}

TEST (shortest_remaining_time_first, NullScheduleResult){
    ScheduleResult_t *sr = NULL;
    dyn_array_t *dynArray = dyn_array_create(0, sizeof(ProcessControlBlock_t), NULL);
    bool result = shortest_remaining_time_first(dynArray, sr);
    ASSERT_EQ(false, result);
    dyn_array_destroy(dynArray);
}

TEST (shortest_remaining_time_first, validInput) 
{
    ScheduleResult_t *sr = new ScheduleResult_t;
    dyn_array_t* dynArray = dyn_array_create(0,sizeof(ProcessControlBlock_t),NULL);
    memset(sr,0,sizeof(ScheduleResult_t));
    // add PCBs now
    ProcessControlBlock_t data[4] = 
    {
        [0] = {25,2,0,0},
        [1] = {2,3,1,0},
        [2] = {4,1,2,0},
        [3] = {1,4,3,0},
    };
    // back loading dyn_array, pull from the back
    dyn_array_push_back(dynArray,&data[3]);
    dyn_array_push_back(dynArray,&data[2]);
    dyn_array_push_back(dynArray,&data[1]);
    dyn_array_push_back(dynArray,&data[0]);	

    bool res = shortest_remaining_time_first (dynArray,sr);	
    ASSERT_EQ(true,res);

    float answers[3] = {10.25,2.25,32};
    ASSERT_EQ(answers[0],sr->average_turnaround_time);
    ASSERT_EQ(answers[1],sr->average_waiting_time);
    ASSERT_EQ(answers[2],sr->total_run_time);
    dyn_array_destroy(dynArray);

    delete sr;

    score+=20;
}

class GradeEnvironment : public testing::Environment 
{
    public:
        virtual void SetUp() 
        {
            score = 0;
            total = 210;
        }

        virtual void TearDown()
        {
            ::testing::Test::RecordProperty("points_given", score);
            ::testing::Test::RecordProperty("points_total", total);
            std::cout << "SCORE: " << score << '/' << total << std::endl;
        }
};


int main(int argc, char **argv) 
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new GradeEnvironment);
    return RUN_ALL_TESTS();
}
