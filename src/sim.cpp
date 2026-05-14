/**
* @file sim.cpp
* @brief Electric vehicle charging simulation implementation.
*/

#include "sim.h"

/**
* @brief Construct a new simulation object.
*
* Initializes all simulation pointers, counters, and state variables.
* No memory allocation occurs until init() is called.
*/
sim::sim()
{
	initialized = false;

	run_state		= NULL;
	vehicle_data		= NULL;
	charge_station_state	= NULL;

	num_vehicle = 0;
	num_charge_station = 0;
	num_step = 0;

	memset(&queue, 0, sizeof(queue_t));
}


/**
* @brief Destroy the simulation object.
*
* Frees all dynamically allocated resources if the simulation
* was previously initialized.
*/
sim::~sim()
{
	if (initialized)
	{
		destroy();
	}
}

/**
* @brief Initialize the simulation.
*
* Allocates runtime memory for vehicle state, charger state,
* and copies vehicle type data into the simulation.
*
* Also initializes random number generators and calculates
* per-step runtime values.
*
* @param types Array of available vehicle type definitions.
* @param num_type Number of vehicle types in the types array.
* @param stats Pointer to statistics output array.
* @param num_stat Number of vehicle instances to simulate.
* @param num_charger Number of available charging stations.
*/
void sim::init(const vehicle_data_t *types, int num_type, stats_t *stats, int num_stat, int num_charger)
{
	// allocate const data for vehicle
	num_vehicle = num_stat;
	vehicle_data = new vehicle_data_t[num_vehicle];

	memset(vehicle_data, 0, sizeof(vehicle_data_t) * num_vehicle);

	// allocate variable data for vehicle
	run_state = new run_state_t[num_vehicle];
	memset(run_state, 0, sizeof(run_state_t) * num_vehicle);

	// allocate state for charge stations
	num_charge_station = num_charger;
	charge_station_state = new charge_station_state_t[num_charge_station];
	memset(charge_station_state, 0, sizeof(charge_station_state_t) * num_charge_station);


	// init random seed
	srand((unsigned int)time(NULL));

	// for std distribution
	rng = std::mt19937(std::random_device{}());

	// randomly populate vehicle_data from company types
	for (int i = 0; i < num_vehicle; i++)
	{
		int random_type = rand() % num_type;

		vehicle_data[i] = types[random_type];

		// set runtime battery to full charge
		run_state[i].battery_capacity = vehicle_data[i].battery_capacity;
	}


	sim::stats = stats;
	sim::num_stat = num_stat;
	

	calculate_step_data();

	initialized = true;
}


/**
* @brief Calculate per-step energy consumption and charging values.
*
* Converts hourly rates into simulation step rates
* based on STEPS_PER_HOUR.
*/
void sim::calculate_step_data()
{
	// note duplicating some work since we have vehicle types that are shared
	for (int i = 0; i < num_vehicle; i++)
	{
		// cruise * energy usage = energy usage per hour
		// divide by steps per hour (one step per second) 3600
		run_state[i].energy_consumption_per_step = (vehicle_data[i].cruise_speed * vehicle_data[i].energy_usage_cruise) / (float)STEPS_PER_HOUR;

		// charge time per hour, divide into charge time per second
		run_state[i].energy_charge_per_step = (vehicle_data[i].battery_capacity / vehicle_data[i].charge_time) / (float)STEPS_PER_HOUR;


	}
}

/**
* @brief Randomly determine whether a fault occurs.
*
* Uses a uniform random distribution to generate faults
* according to the specified probability percentage.
*
* @param percentage Fault probability from 0.0 to 100.0.
*
* @return true if a fault occurs.
* @return false otherwise.
*/
bool sim::generate_fault(float percentage)
{
	if (percentage <= 0.0)
		return false;

	if (percentage >= 100.0)
		return true;

	std::uniform_real_distribution<double> dist(0.0, 100.0);

	return dist(rng) < percentage;
}

/**
* @brief Execute one simulation step.
*
* Advances all vehicles by one simulation tick,
* updates charger states, processes faults,
* and updates statistics.
*/
void sim::step()
{
	if (!initialized)
	{
		printf("Must call init() before running step()\r\n");
		return;
	}

	num_step++;

	// for each vehicle simulated
	for (int i = 0; i < num_vehicle; i++)
	{
		vehicle_state_e start_state = run_state[i].vehicle_state;


		float step_prob = 1 - pow(1 - vehicle_data[i].fault_prob, 1.0f / STEPS_PER_HOUR);

		bool fault = generate_fault(step_prob);
		if (fault)
		{
			// assuming faults dont affect operation, mearly counting based on frequency
			run_state[i].run_stats.num_fault++;
		}

		switch (run_state[i].vehicle_state)
		{
		case AIRBORNE:
			step_airborne(i);
			break;
		case GROUND:
			step_ground(i, false);
			break;
		case CHARGING:
			step_charging(i);
			break;
		}

		// print state transitions only
		if (start_state != run_state[i].vehicle_state)
		{
			printf("step %d\r\n", num_step);
			printf("Vehicle %d type %s start_state %s end_state %s\r\n", i, company_e_to_str(vehicle_data[i].name), vehicle_state_e_to_str(start_state), vehicle_state_e_to_str(run_state[i].vehicle_state));
		}

	}

	update_stats();
}

