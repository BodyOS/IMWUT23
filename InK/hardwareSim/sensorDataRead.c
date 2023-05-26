/*
 * sensorDataRead.c
 *
 *  Created on: Nov 2, 2022
 *      Author: erenyildiz
 */
#include <msp430.h>
#include "fram.h"
#include <stdlib.h>
#include <stdint.h>
#include "sensorDataRead.h"
#include "energyHarvester.h"

__nv uint16_t cur_data_num =0;
__nv uint16_t sensorDataPeriod =1480;
__nv uint32_t missingData = 0;
__nv uint32_t missingDataArr [10];
__nv int miscount = 0;
void SensorUpdate(){
    TA2CCR0 = sensorDataPeriod;//max 65535
    TA2CTL = TASSEL__ACLK + MC_1;  //set the max period for 16bit timer operation
    TA2CCTL0 = CCIE;  //enable compare reg 0
//    TA2R = 0x00;
    _BIS_SR( GIE); //ENABLE GLOBAL INTERRRUPTS
}


void __attribute__ ((interrupt(TIMER2_A0_VECTOR))) Timer_A2 (void)
{
    P3OUT ^= 0x01;
    P3OUT ^= 0x01;
    cur_data_num++;
    if(PF_flag){
        missingData++;
    }

}
