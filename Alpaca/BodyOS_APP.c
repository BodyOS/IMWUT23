/*
 * BodyOS_APP.c
 *
 *  Created on: Nov 7, 2022
 *      Author: erenyildiz
 */

#include <msp430.h>
#include <stdlib.h>
#include <libalpaca/alpaca.h>
#include <libmsp/mem.h>
#include <libmsp/watchdog.h>
#include "dataset.h"

#define MEM_SIZE 0x4
__nv uint8_t *data_src[MEM_SIZE];
__nv uint8_t *data_dest[MEM_SIZE];
__nv unsigned int data_size[MEM_SIZE];
void clear_isDirty() {}

void init();
void task_temp();
void task_HR();
void task_RR();


    TASK(1, task_temp)
    TASK(2, task_HR)
    TASK(3, task_RR)


static void init_hw()
{
    msp_watchdog_disable();
    P3OUT = 0x03;
    P1OUT = 0x00;
    PM5CTL0 &= ~LOCKLPM5; // Disable the GPIO power-on default high-impedance mode
    P3DIR = 0x03;
    P1DIR = 0x03;
    //msp_gpio_unlock();
    //msp_clock_setup();
}

void init() {

    init_hw();
    clockStart();
    ExeTime_sim_start();
    SensorUpdate();
    //INIT_CONSOLE();

    __enable_interrupt();

}

void task_temp() {

    int volatile res = 0;// read data from temperature sensor



        res = temp[cur_data_num];
        P1OUT = 0x01;
        __delay_cycles(700);
        P1OUT = 0x00;
        __delay_cycles(2000);

    TRANSITION_TO(task_HR);
}

void task_HR() {

    int res = 0;// read data from temperature sensor

        res = HR[cur_data_num];
        P1OUT = 0x01;
        __delay_cycles(900);
        P1OUT = 0x00;
        __delay_cycles(4000);

       TRANSITION_TO(task_RR);
}
void task_RR() {

    int res = 0;// read data from temperature sensor

            res = RR[cur_data_num];
            P1OUT = 0x01;
            __delay_cycles(800);
            P1OUT = 0x00;
            __delay_cycles(3000);
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
        TRANSITION_TO(task_temp);

}
ENTRY_TASK(task_temp)
INIT_FUNC(init)