/**
* @brief Process one airborne simulation step for a vehicle.
*
* Consumes battery energy while the vehicle is airborne.
* If the battery is depleted, the vehicle transitions
* to the GROUND state.
*
* @param vehicle Vehicle index.
*/
void sim::step_airborne(int vehicle)
{
	// we are flying, so consume a unit of charge
	run_state[vehicle].battery_capacity -= run_state[vehicle].energy_consumption_per_step;

	if (run_state[vehicle].battery_capacity <= 0)
	{
		// battery died, so change to ground state
		run_state[vehicle].battery_capacity = 0;
		run_state[vehicle].vehicle_state = GROUND;

		run_state[vehicle].run_stats.num_flight++;
	}
	else
	{
		// we are still airborne after battery usage, so increment time in air
		run_state[vehicle].run_stats.num_step_airborne++;
	}
}


/**
* @brief Process one grounded simulation step for a vehicle.
*
* Attempts to assign the vehicle to a charging station.
* If all chargers are occupied, the vehicle enters
* the charging queue.
*
* @param vehicle Vehicle index.
* @param debug Enable verbose debug printing.
*/
void sim::step_ground(int vehicle, bool debug)
{
	// line for chargers is empty, so just take the first available
	if (queue.size == 0)
	{
		int j = 0;

		// check charging stations if one is free, then take it
		for (j = 0; j < num_charge_station; j++)
		{
			if (charge_station_state[j].charger_state == FREE)
			{
				// update charger state
				charge_station_state[j].charger_state = OCCUPIED;
				charge_station_state[j].charging_vehicle = vehicle;

				// update vehicle state
				run_state[vehicle].vehicle_state = CHARGING;
				run_state[vehicle].vehicle_charger = j;

				// exit loop since we have a charger
				break;
			}
		}

		if (j == num_charge_station)
		{
			// queue is really for string data, should really convert char to integer, but 0-255 is enough for our case
			unsigned char data = vehicle;

			// all chargers are full, join queue and set in_line bool
			if (debug)
			{
				printf("\tVehicle %d entering charger queue\r\n", vehicle);
			}
			enqueue(&queue, &data, 1);

			run_state[vehicle].in_line = true;

			if (debug)
			{
				print_queue(&queue);
				print_charging();
			}

		}

	}
	else
	{
		// we have a line, check to see if we are at the head of the queue
		unsigned char data;
		dequeue_peek(&queue, &data, 1);

		if ((int)data == vehicle)
		{
			// we are first in line, check for a free station
			for (int j = 0; j < num_charge_station; j++)
			{
				if (charge_station_state[j].charger_state == FREE)
				{
					// update charger state
					charge_station_state[j].charger_state = OCCUPIED;
					charge_station_state[j].charging_vehicle = vehicle;

					// update vehicle state
					run_state[vehicle].vehicle_state = CHARGING;
					run_state[vehicle].vehicle_charger = j;


					// Update Queue and in_line bool
					if (debug)
					{
						print_queue(&queue);
						print_charging();
					}

					// now remove ourselves from the queue for real this time
					dequeue(&queue, &data, 1);

					if (vehicle != (int)data)
					{
						printf("Error: peek didn't match dequeue\r\n");
						exit(0);
					}

					if (debug)
					{
						printf("\tVehicle %d exiting queue\r\n", data);
					}
					run_state[vehicle].in_line = false;

					if (debug)
					{
						print_queue(&queue);
						print_charging();
					}

					// exit loop since we have a charger
					break;
				}
			}
		}
		else
		{
			// we are on the ground
			// there is a line already and we are not first in line
			// so that means chargers are full and we can't take one yet
			// But we need to join the line (if we haven't already)

			if (run_state[vehicle].in_line == false)
			{
				run_state[vehicle].in_line = true;

				// queue is really for string data, should really convert char to integer, but 0-255 is enough for our case
				unsigned char data = vehicle;

				// all chargers are full, join queue
				if (debug)
				{
					printf("Vehicle %d entering charger queue\r\n", vehicle);
				}
				enqueue(&queue, &data, 1);

				if (debug)
				{
					print_queue(&queue);
					print_charging();
				}
			}

		}
	}
}

