#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <libwispbase/accel.h>
#include <libalpaca/alpaca.h>

#include <libmsp/watchdog.h>


// Number of samples to discard before recording training set
#define NUM_WARMUP_SAMPLES 3

#define ACCEL_WINDOW_SIZE 3
#define MODEL_SIZE 16
#define SAMPLE_NOISE_FLOOR 10 // TODO: made up value

// Number of classifications to complete in one experiment
#define SAMPLES_TO_COLLECT 128

unsigned volatile *timer = &TBCTL;
typedef threeAxis_t_8 accelReading;
typedef accelReading accelWindow[ACCEL_WINDOW_SIZE];

typedef struct {
    unsigned meanmag;
    unsigned stddevmag;
} features_t;

typedef enum {
    CLASS_STATIONARY,
    CLASS_MOVING,
} class_t;



typedef enum {
    // MODE_IDLE = (BIT(PIN_AUX_1) | BIT(PIN_AUX_2)),
    //  MODE_TRAIN_STATIONARY = BIT(PIN_AUX_1),
    //  MODE_TRAIN_MOVING = BIT(PIN_AUX_2),
    MODE_IDLE = 3,
    MODE_TRAIN_STATIONARY = 2,
    MODE_TRAIN_MOVING = 1,
    MODE_RECOGNIZE = 0, // default
} run_mode_t;



#define MEM_SIZE 0x4
__nv uint8_t *data_src[MEM_SIZE];
__nv uint8_t *data_dest[MEM_SIZE];
__nv unsigned int data_size[MEM_SIZE];
__nv uint8_t exacution_counter=0;
void clear_isDirty() {}

void init();
void task_init();
void task_selectMode();
void task_resetStats();
void task_sample();
void task_transform();
void task_featurize();
void task_classify();
void task_stats();
void task_warmup();
void task_train();
void task_idle();

TASK(1, task_init)
TASK(2, task_selectMode)
TASK(3, task_resetStats)
TASK(4, task_sample)
TASK(5, task_transform)
TASK(6, task_featurize)
TASK(7, task_classify)
TASK(8, task_stats)
TASK(9, task_warmup)
TASK(10, task_train)
TASK(11, task_idle)

GLOBAL_SB(uint16_t, pinState);
GLOBAL_SB(unsigned, discardedSamplesCount);
GLOBAL_SB(run_mode_t, class);
GLOBAL_SB(unsigned, totalCount);
GLOBAL_SB(unsigned, movingCount);
GLOBAL_SB(unsigned, stationaryCount);
GLOBAL_SB(accelReading, window, ACCEL_WINDOW_SIZE);
GLOBAL_SB(features_t, features);
GLOBAL_SB(features_t, model_stationary, MODEL_SIZE);
GLOBAL_SB(features_t, model_moving, MODEL_SIZE);
GLOBAL_SB(unsigned, trainingSetSize);
GLOBAL_SB(unsigned, samplesInWindow);
GLOBAL_SB(run_mode_t, mode);
GLOBAL_SB(unsigned, seed);
GLOBAL_SB(unsigned, count);

void ACCEL_singleSample_(threeAxis_t_8* result){
    result->x = (GV(seed)*17)%85;
    result->y = (GV(seed)*17*17)%85;
    result->z = (GV(seed)*17*17*17)%85;
    ++GV(seed);
}

static void init_hw()
{
    P3DIR = 0x03;
    msp_watchdog_disable();
    PM5CTL0 &= ~LOCKLPM5;
}


void initializeHardware()
{

//  *timer &= ~(0x0020); //set bit 5 to zero(halt!)
    threeAxis_t_8 accelID = {0};
    PF_sim_start();
    //init_hw();

    __enable_interrupt();
}

