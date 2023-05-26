/*
 * msp430fr5969.c
 *
 *  Created on: 15 Feb 2018
 *      Author: sinanyil81
 */

#include <mcu/msp430/msp430fr5969.h>

void __mcu_init() {

  WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer


  // Disable FRAM wait cycles to allow clock operation over 8MHz
//  FRCTL0 = 0xA500 | ((1) << 4); // FRCTLPW | NWAITS_1;
//  __delay_cycles(3);

  /* init FRAM */
//  FRCTL0_H |= (FWPW) >> 8;
//  __delay_cycles(3);

//  __led_init(LED1);
//  __led_init(LED2);
  P3OUT = 0x03;
  P1OUT = 0x00;
  PM5CTL0 &= ~LOCKLPM5; // Disable the GPIO power-on default high-impedance mode
  P3DIR = 0x03;
  P1DIR = 0x03;
}
