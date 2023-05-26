/*
 * temperature_alpaca.c
 *
 *  Created on: Jun 15, 2022
 *      Author: erenyildiz
 */



#include <stdio.h>
#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <libalpaca/alpaca.h>
#include <libmsp/watchdog.h>
#include "mayfly.h"


#define MEM_SIZE 0x4
__nv uint8_t *data_src[MEM_SIZE];
__nv uint8_t *data_dest[MEM_SIZE];
__nv unsigned int data_size[MEM_SIZE];
void clear_isDirty() {}

/* APP functions */
int ReadMoist();
int ReadTemp();
int ReadLight();
void Send();

/* Timekeeper Function */
uint32_t GetTime();

/* Alpaca Task definitions */
void init();
void task_moist();
void task_light();
void task_temp();
void task_wet();
void task_send();
TASK(1,  task_moist)
TASK(2,  task_light)
TASK(3,  task_temp)
TASK(4,  task_wet)
TASK(5,  task_send)

/* Mayfly Edge variables (inputs and outputs of the tasks). The type should be edge_var. */
__nv edge_var temperature = {.state = 0};
__nv edge_var light = {.state = 0};
__nv edge_var moisture = {.state = 0};
__nv edge_var leaf_wetness = {.state = 0};


void task_moist()
{

    /* This Macro reads sensor value and labels the time stamp */
    SaveOutput(moisture, ReadMoist(), GetTime()); // SaveOutput(outputVariable, function, Time for timestamp)

    /*For task transition MAYFLY_TRANSITION macro should be used since it gong forward according tto the defined path by programmer*/
    MAYFLY_TRANSITION;
}

void task_light()
{
    /* This Macro reads sensor value and labels the time stamp */
    SaveOutput(light, ReadMoist(), GetTime());// SaveOutput(outputVariable, function, Time for timestamp)

    MAYFLY_TRANSITION;
}
void task_temp()
{
    /* This Macro reads sensor value and labels the time stamp */
    SaveOutput(temperature, ReadMoist(), GetTime());// SaveOutput(outputVariable, function, Time for timestamp)

    /*For task transition MAYFLY_TRANSITION macro should be used since it gong forward according tto the defined path by programmer*/
    MAYFLY_TRANSITION;
}

void task_wet()
{
    int moist, temp, lightLevel, leaf;

    /* This Macro gets the read sensor value. The data type should be given */
    GetInput(int, temp, temperature); //GetInput(type, Var, EdgeVar(input/output))

    /* This Macro gets the read sensor value. The data type should be given */
    GetInput(int, lightLevel, light);

    /* This Macro gets the read sensor value. The data type should be given */
    GetInput(int, moist, moisture);


    leaf = (temp + lightLevel + moist)/3;


    /* This Macro reads sensor value and labels the time stamp */
    SaveOutput(leaf_wetness, leaf, GetTime());// SaveOutput(outputVariable, function, Time for timestamp)

    /*For task transition MAYFLY_TRANSITION macro should be used since it gong forward according tto the defined path by programmer*/
    MAYFLY_TRANSITION;
}


void task_send()
{

    void Send();
    MAYFLY_TRANSITION;
}
static void init_hw()
{
    msp_watchdog_disable();
    PM5CTL0 &= ~LOCKLPM5;
}

void init()
{

    init_hw();

    /*Mayfly task definition, data flows, and edge constraints should be
     *defined in init() function. The definition must be located between
     *defined MAYFLY_BEGIN/MAYFLY_END. You can check Listing1 in the Mayfly
     *paper for a better demonstration of the definition logic.
    */
    MAYFLY_BEGIN
        MAYFLY_TASK(1,  task_moist); // MAYFLY_TASK(taskID, taskName)
        MAYFLY_TASK(2,  task_light);
        MAYFLY_TASK(3,  task_temp);
        MAYFLY_TASK(4,  task_wet);
        MAYFLY_TASK(5,  task_send);

        DATA_FLOWS(1, 3, 4, 5);  // DATA_FLOWS(PathID, PathOrder) - for this Data flow; PathId: 1, PathOrder: 3(task_temp) -> 4(task_wet) -> 5(task_send)
        DATA_FLOWS(2, 2, 4);
        DATA_FLOWS(3, 1, 4);

        EDGE_CONST(4, 1, temperature, 60, 1, 1); // EDGE_CONST(TaskID, ConstID, ConstVariable, ExpireTime, CollectNum, PathID) - For this Edge TaskID:4(task_wet)
        // ConstID of the Task: 1(first constraint of the wet task), ConstVariable: temperature, ExpireTime: 60 sec, CollectNum: 1, PathID: 1 (since the temp task is on the first path)
        EDGE_CONST(4, 2, light, 10, 1, 2);
        EDGE_CONST(4, 3, moisture, 120, 1, 3);
        EDGE_CONST(5, 1, leaf_wetness, 120, 1, 1);
    MAYFLY_END


    __enable_interrupt();

}

int ReadMoist(){
    return 5;
}
int ReadTemp(){
    return 8;
}
int ReadLight(){
    return 10;
}

void Send(){

}

uint32_t GetTime(){

    return 8;
}
MAYFLY_ENTRY
INIT_FUNC(init)
