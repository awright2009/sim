
#ifndef UTIL_H
#define UTIL_H

#define NUM_STATS (20)
#define NUM_TYPE  (5)
#define NUM_CHARGE_STATION (3)

#define CSV_WIDTH  (7)
#define CSV_HEIGHT (6)
#define CSV_LENGTH (128)

#define NUM_HOURS_SIMULATED (3)

int read_vehicle_data(vehicle_data_t *types, char *filename);
void write_stats(stats_t *stats, int num_stat);

#endif