void task_init()
{
    if(exacution_counter>100){
        while(1);
    }

    //PF_sim_start();
    P3OUT ^= 0x01;
    P3OUT ^= 0x01;

    GV(pinState) = MODE_IDLE;

    GV(count) = 0;
    GV(seed) = 1;
    TRANSITION_TO(task_selectMode);
}
void task_selectMode()
{
    uint16_t pin_state=1;
    ++GV(count);

    if(GV(count) >= 3) pin_state=2;
    if(GV(count)>=5) pin_state=0;
    if (GV(count) >= 7) {
        //while(1);
        exacution_counter++;
        TRANSITION_TO(task_init);
    }
    run_mode_t mode;
    class_t class;

    // uint16_t pin_state = GPIO(PORT_AUX, IN) & (BIT(PIN_AUX_1) | BIT(PIN_AUX_2));

    // Don't re-launch training after finishing training
    if ((pin_state == MODE_TRAIN_STATIONARY ||
                pin_state == MODE_TRAIN_MOVING) &&
            pin_state == GV(pinState)) {
        pin_state = MODE_IDLE;
    } else {
        GV(pinState) = pin_state;
    }



    switch(pin_state) {
        case MODE_TRAIN_STATIONARY:
            GV(discardedSamplesCount) = 0;
            GV(mode) = MODE_TRAIN_STATIONARY;
            GV(class) = CLASS_STATIONARY;
            GV(samplesInWindow) = 0;

            TRANSITION_TO(task_warmup);
            break;

        case MODE_TRAIN_MOVING:
            GV(discardedSamplesCount) = 0;
            GV(mode) = MODE_TRAIN_MOVING;
            GV(class) = CLASS_MOVING;
            GV(samplesInWindow) = 0;

            TRANSITION_TO(task_warmup);
            break;

        case MODE_RECOGNIZE:
            GV(mode) = MODE_RECOGNIZE;

            TRANSITION_TO(task_resetStats);
            break;

        default:
            TRANSITION_TO(task_idle);
    }
}

void task_resetStats()
{
    // NOTE: could roll this into selectMode task, but no compelling reason



    // NOTE: not combined into one struct because not all code paths use both
    GV(movingCount) = 0;
    GV(stationaryCount) = 0;
    GV(totalCount) = 0;

    GV(samplesInWindow) = 0;

    TRANSITION_TO(task_sample);
}

void task_sample()
{


    accelReading sample;
    ACCEL_singleSample_(&sample);
    GV(window, _global_samplesInWindow) = sample;
    ++GV(samplesInWindow);


    if (GV(samplesInWindow) < ACCEL_WINDOW_SIZE) {
        TRANSITION_TO(task_sample);
    } else {
        GV(samplesInWindow) = 0;
        TRANSITION_TO(task_transform);
    }
}

void task_transform()
{
    unsigned i;


    accelReading *sample;
    accelReading transformedSample;

    for (i = 0; i < ACCEL_WINDOW_SIZE; i++) {
        if (GV(window, i).x < SAMPLE_NOISE_FLOOR ||
                GV(window, i).y < SAMPLE_NOISE_FLOOR ||
                GV(window, i).z < SAMPLE_NOISE_FLOOR) {

            GV(window, i).x = (GV(window, i).x > SAMPLE_NOISE_FLOOR)
                ? GV(window, i).x : 0;
            GV(window, i).y = (GV(window, i).y > SAMPLE_NOISE_FLOOR)
                ? GV(window, i).y : 0;
            GV(window, i).z = (GV(window, i).z > SAMPLE_NOISE_FLOOR)
                ? GV(window, i).z : 0;
        }
    }
    TRANSITION_TO(task_featurize);
}

void task_featurize()
{
    accelReading mean, stddev;
    mean.x = mean.y = mean.z = 0;
    stddev.x = stddev.y = stddev.z = 0;
    features_t features;



    int i;
    for (i = 0; i < ACCEL_WINDOW_SIZE; i++) {

        mean.x += GV(window, i).x;
        mean.y += GV(window, i).y;
        mean.z += GV(window, i).z;
    }
    mean.x >>= 2;
    mean.y >>= 2;
    mean.z >>= 2;


    for (i = 0; i < ACCEL_WINDOW_SIZE; i++) {
        stddev.x += GV(window, i).x > mean.x ? GV(window, i).x - mean.x
            : mean.x - GV(window, i).x;
        stddev.y += GV(window, i).y > mean.y ? GV(window, i).y - mean.y
            : mean.y - GV(window, i).y;
        stddev.z += GV(window, i).z > mean.z ? GV(window, i).z - mean.z
            : mean.z - GV(window, i).z;
    }
    stddev.x >>= 2;
    stddev.y >>= 2;
    stddev.z >>= 2;

    unsigned meanmag = mean.x*mean.x + mean.y*mean.y + mean.z*mean.z;
    unsigned stddevmag = stddev.x*stddev.x + stddev.y*stddev.y + stddev.z*stddev.z;
    //features.meanmag   = sqrt16(meanmag);
    //features.stddevmag = sqrt16(stddevmag);


    switch (GV(mode)) {
        case MODE_TRAIN_STATIONARY:
        case MODE_TRAIN_MOVING:
            GV(features) = features;
            TRANSITION_TO(task_train);
            break;
        case MODE_RECOGNIZE:
            GV(features) = features;
            TRANSITION_TO(task_classify);
            break;
        default:
            // TODO: abort
            break;
    }
}

