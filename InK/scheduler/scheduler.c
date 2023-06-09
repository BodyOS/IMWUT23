/*
 * scheduler.c
 *
 *  Created on: 12 Feb 2018
 *      Author: sinanyil81
 */

#include "ink.h"
#include "scheduler.h"
#include "priority.h"

// all threads in the system
#pragma PERSISTENT(_threads)
static thread_t _threads[MAX_THREADS];

// variables for keeping track of the ready threads
#pragma PERSISTENT(_priorities)
static priority_t _priorities;

enum { SCHED_SELECT, SCHED_BUSY };

// the id of the current thread being executed.
#pragma PERSISTENT(_thread)
static thread_t *_thread = NULL;

#pragma PERSISTENT(_sched_state)
static volatile uint8_t _sched_state = SCHED_SELECT;

void __scheduler_boot_init() {
    uint8_t i;

    // clear priority variables for the threads
    __priority_init(&_priorities);

    for (i = MAX_THREADS; i > 0; i--){
        // threads are not created yet
        _threads[i].state == THREAD_STOPPED;
    }
    _sched_state = SCHED_SELECT;
}

// Assigns a slot to a thread. Should be called ONLY at the first system boot
void __create_thread(uint8_t priority, void *entry, void *data_org,
                     void *data_temp, uint16_t size)
{
    // init properties
    _threads[priority].priority = priority;
    _threads[priority].entry = entry;
    _threads[priority].next = entry;
    _threads[priority].state = THREAD_STOPPED;

    // init shared buffer
    _threads[priority].buffer.buf[0] = data_org;
    _threads[priority].buffer.buf[1] = data_temp;
    _threads[priority].buffer.idx = 0;
    _threads[priority].buffer.size = size;
}

// puts the thread in waiting state
inline void __stop_thread(thread_t *thread){
    __priority_remove(thread->priority, &_priorities);
    thread->state = THREAD_STOPPED;
}

// puts the thread in active state
inline void __start_thread(thread_t *thread) {
    thread->next = thread->entry;
    __priority_insert(thread->priority, &_priorities);
    thread->state = TASK_READY;
}

// returns the highest-priority thread
static inline thread_t *__next_thread(){
    uint8_t idx = 15;//__priority_highest(&_priorities);
    if(idx)
        return &_threads[idx];

    return NULL;
}

inline thread_t *__get_thread(uint8_t priority){
    return &_threads[priority];
}

// finish the interrupted task before enabling interrupts
static inline void __task_commit(){
    if(_thread){
        __tick(_thread);
    }
}

// at each step, the scheduler selects the highest priority thread and
// runs the next task within the thread
void __scheduler_run()
{
    // For the sake of consistency, the event insertion by an ISR which
    // was interrupted by a power failure should be committed to the
    // event queue _events in isrmanager.c before enabling the interrupts.
    __events_commit();

    // always finalize the latest task before enabling interrupts since
    // this task might be interrupted by a power failure and the changes
    // it performs on the system variables (e.g. on _priorities due to
    // signaling another task or on the event queue _events in isrmanager.c)
    // will be committed before enabling interrupts so that these variables
    // remain consistent and stable.
    __task_commit();

    // enable interrupts
//    __enable_interrupt();

    while (1){
        switch (_sched_state){
        case SCHED_SELECT:
            // the scheduler selects the highest priority task right
            // after it has finished the execution of a single task
            _thread = __next_thread();
            _sched_state = SCHED_BUSY;
        case SCHED_BUSY:
            // always execute the selected task to completion
            // execute one task inside the highest priority thread
            if (_thread){
                __tick(_thread);
                // after execution of one task, check the events
                _sched_state = SCHED_SELECT;
                break;
            }
            _sched_state = SCHED_SELECT;
            __disable_interrupt();
            // check the ready queue for the last time
            if(!__next_thread()){
                __mcu_sleep();
                __enable_interrupt();
            }
        }
    }
}
