/**
 * @file csv_io.h
 * @brief CSV file parsing and spreadsheet-style cell access utilities.
 *
 * This header provides functions for:
 * - Loading CSV files into memory
 * - Accessing CSV cells using spreadsheet-style coordinates
 * - Extracting numeric arrays from CSV data
 * - Converting between integer and base-26 column notation
 *
 * The API is designed for lightweight CSV parsing and data extraction.
 */

#ifndef CSV_IO
#define CSV_IO

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdint.h>



#define CSV_MAX_ROW 1024
#define MAX_CSV_LINE 32768

typedef struct
{
	char *line_list[CSV_MAX_ROW];
	unsigned int num_line;
	char *pfile;
} csv_handle_t;


char *get_file(const char *filename, unsigned int *size);
int OpenCSV(const char *filename, csv_handle_t *handle);
int CloseCSV(csv_handle_t *handle);
void IntToBase26(char *output, int pos);
int Base26ToInt(const char *col_char);
int GetArray(csv_handle_t *a, int rbase_a, int cbase_a, int rows, int cols, double *data);
int readCSVCell(const csv_handle_t *handle, const char *col_char, int row, char *value, int length);


#endif

