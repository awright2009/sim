/**
* @file main.cpp
* @brief Entry point for the electric vehicle charging simulation.
*
* Simulates multiple vehicle types competing for a limited
* number of charging stations over a fixed simulation duration.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "sim.h"
#include "csv_io.h"
#include "util.h"

//=============================================================================
// Notes from eVtol Simulation Problem - AnyTimeDomain V2.docx:
//=============================================================================

// 3 hours of sim time
// 20 vehicles of 5 types
// random selection of types to fill the 20
// Three chargers for all 20 vehicles
// charger charges in the time to charge time listed per vehicle (hours)

// assume each vehicle starts with full battery
// each vehicle instantaneously reaches cruise
// each vehicle airborne for full use of the battery
// each vehicle immediately in line for the charger after running out of battery


/**
* @brief Program entry point.
*
* Initializes vehicle type data, creates the simulator,
* runs the simulation for the configured duration,
* prints simulation statistics, and releases resources.
*
* @param argc Number of command-line arguments.
* @param argv Array of command-line argument strings.
*
* @return int Exit status code.
* @retval 0 Program completed successfully.
*/
int main(int argc, char *argv[])
{
#if 0
	const vehicle_data_t types_old[5] =
	{
		{ ALPHA,   120, 320, 0.60f, 1.6f, 4, 0.25f },
		{ BRAVO,   100, 100, 0.20f, 1.5f, 5, 0.10f },
		{ CHARLIE, 160, 220, 0.80f, 2.2f, 3, 0.05f },
		{ DELTA,    90, 120, 0.62f, 0.8f, 2, 0.22f },
		{ ECHO,     30, 150, 0.30f, 5.8f, 2, 0.61f },
	};
#endif

	if (argc < 2)
	{
		printf("Usage: %s <num_hours_to_simulate>\r\n", argv[0]);
		return 0;
	}

	float num_hours_to_simulate = atof(argv[1]);
	if (num_hours_to_simulate < 0.0f)
	{
		printf("Invalid input, negative time\r\n");
		return -1;
	}

	if (num_hours_to_simulate > 1000.0f)
	{
		printf("Hours to simulate must be within [0, 1000]\r\n");
		return -1;
	}


	vehicle_data_t types[5] = {};
	if (read_vehicle_data(types, (char *)"vehicle_data.csv") != 0)
	{
		printf("Error reading vehicle data from csv\r\n");
		return -1;
	}


	stats_t stats[NUM_STATS];
	sim simulator;

	simulator.init(types, NUM_TYPE, stats, NUM_STATS, NUM_CHARGE_STATION);
	
	// simulating each second, 1 step = 1 second (could redefine steps to an arbitrary time period)
	for (uint64_t i = 0; i < num_hours_to_simulate * STEPS_PER_HOUR; i++)
	{
		simulator.step();
	}

	write_stats(stats, NUM_STATS);

	simulator.destroy();

	return 0;
}
