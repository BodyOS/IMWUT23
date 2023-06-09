/*
 * eventsmanager.c
 *
 *  Created on: 19 Feb 2018
 *      Author: sinanyil81
 */
#include "ink.h"
#include "persistentqueue.h"
#include "scheduler/priority.h"

enum{   EVENT_INSERT, EVENT_COMMIT, EVENT_SIGNAL,EVENT_DONE } ;

#pragma PERSISTENT(_status)
static volatile uint8_t _status = EVENT_DONE;

// keep track of the current event insertion
#pragma PERSISTENT(thread_t)
static  thread_t *_thread;
#pragma PERSISTENT(_event)
static  isr_event_t _event;

// an event queue per each thread
#pragma PERSISTENT(_events)
static per_queue_t _events[MAX_THREADS];

// keep track of the popped event since tasks
// might be restarted
#pragma PERSISTENT(_popped)
static isr_event_t *_popped[MAX_THREADS];

// should be called at the first boot only
void __events_boot_init(){
    uint8_t i;

    for(i=MAX_THREADS;i>0;i--){
        // initialize each queue
        __perqueue_init(&_events[i]);
        _popped[i] = NULL;
    }
}

// This function will be executed withing the context of an ISR or
// it should be also called at each reboot to finish event insertion
void __events_commit(){
    switch(_status){
    case EVENT_INSERT:
        __perqueue_push(&_events[_thread->priority], &_event);
        _status = EVENT_COMMIT;
    case EVENT_COMMIT:
        __perqueue_push_commit(&_events[_thread->priority]);
        _status = EVENT_SIGNAL;
    case EVENT_SIGNAL:
        // if the thread is sleeping, activate it!
        if(_thread->state == THREAD_STOPPED){
            __start_thread(_thread);
        }
        _status = EVENT_DONE;
    }
}

// check if all slots are full or not
inline uint8_t __event_buffer_full_ISR(thread_t *thread){
    return __perqueue_is_full(&_events[_thread->priority]);
}


// Signaling of the events from ISRs-- can be interrupted by power failures.
// Therefore, additional state handling is required
inline void __event_signal_ISR(thread_t *thread, isr_event_t *event){
    // insert to the event queue
    _thread = thread;
    _event = *event;
    _status = EVENT_INSERT;
    __events_commit();
}

// check if there is a pending event for the given thread
inline uint8_t __has_events(thread_t *thread){
    if(__perqueue_is_empty(&_events[thread->priority]))
        return 0;

    return 1;
}

// This function is called by the tasks. Therefore, when the task is restarted,
// this function is recalled. Therefore, we need to keep track of our state.
// We first pop an event and lock it
inline isr_event_t *__lock_event(thread_t *thread){
    _popped[_thread->priority] = __perqueue_pop(&_events[_thread->priority]);
    return _popped[_thread->priority];
}

// Events should be released after lock
inline void __release_event(thread_t *thread){
    __perqueue_pop_commit(&_events[_thread->priority]);
    _popped[_thread->priority] = NULL;
}
