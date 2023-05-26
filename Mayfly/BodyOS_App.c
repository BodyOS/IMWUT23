/*
 * BodyOS_App.c
 *
 *  Created on: Nov 7, 2022
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
#include "dataset.h"


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



/* Alpaca Task definitions */
void init();
void task_temp();
void task_HR();
void task_RR();
TASK(1,  task_temp)
TASK(2,  task_HR)
TASK(3,  task_RR)

/* Mayfly Edge variables (inputs and outputs of the tasks). The type should be edge_var. */
__nv edge_var temperature = {.state = 0};
__nv edge_var HR_data = {.state = 0};
__nv edge_var RR_data = {.state = 0};


void task_temp()
{

        P1OUT = 0x01;
        __delay_cycles(700);
        P1OUT = 0x00;
        __delay_cycles(2000);
    /* This Macro reads sensor value and labels the time stamp */
    SaveOutput(temperature, temp[cur_data_num], GetTime());// SaveOutput(outputVariable, function, Time for timestamp)

    /*For task transition MAYFLY_TRANSITION macro should be used since it gong forward according tto the defined path by programmer*/
    MAYFLY_TRANSITION;
}

void task_HR()
{


    P1OUT = 0x01;
    __delay_cycles(900);
    P1OUT = 0x00;
    __delay_cycles(4000);
    /* This Macro reads sensor value and labels the time stamp */
    SaveOutput(HR_data, HR[cur_data_num], GetTime());// SaveOutput(outputVariable, function, Time for timestamp)

    //int moist, temp, lightLevel, leaf;

    /* This Macro gets the read sensor value. The data type should be given */
    //GetInput(int, temp, temperature); //GetInput(type, Var, EdgeVar(input/output))

    /* This Macro gets the read sensor value. The data type should be given */
   // GetInput(int, lightLevel, light);

    /* This Macro gets the read sensor value. The data type should be given */
    //GetInput(int, moist, moisture);


    //leaf = (temp + lightLevel + moist)/3;


    /* This Macro reads sensor value and labels the time stamp */
    //SaveOutput(leaf_wetness, leaf, GetTime());// SaveOutput(outputVariable, function, Time for timestamp)

    /*For task transition MAYFLY_TRANSITION macro should be used since it gong forward according tto the defined path by programmer*/
    MAYFLY_TRANSITION;
}


void task_RR()
{

    P1OUT = 0x01;
    __delay_cycles(800);
    P1OUT = 0x00;
    __delay_cycles(3000);
    /* This Macro reads sensor value and labels the time stamp */
    SaveOutput(RR_data, RR[cur_data_num], GetTime());// SaveOutput(outputVariable, function, Time for timestamp)
    if(cur_data_num>=3000){
        _BIC_SR( GIE);
        missingDataArr[miscount] = missingData;
        miscount++;
        P_in = P_in - 100;
        cur_data_num = 0;
        missingData=0;
        PF_number_arr [miscount] = PF_number;
        PF_number = 0;
        charging_time_coff_arr[miscount] = charging_time_coff;
        if(P_in==100)
            __no_operation(); //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        _BIS_SR( GIE);
    }
    cur_data_num++;

    MAYFLY_TRANSITION;
}
static void init_hw()
{
    msp_watchdog_disable();
    P3OUT = 0x03;
    P1OUT = 0x00;
    PM5CTL0 &= ~LOCKLPM5;
    P3DIR = 0x03;
    P1DIR = 0x03;
}

void init()
{

    init_hw();

    clockStart();
    ExeTime_sim_start();
    SensorUpdate();
    /*Mayfly task definition, data flows, and edge constraints should be
     *defined in init() function. The definition must be located between
     *defined MAYFLY_BEGIN/MAYFLY_END. You can check Listing1 in the Mayfly
     *paper for a better demonstration of the definition logic.
    */
    MAYFLY_BEGIN
        MAYFLY_TASK(1,  task_temp); // MAYFLY_TASK(taskID, taskName)
        MAYFLY_TASK(2,  task_HR);
        MAYFLY_TASK(3,  task_RR);

        DATA_FLOWS(1, 1, 2, 3);  // DATA_FLOWS(PathID, PathOrder) - for this Data flow; PathId: 1, PathOrder: 3(task_temp) -> 4(task_wet) -> 5(task_send)


        EDGE_CONST(4, 1, temperature, 60000, 1, 1); // EDGE_CONST(TaskID, ConstID, ConstVariable, ExpireTime, CollectNum, PathID) - For this Edge TaskID:4(task_wet)
        // ConstID of the Task: 1(first constraint of the wet task), ConstVariable: temperature, ExpireTime: 60 sec, CollectNum: 1, PathID: 1 (since the temp task is on the first path)
        EDGE_CONST(4, 2, HR, 60000, 1, 1);

    MAYFLY_END


    __enable_interrupt();

}


MAYFLY_ENTRY
INIT_FUNC(init)




