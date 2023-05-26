#include <stdio.h>

#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <libalpaca/alpaca.h>

#include <libmsp/watchdog.h>

//unsigned count = 0;

#define NUM_INSERTS (NUM_BUCKETS / 4) // shoot for 25% occupancy
#define NUM_LOOKUPS NUM_INSERTS
#define NUM_BUCKETS 128 // must be a power of 2
#define MAX_RELOCATIONS 8
#define BUFFER_SIZE 32

typedef uint16_t value_t;
typedef uint16_t hash_t;
typedef uint16_t fingerprint_t;
typedef uint16_t index_t; // bucket index

typedef struct _insert_count {
	unsigned insert_count;
	unsigned inserted_count;
} insert_count_t;

typedef struct _lookup_count {
	unsigned lookup_count;
	unsigned member_count;
} lookup_count_t;

#define MEM_SIZE 0x4
__nv uint8_t *data_src[MEM_SIZE];
__nv uint8_t *data_dest[MEM_SIZE];
__nv unsigned int data_size[MEM_SIZE];
__nv uint16_t volatile exacution_counter=0;
void clear_isDirty() {}


void init();
void task_init();
void task_generate_key();
void task_insert();
void task_calc_indexes();
void task_calc_indexes_index_1();
void task_calc_indexes_index_2();
void task_relocate();
void task_insert_done();
void task_add();
void task_lookup();
void task_lookup_search();
void task_lookup_done();
void task_print_stats();
void task_done();
void task_init_array();


TASK(1,  task_init)
TASK(2,  task_generate_key)
TASK(3,  task_insert)
TASK(4,  task_calc_indexes)
TASK(5,  task_calc_indexes_index_1)
TASK(6,  task_calc_indexes_index_2)
TASK(7,  task_add) // TODO: rename: add 'insert' prefix
TASK(8,  task_relocate)
TASK(9,  task_insert_done)
TASK(10, task_lookup)
TASK(11, task_lookup_search)
TASK(12, task_lookup_done)
TASK(13, task_print_stats)
TASK(14, task_done)
TASK(15, task_init_array)

GLOBAL_SB(fingerprint_t, filter, NUM_BUCKETS);
GLOBAL_SB(index_t, index);
GLOBAL_SB(value_t, key);
GLOBAL_SB(task_t*, next_task);
GLOBAL_SB(fingerprint_t, fingerprint);
GLOBAL_SB(value_t, index1);
GLOBAL_SB(value_t, index2);
GLOBAL_SB(value_t, relocation_count);
GLOBAL_SB(value_t, insert_count);
GLOBAL_SB(value_t, inserted_count);
GLOBAL_SB(value_t, lookup_count);
GLOBAL_SB(value_t, member_count);
GLOBAL_SB(bool, success);
GLOBAL_SB(bool, member);

static value_t init_key = 0x0001; // seeds the pseudo-random sequence of keys

static hash_t djb_hash(uint8_t* data, unsigned len)
{
	uint16_t hash = 5381;
	unsigned int i;

	for(i = 0; i < len; data++, i++)
		hash = ((hash << 5) + hash) + (*data);


	return hash & 0xFFFF;
}

static index_t hash_to_index(fingerprint_t fp)
{
	hash_t hash = djb_hash((uint8_t *)&fp, sizeof(fingerprint_t));
	return hash & (NUM_BUCKETS - 1); // NUM_BUCKETS must be power of 2
}

static fingerprint_t hash_to_fingerprint(value_t key)
{
	return djb_hash((uint8_t *)&key, sizeof(value_t));
}

void task_init()
{
    if(exacution_counter>1000){
        while(1);
    }
	unsigned i;
	for (i = 0; i < NUM_BUCKETS ; ++i) {
		GV(filter, i) = 0;
	}
	GV(insert_count) = 0;
	GV(lookup_count) = 0;
	GV(inserted_count) = 0;
	GV(member_count) = 0;
	GV(key) = init_key;
	GV(next_task) = TASK_REF(task_insert);

	TRANSITION_TO(task_generate_key);
}
void task_init_array() {
;
	unsigned i;
	for (i = 0; i < BUFFER_SIZE - 1; ++i) {
		GV(filter, i + _global_index*(BUFFER_SIZE-1)) = 0;
	}
	++GV(index);
	if (GV(index) == NUM_BUCKETS/(BUFFER_SIZE-1)) {
		TRANSITION_TO(task_generate_key);
	}
	else {
		TRANSITION_TO(task_init_array);
	}
}
void task_generate_key()
{


	// insert pseufo-random integers, for testing
	// If we use consecutive ints, they hash to consecutive DJB hashes...
	// NOTE: we are not using rand(), to have the sequence available to verify
	// that that are no false negatives (and avoid having to save the values).
	GV(key) = (GV(key) + 1) * 17;

	transition_to(GV(next_task));
}

void task_calc_indexes()
{
	GV(fingerprint) = hash_to_fingerprint(GV(key));

	TRANSITION_TO(task_calc_indexes_index_1);
}

