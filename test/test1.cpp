#include <gtest/gtest.h>

#include "../include/sim.h"
#include "../include/util.h"

// install and compile
// sudo yum install gtest gtest-devel
// g++ test1.cpp  -I../include/ ../src/sim.cpp ../src/queue.cpp ../src/types.cpp ../src/util.cpp ../src/csv_io.cpp -lgtest -lgtest_main -pthread -o test1

TEST(HelloTest, BasicAssertions)
{
	vehicle_data_t types[5] = {};
	if (read_vehicle_data(types, (char *)"../vehicle_data.csv") != 0)
	{
		printf("Error reading vehicle data from csv\r\n");
		return;
	}


	stats_t stats[NUM_STATS];
	sim simulator;

	simulator.init(types, NUM_TYPE, stats, NUM_STATS, NUM_CHARGE_STATION);
	
	// simulating each second, 1 step = 1 second (could redefine steps to an arbitrary time period)
	for (uint64_t i = 0; i < NUM_HOURS_SIMULATED * STEPS_PER_HOUR; i++)
	{
		simulator.step();
	}

	write_stats(stats, NUM_STATS);

	simulator.destroy();



	// Expect two strings not to be equal.
	EXPECT_STRNE("hello", "world");
	// Expect equality.
	EXPECT_EQ(7 * 6, 42);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


