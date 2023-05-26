/*
 * BodyOS_App.c
 *
 *  Created on: Nov 7, 2022
 *      Author: erenyildiz
 */

#include <msp430.h>
#include <stdlib.h>
#include <libalpaca/alpaca.h>
#include <libmsp/mem.h>
#include <libmsp/watchdog.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include "mayfly.h"
#include "uart.h"
#include "config.h"
#include "algorithm.h"
#include "DSPLib.h"

volatile int adcReading = 0;

#pragma PERSISTENT(temp_per)
unsigned int temp_per;

#define LENGTH 3000
#define FIR_LENGTH 64
#define SIGNAL_LENGTH 256

#ifndef FILTER_COEFFS_EX1
const _q15 FILTER_COEFFS_EX1[48] = {_Q15(0.00243168555283467),
                                    _Q15(0.00262513856308543),
                                    _Q15(0.00307384237893265),
                                    _Q15(0.0037900357208569),
                                    _Q15(0.00477952678992585),
                                    _Q15(0.00604136212379601),
                                    _Q15(0.00756766739955613),
                                    _Q15(0.00934366688694775),
                                    _Q15(0.0113478834588433),
                                    _Q15(0.013552516176688),
                                    _Q15(0.015923987622737),
                                    _Q15(0.0184236484836823),
                                    _Q15(0.0210086225326944),
                                    _Q15(0.0236327712319624),
                                    _Q15(0.0262477537968826),
                                    _Q15(0.0288041558227164),
                                    _Q15(0.0312526575538251),
                                    _Q15(0.0335452116336212),
                                    _Q15(0.0356361997476832),
                                    _Q15(0.037483537977819),
                                    _Q15(0.0390497019128194),
                                    _Q15(0.0403026445807066),
                                    _Q15(0.0412165830238057),
                                    _Q15(0.0417726327575467),
                                    _Q15(0.0419592733434863),
                                    _Q15(0.0417726327575467),
                                    _Q15(0.0412165830238057),
                                    _Q15(0.0403026445807066),
                                    _Q15(0.0390497019128194),
                                    _Q15(0.037483537977819),
                                    _Q15(0.0356361997476832),
                                    _Q15(0.0335452116336212),
                                    _Q15(0.0312526575538251),
                                    _Q15(0.0288041558227164),
                                    _Q15(0.0262477537968826),
                                    _Q15(0.0236327712319624),
                                    _Q15(0.0210086225326944),
                                    _Q15(0.0184236484836823),
                                    _Q15(0.015923987622737),
                                    _Q15(0.013552516176688),
                                    _Q15(0.0113478834588433),
                                    _Q15(0.00934366688694775),
                                    _Q15(0.00756766739955613),
                                    _Q15(0.00604136212379601),
                                    _Q15(0.00477952678992585),
                                    _Q15(0.0037900357208569),
                                    _Q15(0.00307384237893265),
                                    _Q15(0.00262513856308543),
                                    _Q15(0.00243168555283467)};
#endif

#define COEFF_LENTH sizeof(FILTER_COEFFS_EX1) / sizeof(FILTER_COEFFS_EX1[0])
DSPLIB_DATA(circularBuffer, MSP_ALIGN_FIR_Q15(FIR_LENGTH))
_q15 circularBuffer[2 * FIR_LENGTH];
/* Filter coefficients */
DSPLIB_DATA(filterCoeffs, 4)
_q15 filterCoeffs[COEFF_LENTH];

//volatile int adcReading = 0;

char string[40];

#pragma PERSISTENT(readings_per)
_q15 readings_per[LENGTH] = {0};

#pragma PERSISTENT(temp_per)
unsigned int temp_per;

DSPLIB_DATA(readings, 4)
_q15 readings[SIGNAL_LENGTH] = {0};

#pragma PERSISTENT(filtered_per)
_q15 filtered_per[LENGTH] = {0};

DSPLIB_DATA(filtered, 4)
_q15 filtered[SIGNAL_LENGTH] = {0};
 i = 0;
int k;
int p, q;
int32_t an_ir_valley_locs[15];
float n_peak_interval_sum;
int zz;

#define MEM_SIZE 0x4
#pragma PERSISTENT(data_src)
uint8_t *data_src[MEM_SIZE];

#pragma PERSISTENT(data_dest)
uint8_t *data_dest[MEM_SIZE];