void task_calc_indexes_index_1()
{
	GV(index1) = hash_to_index(GV(key));
	TRANSITION_TO(task_calc_indexes_index_2);
}

void task_calc_indexes_index_2()
{
	index_t fp_hash = hash_to_index(GV(fingerprint));
	GV(index2) = GV(index1) ^ fp_hash;

	transition_to(GV(next_task));
}

// This task is redundant.
// Alpaca never needs this but since Chain code had it, leaving it for fair comparison.
void task_insert()
{
	GV(next_task) = TASK_REF(task_add);
	TRANSITION_TO(task_calc_indexes);
}

void task_add()
{
	// Fingerprint being inserted


	// index1,fp1 and index2,fp2 are the two alternative buckets


	if (!GV(filter, _global_index1)) {

		GV(success) = true;
		GV(filter, _global_index1) = GV(fingerprint);
		TRANSITION_TO(task_insert_done);
	} else {
		if (!GV(filter, _global_index2)) {

			GV(success) = true;
			GV(filter, _global_index2) = GV(fingerprint);
			TRANSITION_TO(task_insert_done);
		} else { // evict one of the two entries
			fingerprint_t fp_victim;
			index_t index_victim;

			if (rand() % 2) {
				index_victim = GV(index1);
				fp_victim = GV(filter, _global_index1);
			} else {
				index_victim = GV(index2);
				fp_victim = GV(filter, _global_index2);
			}


			// Evict the victim
			GV(filter, index_victim) = GV(fingerprint);
			GV(index1) = index_victim;
			GV(fingerprint) = fp_victim;
			GV(relocation_count) = 0;

			TRANSITION_TO(task_relocate);
		}
	}
}

void task_relocate()
{
	fingerprint_t fp_victim = GV(fingerprint);
	index_t fp_hash_victim = hash_to_index(fp_victim);
	index_t index2_victim = GV(index1) ^ fp_hash_victim;




	if (!GV(filter, index2_victim)) { // slot was free
		GV(success) = true;
		GV(filter, index2_victim) = fp_victim;
		TRANSITION_TO(task_insert_done);
	} else { // slot was occupied, rellocate the next victim


		if (GV(relocation_count) >= MAX_RELOCATIONS) { // insert failed

			GV(success) = false;
			TRANSITION_TO(task_insert_done);
		}

		++GV(relocation_count);
		GV(index1) = index2_victim;
		GV(fingerprint) = GV(filter, index2_victim);
		GV(filter, index2_victim) = fp_victim;
		TRANSITION_TO(task_relocate);
	}
}

void task_insert_done()
{
#if VERBOSE > 0
	unsigned i;


	for (i = 0; i < NUM_BUCKETS; ++i) {

		if (i > 0 && (i + 1) % 8 == 0)
	}
#endif

	++GV(insert_count);
	GV(inserted_count) += GV(success);


	if (GV(insert_count) < NUM_INSERTS) {
		GV(next_task) = TASK_REF(task_insert);
		TRANSITION_TO(task_generate_key);
	} else {
		GV(next_task) = TASK_REF(task_lookup);
		GV(key) = init_key;
		TRANSITION_TO(task_generate_key);
	}
}

void task_lookup()
{
	GV(next_task) = TASK_REF(task_lookup_search);
	TRANSITION_TO(task_calc_indexes);
}

void task_lookup_search()
{

	if (GV(filter, _global_index1) == GV(fingerprint)) {
		GV(member) = true;
	} else {


		if (GV(filter, _global_index2) == GV(fingerprint)) {
			GV(member) = true;
		}
		else {
			GV(member) = false;
		}
	}


	if (!GV(member)) {

	}

	TRANSITION_TO(task_lookup_done);
}

void task_lookup_done()
{
	++GV(lookup_count);

	GV(member_count) += GV(member);


	if (GV(lookup_count) < NUM_LOOKUPS) {
		GV(next_task) = TASK_REF(task_lookup);
		TRANSITION_TO(task_generate_key);
	} else {
		TRANSITION_TO(task_print_stats);
	}
}

void task_print_stats()
{

	TRANSITION_TO(task_done);
}

void task_done()
{
    P3OUT ^= 0x01;
    P3OUT ^= 0x01;
    exacution_counter++;
    TRANSITION_TO(task_init);
//	count++;
//	if(count == 5){
//		count = 0;
//		exit(0);
//	}
//	TRANSITION_TO(task_init);
}
static void init_hw()
{
    P3DIR = 0xFF;
	msp_watchdog_disable();
	PM5CTL0 &= ~LOCKLPM5;
    P3OUT ^=0x04;
    P3OUT ^=0x04;
	//msp_gpio_unlock();
	//msp_clock_setup();
}

void init()
{
	// set timer for measuring time

	//	*timer &= ~(0x0020); //set bit 5 to zero(halt!)
	init_hw();
    PF_sim_start();

#ifdef CONFIG_EDB
	edb_init();
#endif

	//INIT_CONSOLE();

	__enable_interrupt();


}

	ENTRY_TASK(task_init)
INIT_FUNC(init)
