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
}

TEST (first_come_first_serve, NullScheduleResult){
    ScheduleResult_t *sr = NULL;
    dyn_array_t *array = dyn_array_create(0, sizeof(ProcessControlBlock_t), NULL);
    bool result = first_come_first_serve(array, sr);
    ASSERT_EQ(false, result);
    dyn_array_destroy(array);
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