#pragma PERSISTENT(data_size)
unsigned int data_size[MEM_SIZE];

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
//void task_RR();
//void task_cough();
TASK(1,  task_HR)
TASK(2,  task_temp)
//TASK(3,  task_cough)
//TASK(3,  task_RR)
/* Mayfly Edge variables (inputs and outputs of the tasks). The type should be edge_var. */
#pragma PERSISTENT(temperature)
edge_var temperature = {.state = 0};
#pragma PERSISTENT(HR_data)
edge_var HR_data = {.state = 0};
//__nv edge_var cough = {.state = 0};
//__nv edge_var RR_data = {.state = 0};


void task_temp()
{

#ifdef DEBUG
    uca0WriteString("Finding Temp");
#endif
    ADC12CTL0 = ADC12SHT0_2 | ADC12ON;        // Sampling time, S&H=16, ADC12 on
    ADC12CTL1 = ADC12SHP ;                     // Use sampling timer
    ADC12CTL2 |= ADC12RES_2;                  // 12-bit conversion results
    ADC12MCTL1 |= ADC12VRSEL_0 | ADC12INCH_3; // 2 ADC input select; Vref=AVCC
    ADC12IER0 |= ADC12IE1;                    // Enable ADC conv complete interrupt
    ADC12CTL3 = ADC12CSTARTADD_1;

    ADC12CTL0 |= ADC12ENC | ADC12SC; // Start sampling/conversion
    __bis_SR_register(LPM0_bits | GIE); // LPM0, ADC12_ISR will force exit

    unsigned int deg_c = ((1035 - temp_per)*1.0)/5.5;

#ifdef DEBUG
    sprintf(string, "Temperature : %d *C\n", deg_c);
    uca0WriteString(string);
#endif

    ADC12CTL0 = 0x0000;


    /*For task transition MAYFLY_TRANSITION macro should be used since it gong forward according tto the defined path by programmer*/
    MAYFLY_TRANSITION;
}

