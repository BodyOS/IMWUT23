/*
 * energyHarvester.h
 *
 *  Created on: Nov 1, 2022
 *      Author: erenyildiz
 */

#ifndef HARDWARESIM_ENERGYHARVESTER_H_
#define HARDWARESIM_ENERGYHARVESTER_H_
#include "fram.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
extern int CapSize; // uF
extern int E_tot; // uJ (VH: 1.25V - VL: 1.05V)
extern int E_remaining;
extern int targetEnergy;
extern int Vs; // Boosted Voltage
extern int I_msp ; // uA
extern int exe_time_coff;  // ~132 ms
extern int charging_time_coff;   // ~328 ms
extern int P_in;
extern bool PF_flag;
void ExeTime_sim_start();
void Carging_sim_start();
#endif /* HARDWARESIM_ENERGYHARVESTER_H_ */
