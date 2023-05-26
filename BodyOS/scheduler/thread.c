/*
 * thread.c
 *
 *  Created on: 11 Feb 2018
 *      Author: sinanyil81
 */

#include "ink.h"
#include <stdbool.h>
// prepares the stack of the thread for the task execution
bool check_task_freshness(uint64_t currentTime, uint64_t timestamp, uint64_t timelimit);
bool check_idp_task_time_violation(uint64_t timestamp, uint64_t timelimit);
uint64_t GetTime();
static inline void __prologue(thread_t *thread)
{
    buffer_t *buffer = &thread->buffer;
    // copy original stack to the temporary stack
    __dma_word_copy(buffer->buf[buffer->idx],buffer->buf[buffer->idx ^ 1], buffer->size>>1);
}

tcb_t * find_next_task(thread_t* thread,tcb_t *current_task, unsigned int value){
    if(current_task!= thread->entry && thread->q_head == thread->q_tail){
//        cur_data_num++;
        return thread->entry;
    }
    if (0 && current_task-> max >= value && current_task-> min <= value){
//        cur_data_num++;
        return thread->entry; // everything is normal
    }else //if (current_task-> max < value || current_task-> min > value)
    { // an outlier
        int i=0;
        for(;i<current_task->dep_count;i++){
            thread->task_queue[thread->q_tail] = current_task->dependent[i];
            thread->q_tail = (thread->q_tail + 1 )%10;
        }
        tcb_t * next_task = thread->task_queue[thread->q_head];
        thread->q_head = (thread->q_head + 1 )%10;
        return next_task;
    }

    /*TODO: Also check if the system has enough energy to run the tasks or if there will be timeliness violation or not*/
}

void update_cap_energy(tcb_t *current_task){
//    E_remaining = E_remaining - current_task->energy_required;
   //TODO: implement capacitor voltage reading logic here
}
int get_charging_constant(){
   //TODO: implement get_charging_constant logic here
}
int get_task_queue_energy(thread_t *thread){
    int sum  = 0;
    int i=thread->q_head;
   for (; i!=thread->q_tail; i=(i+1)%10){// 10 is max queue size
       sum = sum + thread->task_queue[i]->energy_required;
   }
   return sum;
}

void energy_check(thread_t *thread){

//    int available_energy = check_cap_energy();// current energy value in the capacitor.
//    int required_energy= get_task_queue_energy();//sum of energy required to run all tasks in the task queue.
//    int CHARGING_CONSTANT = get_charging_constant();// for bodyOS.
//
//    if (required_energy > available_energy){
//
//        int time_required= (required_energy - available_energy)/CHARGING_CONSTANT;
//
//        if(time_required < time_to_deadline){
//            // allow the capcitor to charge
//            continue;
//        }
//        else{
//            // two options: either wait till the capacitor gets charged or
//            //reduce the number of tasks; drop a dependent task that has minimum role in inference.
//        }
//            //each dependent task is assigned with a priority when the dropping deicison is to be taken
//
//    }
//    else{
//            // do normal execution finish the dependents
//    }

    // If energy available is run the all dependent task
    //keep going
    //otherwise make the system wait

}


void adaptive_cap_charging(thread_t *thread){

}



