#include <gtest/gtest.h>

#include "../include/sim.h"
#include "../include/util.h"
#include "../include/csv_io.h"

#include <unistd.h>

// install and compile
// sudo yum install gtest gtest-devel
// g++ test1.cpp  -I../include/ ../src/sim.cpp ../src/queue.cpp ../src/types.cpp ../src/util.cpp ../src/csv_io.cpp -lgtest -lgtest_main -pthread -o test1

TEST(SimTest, BasicFlight)
{

	if ( chdir("./testdata1") != 0 )
	{
		perror("chdir failed");
		EXPECT_EQ(1, 0);
		return;
	}

	float num_hours_to_simulate = 1000.0f;


	stats_t stats[NUM_STATS];
	sim simulator;


        vehicle_data_t types[5] = {};
        if (read_vehicle_data(types, (char *)"vehicle_data.csv") != 0)
        {
                printf("Error reading vehicle data from csv\r\n");
		EXPECT_EQ(1, 0);
        }

	simulator.init(types, NUM_TYPE, stats, NUM_STATS, NUM_CHARGE_STATION);
	
	// simulating each second, 1 step = 1 second (could redefine steps to an arbitrary time period)
	for (uint64_t i = 0; i < num_hours_to_simulate * STEPS_PER_HOUR; i++)
	{
		simulator.step();
	}

	write_stats(stats, NUM_STATS);

	simulator.destroy();


	unsigned int file_size = 0;
	char *data = get_file("vehicle_stats.csv", &file_size);
	if (data != NULL)
	{
		int vehicle_index = -1;
		//char vehicle_type[128] = {0};
		float avg_time_per_flight = 0.0f;
		float avg_dist_per_flight = 0.0f;
		float avg_time_charging_per_charge_session = 0.0f;
		int total_faults = 0;
		int passenger_miles = 0;


		printf("data: %s\r\n", data);

		char *line = strstr(data, "0, BRAVO");
		if (line == NULL)
		{
			EXPECT_EQ(1, 0);
		}


		// 0, BRAVO,       10.110556, 1011.055542, 0.999722, 0, 363980
		int ret = sscanf(line, "%d, BRAVO, %f, %f, %f, %d, %d", &vehicle_index, &avg_time_per_flight, &avg_dist_per_flight, &avg_time_charging_per_charge_session, &total_faults, &passenger_miles); 
		if (ret != 6)
		{
			EXPECT_EQ(ret, 6);
		}
		else
		{
			EXPECT_NEAR(avg_dist_per_flight, 1011.06, 0.01);
		}
	}
	else
	{
		EXPECT_EQ(1, 0);
	}
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