void task_HR()
{

    uint16_t samples;
    uint16_t copyindex;
    uint16_t filterIndex;
    msp_status status;
    msp_fir_q15_params firParams;
    msp_fill_q15_params fillParams;
    msp_copy_q15_params copyParams;

#ifdef DEBUG
    uca0WriteString("Finding HR\n");
#endif

    ADC12CTL0 = ADC12SHT0_2 | ADC12ON;        // Sampling time, S&H=16, ADC12 on
    ADC12CTL1 = ADC12SHP;                     // Use sampling timer
    ADC12CTL2 |= ADC12RES_2;                  // 12-bit conversion results
    ADC12MCTL0 |= ADC12VRSEL_0 | ADC12INCH_2; // A1 ADC input select; Vref=AVCC
    ADC12IER0 |= ADC12IE0;                    // Enable ADC conv complete interrupt
    ADC12CTL3 = ADC12CSTARTADD_0;

    P1OUT &= ~(BIT4);
    P1OUT &= ~(BIT5);

    P3OUT |= BIT0; // Shutdown sensor
    int done =0;
    while(!done)
    {

        if (i == LENGTH)
        {
            __no_operation();
            P1OUT |= (BIT5);
            int pn_heart_rate, pch_hr_valid;
            for (k = 0; k < LENGTH - 4; k++)
            {
                readings[k] = (readings[k] + readings[k + 1] + readings[k + 2] + readings[k + 3]) / (int)4;
            }
            memcpy(filterCoeffs, FILTER_COEFFS_EX1, sizeof(filterCoeffs));
            fillParams.length = FIR_LENGTH * 2;
            fillParams.value = 0;
            status = msp_fill_q15(&fillParams, circularBuffer);
            msp_checkStatus(status);
            /* Initialize the copy parameter structure. */
            copyParams.length = FIR_LENGTH;
            /* Initialize the FIR parameter structure. */
            firParams.length = FIR_LENGTH;
            firParams.tapLength = COEFF_LENTH;
            firParams.coeffs = filterCoeffs;
            firParams.enableCircularBuffer = true;
            /* Initialize counters. */
            samples = 0;
            copyindex = 0;
            filterIndex = 2 * FIR_LENGTH - COEFF_LENTH;
            /* Run FIR filter with 128 sample circular buffer. */

            while (samples < LENGTH)
            {
                memcpy(&readings, &readings_per[samples], sizeof(readings_per[0]) * FIR_LENGTH);
                /* Copy FIR_LENGTH samples to filter input. */
                status = msp_copy_q15(&copyParams, &readings, &circularBuffer[copyindex]);
                msp_checkStatus(status);
                /* Invoke the msp_fir_q15 function. */

                status = msp_fir_q15(&firParams, &circularBuffer[filterIndex], &filtered);

                msp_checkStatus(status);
                memcpy(&filtered_per[samples], &filtered, sizeof(filtered[0]) * FIR_LENGTH);
                /* Update counters. */
                copyindex ^= FIR_LENGTH;
                filterIndex ^= FIR_LENGTH;
                samples += FIR_LENGTH;
            }
            int32_t n_th1 = 0;

            for (i = 0; i < 4000; i++)
            {
                filtered_per[i] *= -1;
                n_th1 += filtered_per[i];
            }
            n_th1 = n_th1 / 4000;
            for (i = 0; i < 4000; i++)
            {
                filtered_per[i] -= n_th1;
            }

            n_th1 = 50;
            for (k = 0; k < 15; k++)
                an_ir_valley_locs[k] = 0;
            int32_t n_npks;
            // since we flipped signal, we use peak detector as valley detector
            maxim_find_peaks(an_ir_valley_locs, &n_npks, filtered_per, LENGTH, n_th1, 100, 15); // peak_height, peak_distance, max_num_peaks
            n_peak_interval_sum = 0;
            if (n_npks >= 2)
            {
                for (k = 1; k < n_npks; k++)
                    n_peak_interval_sum += (an_ir_valley_locs[k] - an_ir_valley_locs[k - 1]);
                n_peak_interval_sum = n_peak_interval_sum / (n_npks - 1);
                pn_heart_rate = ((float)60 / (n_peak_interval_sum / 1600));
                pch_hr_valid = 1;
            }
            else
            {
                pn_heart_rate = -999; // unable to calculate because # of peaks are too small
                pch_hr_valid = 0;
            }

#ifdef DEBUG
            sprintf(string, "Heart rate: %d\n", pn_heart_rate);
            uca0WriteString(string);
#endif

            P1OUT |= (BIT4);

            i = 0;
            done = 1;
        }

        P3OUT &= ~(BIT0); // Start sensor
        __delay_cycles(5000);

        ADC12CTL0 |= ADC12ENC | ADC12SC; // Start sampling/conversion

        __bis_SR_register(LPM0_bits | GIE); // LPM0, ADC12_ISR will force exit

#ifdef DEBUG
        P1OUT &= ~(BIT4);
#endif

        //        delay(1); //stop collecting samples for 1ms to reduce sampling rate by 3

    }

    /*For task transition MAYFLY_TRANSITION macro should be used since it gong forward according tto the defined path by programmer*/
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
    configClock();
    disableUnwantedGPIO();
    initTMR();

#ifdef DEBUG
    uca0Init();
#endif

    FRCTL0 = FRCTLPW;
    GCCTL0 = FRPWR | 0x02;



    /*Mayfly task definition, data flows, and edge constraints should be
     *defined in init() function. The definition must be located between
     *defined MAYFLY_BEGIN/MAYFLY_END. You can check Listing1 in the Mayfly
     *paper for a better demonstration of the definition logic.
     */
    MAYFLY_BEGIN
    MAYFLY_TASK(1,  task_temp); // MAYFLY_TASK(taskID, taskName)
    MAYFLY_TASK(2,  task_HR);
//    MAYFLY_TASK(3,  task_cough);
//    MAYFLY_TASK(4,  task_RR);
    DATA_FLOWS(1, 1, 2, 3, 4);  // DATA_FLOWS(PathID, PathOrder) - for this Data flow; PathId: 1, PathOrder: 3(task_temp) -> 4(task_wet) -> 5(task_send)


    EDGE_CONST(4, 1, temperature, 30, 1, 1);
    EDGE_CONST(4,2, HR_data , 30, 1, 1); // EDGE_CONST(TaskID, ConstID, ConstVariable, ExpireTime, CollectNum, PathID) - For this Edge TaskID:4(task_wet)
    // ConstID of the Task: 1(first constraint of the wet task), ConstVariable: temperature, ExpireTime: 60 sec, CollectNum: 1, PathID: 1 (since the temp task is on the first path)

    //        EDGE_CONST(4, 3, cough, 30, 1, 1);
    MAYFLY_END


    //    __enable_interrupt();

}


MAYFLY_ENTRY
INIT_FUNC(init)




