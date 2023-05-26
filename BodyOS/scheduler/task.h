/*
 * task.h
 *
 *  Created on: 12 Feb 2018
 *      Author: sinanyil81
 */

#ifndef TASK_H_
#define TASK_H_

typedef struct tcb_t{
    void *function;
    int max;
    int min;
    uint64_t freshness;
    uint64_t timestamp;
    int energy_required;
    int is_dependent; // a boolean flag
    int dep_count; // only difference from independent tasks is that it has
    struct tcb_t* dependent[3];

}tcb_t __attribute__ ((aligned (2)));

//#define DEP_TASK(name,max,min,freshness)  \
//    __nv tcb_t ##name##_tcb;\
//    ##name##_tcb->function = *name;\
//    static void *name(void *__buffer)
 
#define DEP_TASK(name) \
    tcb_t _##name##_tcb; \
    static int name(void *__buffer)

#define INDEP_TASK(name) \
    tcb_t _##name##_tcb; \
    static int name(void *__buffer)

#define INDEP_TASK_CONSTRAINTS(t,m,n,fresh,energy) \
    _##t##_tcb.is_dependent = 0;\
    _##t##_tcb.dep_count=0; \
    _##t##_tcb.max = m; \
    _##t##_tcb.min = n;\
    _##t##_tcb.freshness = fresh; \
    _##t##_tcb.function = t; \
    _##t##_tcb.energy_required = energy;

#define DEP_TASK_CONSTRAINTS(t,m,n,fresh,energy) \
    _##t##_tcb.is_dependent = 1;\
    _##t##_tcb.dep_count=0; \
    _##t##_tcb.max = m; \
    _##t##_tcb.min = n;\
    _##t##_tcb.freshness = fresh; \
    _##t##_tcb.function = t; \
    _##t##_tcb.energy_required = energy;

#define DEPENDS_ON(t2,t1) _##t1##_tcb.dependent[_##t1##_tcb.dep_count++]= &_##t2##_tcb;

// #define TASK(name)  tcb_t _##name##_tcb; static void *name(void *__buffer)

#define ENTRY_TASK(name)  static void *name(void *__buffer,isr_event_t *__event)

// reads the value from the original stack
#define __GET(x) ( (FRAM_data_t *)__buffer)->x

// returns the address of the variable
#define __GET_ADDR(x) &( (FRAM_data_t *)__buffer-)->x

// writes the value to the temporary stack
#define __SET(x,val) ( (FRAM_data_t *)__buffer)->x = val


// creates a thread
#define __CREATE(priority,entry)  \
        __create_thread(priority,&_##entry##_tcb,(void *)&__persistent_vars[0],(void *)&__persistent_vars[1],sizeof(FRAM_data_t))


//// creates a thread
//#define __CREATE(priority,entry)  \
//        __create_thread(priority,(void *)entry,(void *)&__persistent_vars[0],(void *)&__persistent_vars[1],sizeof(FRAM_data_t))

// puts the thread state into ACTIVE
#define __SIGNAL(priority) \
        __disable_interrupt();  \
        __start_thread(__get_thread(priority)); \
        __enable_interrupt()

// event related information
#define __EVENT __event
#define __EVENT_DATA __event->data
#define __EVENT_DATALEN __event->size
#define __EVENT_TIME __event->timestamp

#endif /* TASK_H_ */
