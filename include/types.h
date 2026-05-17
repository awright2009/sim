/**
 * @file types.h
 * @brief Common type definitions and simulation data structures.
 *
 * This header defines:
 * - Enumerations for companies, vehicle states, and charger states
 * - Runtime simulation data structures
 * - Vehicle configuration data
 * - Statistics tracking structures
 * - String conversion helper functions
 */

#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum company_e { ALPHA, BRAVO, CHARLIE, DELTA, ECHO };
enum vehicle_state_e { AIRBORNE, GROUND, CHARGING };
enum charger_state_e { FREE, OCCUPIED };

char *company_e_to_str(company_e param);
char *vehicle_state_e_to_str(vehicle_state_e param);
char *charger_state_e_to_str(charger_state_e param);

company_e str_to_company_e(const char *str);
vehicle_state_e str_to_vehicle_state_e(const char *str);
charger_state_e str_to_charger_state_e(const char *str);


typedef struct
{
	company_e	name;
	float		cruise_speed; // mph
	float		battery_capacity; // kWh
	float		charge_time; // hours
	float		energy_usage_cruise; // kWh/mile
	int			passenger_count;
	float		fault_prob; // per hour
} vehicle_data_t;


typedef struct
{
	int				num_flight;
	int				num_charge_session;
	int				num_fault;

	// step based, flight time and total distance time are essentially both based on airborne steps
	int				num_step_airborne;
	int				num_step_charging;
} run_stats_t;

typedef struct
{
	float			battery_capacity;
	float			energy_consumption_per_step;
	float			energy_charge_per_step;
	int				vehicle_charger;
	vehicle_state_e	vehicle_state;
	run_stats_t		run_stats;
	bool			in_line;
} run_state_t;

typedef struct
{
	charger_state_e charger_state;
	int				charging_vehicle;
} charge_station_state_t;


typedef struct
{
	company_e name;
	float avg_time_per_flight;
	float avg_dist_per_flight;
	float avg_time_charging_per_charge_session;
	int total_faults;
	// 2 vehicles, 4 passengers 1 hour at 100mph, 2 * 4 * 1 * 100 = 800
	int passenger_miles;

	float cruise_speed;
	int num_step_airborne;
	int num_flight;
	int num_step_charging;
	int num_charge_session;
} stats_t;


#endif
