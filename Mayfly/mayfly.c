#include "mayfly.h"

#pragma PERSISTENT(task_input_channels)
task_input_channel task_input_channels;

#pragma PERSISTENT(dataPaths)
int dataPaths[MAX_PATH_NUM][MAX_TASK_NUM] = {[0 ... MAX_PATH_NUM-1][0 ... MAX_TASK_NUM-1] = 0};

#pragma PERSISTENT(mayfly_constraints)
char mayfly_constraints = 0;

int dataPathCounter;

#pragma PERSISTENT(pathIDCount)
int pathIDCount = 1;

#pragma PERSISTENT(pathLayerCounter)
int pathLayerCounter = 0;

#pragma PERSISTENT(__pathIDCount)
int __pathIDCount = 1;

#pragma PERSISTENT(__pathLayerCounter)
int __pathLayerCounter = 0;
