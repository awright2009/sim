/**
 * @file sim.h
 * @brief Vehicle charging simulation engine.
 *
 * This header defines the main simulation class responsible for managing
 * vehicle state transitions, charging station allocation, queue handling,
 * and statistical tracking for a charging simulation environment.
 */

#ifndef SIM_H
#define SIM_H

#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <random>

#include "queue.h"

#define STEPS_PER_HOUR (3600)


class sim
{
public:
	sim();
	~sim();

	void init(const vehicle_data_t *types, int num_type, stats_t *stats, int num_stat, int num_charger);
	void step();
	void destroy();

private:
	void step_airborne(int vehicle);
	void step_charging(int vehicle);
	void step_ground(int vehicle);

	bool process_queue_head(unsigned int vehicle);
	void enqueue_vehicle(unsigned int vehicle);
	bool try_assign_charger(unsigned int vehicle);

	void calculate_step_data();
	void update_stats();
	void print_charging();
	bool generate_fault(float percentage);

	run_state_t *run_state;
	vehicle_data_t *vehicle_data; // 20 total vehicles, of 5 types
	int num_vehicle;


	charge_station_state_t *charge_station_state;
	int num_charge_station;

	int num_step;
	
	stats_t *stats;
	int num_stat;


	queue_t queue;

	std::mt19937 rng;

	bool initialized;
	bool debug;
};


#endif