/**
* @brief Process one charging simulation step for a vehicle.
*
* Charges the vehicle battery incrementally until full.
* Once fully charged, the vehicle transitions back
* to the AIRBORNE state.
*
* @param vehicle Vehicle index.
*/
void sim::step_charging(int vehicle)
{
	// we are charging, so add a unit of charge
	run_state[vehicle].battery_capacity += run_state[vehicle].energy_charge_per_step;

	// check if we are fully charged
	if (run_state[vehicle].battery_capacity >= vehicle_data[vehicle].battery_capacity)
	{
		// battery charged, so change to airborne state
		run_state[vehicle].battery_capacity = vehicle_data[vehicle].battery_capacity;
		charge_station_state[run_state[vehicle].vehicle_charger].charger_state = FREE;
		charge_station_state[run_state[vehicle].vehicle_charger].charging_vehicle = -1;

		run_state[vehicle].vehicle_charger = -1;
		run_state[vehicle].vehicle_state = AIRBORNE;
		run_state[vehicle].run_stats.num_charge_session++;
	}
	else
	{
		run_state[vehicle].run_stats.num_step_charging++;
	}
}


/**
* @brief Print current charging station information.
*
* Displays charger occupancy and charging progress
* for all charging stations.
*/
void sim::print_charging()
{
	for (int j = 0; j < num_charge_station; j++)
	{

		if (charge_station_state[j].charging_vehicle != -1)
		{
			float current = run_state[charge_station_state[j].charging_vehicle].battery_capacity;
			float full = vehicle_data[charge_station_state[j].charging_vehicle].battery_capacity;

			printf("\tStation %d %s charging %d type %s\t(%f of %f)\r\n",
				j,
				charger_state_e_to_str(charge_station_state[j].charger_state),
				charge_station_state[j].charging_vehicle,
				company_e_to_str(vehicle_data[charge_station_state[j].charging_vehicle].name),
				current,
				full
			);
		}
		else
		{
			printf("\tStation %d %s\r\n",
				j,
				charger_state_e_to_str(charge_station_state[j].charger_state)
			);
		}
	}
}

/**
* @brief Update simulation statistics.
*
* Calculates aggregate metrics including:
* - average flight time
* - average charging duration
* - passenger miles
* - total faults
*/
void sim::update_stats()
{
	for (int i = 0; i < num_vehicle; i++)
	{
		stats[i].name = vehicle_data[i].name;
		stats[i].cruise_speed = vehicle_data[i].cruise_speed;

		if (run_state[i].run_stats.num_flight > 0)
		{
			stats[i].avg_time_per_flight = (run_state[i].run_stats.num_step_airborne / run_state[i].run_stats.num_flight) / (float)STEPS_PER_HOUR;
			stats[i].num_step_airborne = run_state[i].run_stats.num_step_airborne;
			stats[i].num_flight = run_state[i].run_stats.num_flight;
		}
		else
		{
			// assuming one flight even though we havent finished
			stats[i].avg_time_per_flight = (run_state[i].run_stats.num_step_airborne) / (float)STEPS_PER_HOUR;
			stats[i].num_step_airborne = run_state[i].run_stats.num_step_airborne;
			stats[i].num_flight = 1;
		}

		if (run_state[i].run_stats.num_charge_session > 0)
		{
			stats[i].avg_time_charging_per_charge_session = (run_state[i].run_stats.num_step_charging / run_state[i].run_stats.num_charge_session) / (float)STEPS_PER_HOUR;

			stats[i].num_step_charging = run_state[i].run_stats.num_step_charging;
			stats[i].num_charge_session = run_state[i].run_stats.num_charge_session;
		}
		else
		{
			// assuming one charge session even though we havent finished
			stats[i].avg_time_charging_per_charge_session = (run_state[i].run_stats.num_step_charging) / (float)STEPS_PER_HOUR;
			stats[i].num_step_charging = run_state[i].run_stats.num_step_charging;
			stats[i].num_charge_session = 1;
		}


		stats[i].avg_dist_per_flight					= stats[i].avg_time_per_flight * vehicle_data[i].cruise_speed;
		stats[i].passenger_miles = (int) ( vehicle_data[i].passenger_count * (run_state[i].run_stats.num_step_airborne / (float)STEPS_PER_HOUR) * vehicle_data[i].cruise_speed );
		stats[i].total_faults = run_state[i].run_stats.num_fault;
	}

}

/**
* @brief Destroy simulation resources.
*
* Frees all dynamically allocated memory associated
* with vehicles and charging stations.
*/
void sim::destroy()
{
	if (initialized)
	{
		// deallocate const data for vehicle
		if (vehicle_data)
		{
			delete[] vehicle_data;
			vehicle_data = NULL;
			num_vehicle = 0;
		}

		// deallocate run data for vehicle
		if (run_state)
		{
			delete[] run_state;
			run_state = NULL;
		}

		// deallocate state for charge stations
		if (charge_station_state)
		{
			delete[] charge_station_state;
			charge_station_state = NULL;
			num_charge_station = 0;
		}

		initialized = false;
	}
}
