#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "sim.h"
#include "csv_io.h"
#include "util.h"

/**
* @brief Write simulation statistics.
*
* Generate both per-vehicle and aggregated per-company
* simulation statistics in CSV output format.
*
* Statistics include:
* - average flight time
* - average distance per flight
* - average charging duration
* - total faults
* - passenger miles
*
* @param stats Pointer to statistics array.
* @param num_stat Number of statistics entries.
*/
void write_stats(stats_t *stats, int num_stat)
{
	stats_t per_type[NUM_TYPE];

	memset(per_type, 0, sizeof(stats_t) * NUM_TYPE);

	FILE *fp = fopen("vehicle_stats.csv", "w");
	if (fp == NULL)
	{
		perror("Unable to open vehicle_stats.csv");
		return;
	}

	fprintf(fp, "Vehicle, Type, avg_time_per_flight, avg_dist_per_flight, avg_time_charging_per_charge_session, total_faults, passenger_miles\n");
	for (int i = 0; i < num_stat; i++)
	{
		int type = (int)stats[i].name;

		// CSV format
		fprintf(fp, "%d, %s,\t%f, %f, %f, %d, %d\n",
			i,
			company_e_to_str(stats[i].name),
			stats[i].avg_time_per_flight,
			stats[i].avg_dist_per_flight,
			stats[i].avg_time_charging_per_charge_session,
			stats[i].total_faults,
			stats[i].passenger_miles
		);

		// sum up data for each type
		//per_type[(int)stats[i].name].avg_time_per_flight += stats[i].avg_time_per_flight;
		//per_type[(int)stats[i].name].avg_dist_per_flight += stats[i].avg_dist_per_flight;

		per_type[type].num_step_airborne += stats[i].num_step_airborne;
		per_type[type].num_flight += stats[i].num_flight;
		per_type[type].num_step_charging += stats[i].num_step_charging;
		per_type[type].num_charge_session += stats[i].num_charge_session;

		per_type[type].avg_time_charging_per_charge_session += stats[i].avg_time_charging_per_charge_session;
		per_type[type].total_faults += stats[i].total_faults;
		per_type[type].passenger_miles += stats[i].passenger_miles;
		per_type[type].cruise_speed = stats[i].cruise_speed;
	}

	fclose(fp);


	fp = fopen("type_stats.csv", "w");
	if (fp == NULL)
	{
		perror("Unable to open type_stats.csv");
		return;
	}

	fprintf(fp, "Type, avg_time_per_flight, avg_dist_per_flight, avg_time_charging_per_charge_session, total_faults, passenger_miles\n");



	for (int i = 0; i < NUM_TYPE; i++)
	{
		float avg_time_per_flight = 0.0f;
		float avg_dist_per_flight = 0.0f;

		if (per_type[i].num_flight > 0)
		{
			avg_time_per_flight = (per_type[i].num_step_airborne / (float)per_type[i].num_flight) / (float)STEPS_PER_HOUR;
			avg_dist_per_flight = (per_type[i].num_step_airborne / (float)per_type[i].num_flight / (float)STEPS_PER_HOUR) * per_type[i].cruise_speed;
		}

		float avg_time_charging_per_charge_session = 0.0f;

		if (per_type[i].num_charge_session > 0)
		{
			avg_time_charging_per_charge_session = (per_type[i].num_step_charging / (float)per_type[i].num_charge_session) / (float)STEPS_PER_HOUR;
		}


		fprintf(fp, "%s,\t%f, %f, %f, %d, %d\n",
			company_e_to_str((company_e) i),
			avg_time_per_flight,
			avg_dist_per_flight,
			avg_time_charging_per_charge_session,
			per_type[i].total_faults,
			per_type[i].passenger_miles
		);
	}

	fclose(fp);
}


/**
* @brief Read vehicle configuration data from a CSV file.
*
* Opens the specified CSV file, reads all vehicle data entries, and
* populates an array of vehicle_data_t structures with the parsed values.
* Each row in the CSV corresponds to one vehicle definition.
*
* Expected CSV column order:
* - Column A: Company name
* - Column B: Cruise speed
* - Column C: Battery capacity
* - Column D: Charge time
* - Column E: Cruise energy usage
* - Column F: Passenger count
* - Column G: Fault probability
*
* @param types Pointer to an array of vehicle_data_t structures to populate.
* @param filename Path to the CSV file containing vehicle data.
*
* @return 0 on success, -1 on failure.
*/
int read_vehicle_data(vehicle_data_t *types, char *filename)
{
	csv_handle_t handle;
	char vehicle_data[CSV_HEIGHT][CSV_WIDTH][CSV_LENGTH] = { 0 };


	if (OpenCSV(filename, &handle) != 0)
	{
		printf("Unable to open vehicle_data.csv\r\n");
		return -1;
	}

	for (int y = 0; y < CSV_HEIGHT; y++)
	{
		for (int x = 0; x < CSV_WIDTH; x++)
		{
			char column_name[CSV_LENGTH] = { 0 };

			IntToBase26(column_name, 1 + x);

			readCSVCell(&handle, column_name, y + 1, &vehicle_data[y][x][0], CSV_LENGTH);
		}
	}

	for (int i = 1; i < CSV_HEIGHT; i++)
	{
		types[i - 1].name = str_to_company_e(vehicle_data[i][0]);
		types[i - 1].cruise_speed = (float)atof(vehicle_data[i][1]);
		types[i - 1].battery_capacity = (float)atof(vehicle_data[i][2]);
		types[i - 1].charge_time = (float)atof(vehicle_data[i][3]);
		types[i - 1].energy_usage_cruise = (float)atof(vehicle_data[i][4]);
		types[i - 1].passenger_count = atoi(vehicle_data[i][5]);
		types[i - 1].fault_prob = (float)atof(vehicle_data[i][6]);
	}

	CloseCSV(&handle);

	return 0;
}
