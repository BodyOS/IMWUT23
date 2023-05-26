
#ifndef LIBALPACA_MAYFLY_H_
#define LIBALPACA_MAYFLY_H_

#include <stddef.h>
#include <stdint.h>
#include <libmsp/mem.h>
#include <task.h>
//#include "energyHarvester.h"
#include "clock.h"
//#include "sensorDataRead.h"
#ifndef MAX_ARG
#define MAX_ARG 10
#endif

#ifndef MAX_TASK_NUM
#define MAX_TASK_NUM 20
#endif

#ifndef MAX_PATH_NUM
#define MAX_PATH_NUM 20
#endif

#define __SET 1;
#define __RESET 0;

typedef struct {
    uint64_t data;
    uint64_t time_stamp;
    char state;
}edge_var;

typedef struct {
    edge_var *input [MAX_TASK_NUM][MAX_ARG];
    uint64_t expire_time [MAX_TASK_NUM][MAX_ARG];
    uint32_t collect [MAX_TASK_NUM][MAX_ARG];
    int dataPath [MAX_TASK_NUM][MAX_ARG];
    char state[MAX_TASK_NUM][MAX_ARG];
}task_input_channel;

extern task_input_channel task_input_channels;
extern  int dataPaths[MAX_PATH_NUM][MAX_TASK_NUM];
extern int dataPathCounter;
extern task_t *task_list_count[MAX_TASK_NUM];
extern char mayfly_constraints;
#define MAYFLY_TASK_SYM_NAME(func) _task_ ## func

#define MAYFLY_TASK(idx, func)                               \
        task_list_count[idx] = & MAYFLY_TASK_SYM_NAME(func);            \

#define EDGE_CONST(TaskNum, InputIdx, Var, ExpireTime,CollectNum, dataPathID)                \
        task_input_channels.input [TaskNum][InputIdx] = &Var;                                \
        task_input_channels.expire_time [TaskNum][InputIdx] = ExpireTime;                    \
        task_input_channels.collect [TaskNum][InputIdx] = CollectNum;                        \
        task_input_channels.dataPath [TaskNum][InputIdx] = dataPathID;                       \
        task_input_channels.state [TaskNum][InputIdx] = __SET;                       \

#define DATA_FLOWS(ID, ... )                                                                                 \
        int tempDataFlow##ID [] = { __VA_ARGS__ };                                                                  \
        for(dataPathCounter = 0; dataPathCounter < (sizeof(tempDataFlow##ID)/sizeof(int)); dataPathCounter++){      \
            dataPaths[ID][dataPathCounter] = tempDataFlow##ID [dataPathCounter];                                    \
        }                                                                                                           \


#define SaveOutput(output, func, time)          \
        output.data = (uint64_t)func;           \
        output.time_stamp = time;              \
        output.state = __SET;                  \

#define GetInput(type,var, input)                   \
        var = (type) input.data;                    \

#define MAYFLY_BEGIN    if(!mayfly_constraints){

#define MAYFLY_END  mayfly_constraints = __SET;     \
        }                                           \

#endif /* LIBALPACA_MAYFLY_H_ */
