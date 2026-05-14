/**
* @file types.cpp
* @brief String conversion helpers for simulation enumerations.
*/

#include "types.h"

/**
* @brief Convert a company enumeration value to a string.
*
* Returns a human-readable string representation
* of the specified company enumeration.
*
* @param param Company enumeration value.
*
* @return Pointer to constant string representation.
*/
char *company_e_to_str(company_e param)
{
	switch (param)
	{
	case ALPHA:
		return (char *)"ALPHA";
	case BRAVO:
		return (char *)"BRAVO";
	case CHARLIE:
		return (char *)"CHARLIE";
	case DELTA:
		return (char *)"DELTA";
	case ECHO:
		return (char *)"ECHO";
	default:
		return (char *)"UNKNOWN";
	}
}

/**
* @brief Convert a company name string to a company enumeration value.
*
* Searches the input string for known company identifiers and returns
* the corresponding company_e enumeration value.
*
* @param str Input string containing a company name.
*
* @return Matching company_e value, or -1 if no match is found.
*/
company_e str_to_company_e(const char *str)
{
	if (strstr(str, "ALPHA") != NULL)
	{
		return ALPHA;
	}
	else if (strstr(str, "BRAVO") != NULL)
	{
		return BRAVO;
	}
	else if (strstr(str, "CHARLIE") != NULL)
	{
		return CHARLIE;
	}
	else if (strstr(str, "DELTA") != NULL)
	{
		return DELTA;
	}
	else if (strstr(str, "ECHO") != NULL)
	{
		return ECHO;
	}

	return (company_e)-1;
}


/**
* @brief Convert a vehicle state string to a vehicle_state_e enumeration.
*
* Searches the input string for known vehicle state identifiers and
* returns the corresponding vehicle_state_e value.
*
* @param str Input string containing a vehicle state name.
*
* @return Matching vehicle_state_e value, or -1 if no match is found.
*/
vehicle_state_e str_to_vehicle_state_e(const char *str)
{
	if (strstr(str, "AIRBORNE") != NULL)
	{
		return AIRBORNE;
	}
	else if (strstr(str, "GROUND") != NULL)
	{
		return GROUND;
	}
	else if (strstr(str, "CHARGING") != NULL)
	{
		return CHARGING;
	}

	return (vehicle_state_e)-1;
}

/**
* @brief Convert a charger state string to a charger_state_e enumeration.
*
* Searches the input string for known charger state identifiers and
* returns the corresponding charger_state_e value.
*
* @param str Input string containing a charger state name.
*
* @return Matching charger_state_e value, or -1 if no match is found.
*/
charger_state_e str_to_charger_state_e(const char *str)
{
	if (strstr(str, "FREE") != NULL)
	{
		return FREE;
	}
	else if (strstr(str, "OCCUPIED") != NULL)
	{
		return OCCUPIED;
	}

	return (charger_state_e)-1;
}

/**
* @brief Convert a vehicle state enumeration value to a string.
*
* Returns a human-readable string representation
* of the specified vehicle state enumeration.
*
* @param param Vehicle state enumeration value.
*
* @return Pointer to constant string representation.
*/
char *vehicle_state_e_to_str(vehicle_state_e param)
{
	switch (param)
	{
	case AIRBORNE:
		return (char *)"AIRBORNE";
	case GROUND:
		return (char *)"GROUND";
	case CHARGING:
		return (char *)"CHARGING";
	default:
		return (char *)"UNKNOWN";
	}
}

/**
* @brief Convert a charger state enumeration value to a string.
*
* Returns a human-readable string representation
* of the specified charger state enumeration.
*
* @param param Charger state enumeration value.
*
* @return Pointer to constant string representation.
*/
char *charger_state_e_to_str(charger_state_e param)
{
	switch (param)
	{
	case FREE:
		return (char *)"FREE";
	case OCCUPIED:
		return (char *)"OCCUPIED";
	default:
		return (char *)"UNKNOWN";
	}
}
