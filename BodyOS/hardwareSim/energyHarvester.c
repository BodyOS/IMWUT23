/*
 * energyHarvester.c
 *
 *  Created on: Nov 1, 2022
 *      Author: erenyildiz
 */


#include <msp430.h>

#include "energyHarvester.h"
/**
 * main.c
 */
#define PF_SIMULATION
#ifdef PF_SIMULATION
//__nv int timer_count = 1500;
__nv int CapSize = 1000; // uF
__nv int E_tot = 230; // uJ (VH: 1.25V - VL: 1.05V)
__nv int E_remaining = 230;
__nv int targetEnergy;
__nv int Vs = 33; // Boosted Voltage
__nv int I_msp = 100; // uA
__nv int P_in = 700;  //uW
__nv int exe_time_coff =4852;  // ~132 ms
__nv int charging_time_coff;   // ~328 ms
__nv bool PF_flag;
void Reset() {
//   PF_number++;
    PMMCTL0 = 0x0008;
}
#endif

void ExeTime_sim_start()
{
    TA0CCR0 = exe_time_coff;//max 65535
    TA0CTL = TASSEL__ACLK + MC_1;  //set the max period for 16bit timer operation
    TA0CCTL0 = CCIE;  //enable compare reg 0
    TA0R = 0x00;
    _BIS_SR( GIE); //ENABLE GLOBAL INTERRRUPTS
}

void Carging_sim_start(int TargetEnergy)
{

    charging_time_coff = GetChargingTime( E_remaining, TargetEnergy);
    if(charging_time_coff>0){
        targetEnergy = TargetEnergy;
        TA1CCR0 = charging_time_coff*37;//max 65535
        TA1CTL = TASSEL__ACLK + MC_1;  //set the max period for 16bit timer operation
        TA1CCTL0 = CCIE;  //enable compare reg 0
        TA1R = 0x00;
        _BIS_SR( GIE); //ENABLE GLOBAL INTERRRUPTS
        __bis_SR_register(LPM3_bits | GIE);
    }

}

// Timer A0 interrupt service routine
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A (void)
{
    TA0CCTL0 &= ~CCIE;
    PF_flag = 1;
    Carging_sim_start(E_tot);


//    __bic_SR_register_on_exit(LPM3_bits);
}

// Timer A1 interrupt service routine
void __attribute__ ((interrupt(TIMER1_A0_VECTOR))) Timer_A1 (void)
{
    TA0CCTL0 &= ~CCIE; // Disable execution timer
//    TA1CCTL0 = CCIE_0;  // disable Charging_sim
    E_remaining = targetEnergy;

    if (PF_flag)
        Reset();
    __bic_SR_register_on_exit(LPM3_bits);
    ExeTime_sim_start();


}

int GetChargingTime(int RemainingEnergy, int TargetEnergy){
    int energy_need = TargetEnergy - RemainingEnergy;
    int time = (int)(((float)(energy_need) / (float)P_in)*1000);
    return time;
}