void task_classify() {
    int move_less_error = 0;
    int stat_less_error = 0;
    int i;
    class_t class;
    long int meanmag;
    long int stddevmag;

    meanmag = GV(features).meanmag;
    stddevmag = GV(features).stddevmag;

    for (i = 0; i < MODEL_SIZE; ++i) {
        long int stat_mean_err = (GV(model_stationary, i).meanmag > meanmag)
            ? (GV(model_stationary, i).meanmag - meanmag)
            : (meanmag - GV(model_stationary, i).meanmag);

        long int stat_sd_err = (GV(model_stationary, i).stddevmag > stddevmag)
            ? (GV(model_stationary, i).stddevmag - stddevmag)
            : (stddevmag - GV(model_stationary, i).stddevmag);

        long int move_mean_err = (GV(model_moving, i).meanmag > meanmag)
            ? (GV(model_moving, i).meanmag - meanmag)
            : (meanmag - GV(model_moving, i).meanmag);

        long int move_sd_err = (GV(model_moving, i).stddevmag > stddevmag)
            ? (GV(model_moving, i).stddevmag - stddevmag)
            : (stddevmag - GV(model_moving, i).stddevmag);
        if (move_mean_err < stat_mean_err) {
            move_less_error++;
        } else {
            stat_less_error++;
        }

        if (move_sd_err < stat_sd_err) {
            move_less_error++;
        } else {
            stat_less_error++;
        }
    }

    GV(class) = (move_less_error > stat_less_error) ? CLASS_MOVING : CLASS_STATIONARY;


    TRANSITION_TO(task_stats);
}

void task_stats()
{
    unsigned movingCount = 0, stationaryCount = 0;


    ++GV(totalCount);

    switch (GV(class)) {
        case CLASS_MOVING:

            ++GV(movingCount);
            break;
        case CLASS_STATIONARY:

            ++GV(stationaryCount);
            break;
    }

    if (GV(totalCount) == SAMPLES_TO_COLLECT) {

        unsigned resultStationaryPct = GV(stationaryCount) * 100 / GV(totalCount);
        unsigned resultMovingPct = GV(movingCount) * 100 / GV(totalCount);

        unsigned sum = GV(stationaryCount) + GV(movingCount);
        TRANSITION_TO(task_idle);
    } else {
        TRANSITION_TO(task_sample);
    }
}

void task_warmup()
{
    threeAxis_t_8 sample;

    if (GV(discardedSamplesCount) < NUM_WARMUP_SAMPLES) {

        ACCEL_singleSample_(&sample);
        ++GV(discardedSamplesCount);
        TRANSITION_TO(task_warmup);
    } else {
        GV(trainingSetSize) = 0;
        TRANSITION_TO(task_sample);
    }
}

void task_train()
{
    unsigned trainingSetSize;;
    unsigned class;

    switch (GV(class)) {
        case CLASS_STATIONARY:
            GV(model_stationary, _global_trainingSetSize) = GV(features);
            break;
        case CLASS_MOVING:
            GV(model_moving, _global_trainingSetSize) = GV(features);
            break;
    }

    ++GV(trainingSetSize);

    if (GV(trainingSetSize) < MODEL_SIZE) {
        TRANSITION_TO(task_sample);
    } else {
        //        PRINTF("train: class %u done (mn %u sd %u)\r\n",
        //               class, features.meanmag, features.stddevmag);
        TRANSITION_TO(task_idle);
    }
}

void task_idle() {

    TRANSITION_TO(task_selectMode);
}

INIT_FUNC(initializeHardware)
ENTRY_TASK(task_init)