// runs one task inside the current thread
void __tick(thread_t *thread)
{
//    energy_check(thread);

    void *buf;
    switch (thread->state)
    {
    case TASK_READY:
        // refresh thread stack
        __prologue(thread);
        // get thread buffer
        buf = thread->buffer.buf[thread->buffer._idx^1];
        // Check if it is the entry task. The entry task always
        // consumes an event in the event queue.
        volatile unsigned int value = (unsigned int)(((task_t)thread->next->function)(buf));

        uint64_t currTime;
        currTime = GetTime();
        thread->next->timestamp = currTime;
        {
            thread->next = find_next_task(thread,thread->next,value);

            if(thread->next != thread->entry){
                // Dependent Task Transition
                // Check if there is enough energy for the next task
//                if((E_remaining - thread->next->energy_required) > 0){
//                    // TODO: make the CPU sleep for as long as the sensed data is fresh.
//                    bool fresh = check_task_freshness(currTime, thread->next->timestamp,thread->next->freshness);
//                    if(!fresh){
//                        thread->state = TASK_FINISHED;
//                    }
//                }else{
//                    // Check time violation of indp. task if device waits charging
//                    bool violation = check_idp_task_time_violation(thread->entry->timestamp, thread->entry->freshness);
//                    if(!violation){
//                        //CPU_sleep(E_tot);  // CPU will sleep until fully charged//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                        thread->state = TASK_FINISHED;
//                    }else{
//                        thread->q_tail = thread->q_head; // assign tail to head of queue for running independent task
//                        thread->state = TASK_FINISHED;
//                    }
//                }

            }else{
                //Independent Task Transition

//                // Check if there is enough energy for the next task
//                if((E_remaining - thread->next->energy_required) > 0){
//                    // TODO: make the CPU sleep for as long as the sensed data is fresh.
//                    bool fresh = check_task_freshness(currTime, thread->next->timestamp,thread->next->freshness);
//                    if(!fresh){
//                        thread->state = TASK_FINISHED;
//                    }else{
//                        int targetEnergy = E_remaining + ((int)(thread->next->freshness - (currTime - thread->next->timestamp))*P_in)/1000; // charged energy until time violation
//                        //CPU_sleep(targetEnergy);/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                        thread->state = TASK_FINISHED;
//                    }
//                }else{
//                    //If there is no enough energy, sleep until fully charged
//                    //CPU_sleep(E_tot);///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                    thread->state = TASK_FINISHED;
//                }

            }
        }

//        if(!thread->next->is_dependent){
//            }
//        else
//        if(thread->next == thread->entry){
//            // pop an event since the thread most probably woke up due to
//            // an event
//            isr_event_t *event = __lock_event(thread);
//            // push event data to the entry task
//            int value = ((entry_task_t)thread->entry->function)(buf,(void *)event);
//            thread->next = find_next_task(thread->entry,value);
//            // the event should be released (deleted)
//            thread->state = TASK_RELEASE_EVENT;
//        }
//        else{
////            thread->next = (void *)(((task_t)thread->next)(buf));
//            int value = (((task_t)thread->next->function)(buf));
//            thread->next = find_next_task(thread->entry,value);
//            thread->state = TASK_FINISHED;
//            break;
//        }
    case TASK_RELEASE_EVENT:
        // release any event which is popped by the task
        __release_event(thread);
        thread->state = TASK_FINISHED;
    case TASK_FINISHED:
        //switch stack index to commit changes
        thread->buffer._idx = thread->buffer.idx ^ 1;
        thread->state = TASK_COMMIT;
    case TASK_COMMIT:
        // copy the real index from temporary index
        thread->buffer.idx = thread->buffer._idx;
        // Task execution finished. Check if the whole tasks are executed (thread finished)
        if (thread->next == NULL)
        {
            __disable_interrupt();
            // check if there are any pending events
            if(!__has_events(thread)){
                // suspend the thread if there are no pending events
                __stop_thread(thread);
            }
            else{
                // thread re-starts from the entry task
                thread->next = thread->entry;
                // ready to execute tasks again.
                thread->state = TASK_READY;
            }
            __enable_interrupt();
        }
        else{
            // ready to execute successive tasks
            thread->state = TASK_READY;
        }
    }
}





bool check_task_freshness(uint64_t currentTime, uint64_t timestamp, uint64_t timelimit){

    bool freshness = (currentTime - timestamp) < timelimit;
    //PF_flag = 0;
    //Carging_sim_start();
    return freshness;
}

bool check_idp_task_time_violation(uint64_t timestamp, uint64_t timelimit){

//    uint64_t charging_time = (int)(((float)(E_tot - E_remaining) / (float)P_in)*1000);
//    bool violaiton = ((GetTime() - timestamp) + charging_time)< timelimit;  // violation:0 => charging is safe - violation:1 => charging is not safe, skip lower priority dependent tasks

//    return violaiton;
    return 0;
}

void CPU_sleep(int energy){
//    TA0CCTL0 &= ~CCIE; // Disable execution timer
//    targetEnergy = energy;
//    PF_flag = 0;
//    Carging_sim_start(targetEnergy);
}
