/*
 * memory.h
 *
 *  Created on: 14 Feb 2018
 *      Author: sinanyil81
 */

#ifndef MEMORY_H_
#define MEMORY_H_

///* defines non-volatile variable */
//#ifdef __GNUC__
//    #define __nv    __attribute__((section(".persistent")))
//    #define __hifram    __attribute__((section(".persistent_hifram")))
//    #define __ro_hifram    __attribute__((section(".persistent_hifram")))
//#else
//    #define __nv __attribute__((section(".TI.persistent")))
//#endif


void __dma_copy(unsigned int from, unsigned int to, unsigned short size);

#endif /* MEMORY_H_ */
