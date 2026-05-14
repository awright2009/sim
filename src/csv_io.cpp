#include "csv_io.h"
#ifdef __linux__
#include <unistd.h>
#else
#include <windows.h>
#define MAX(x,y) (x) > (y) ? (x) : (y)
#define MIN(x,y) (x) < (y) ? (x) : (y)
#endif

#define LINE_LENGTH (MAX_CSV_LINE)



/**
* @brief Read an entire file into memory.
*
* Opens the specified file in binary mode, allocates a buffer large enough
* to contain the file contents plus a null terminator, and reads the file
* into the buffer.
*
* @param filename Path to the file to read.
* @param size Optional pointer to receive the file size in bytes.
*
* @return Pointer to a null-terminated buffer containing the file contents,
*         or nullptr on failure.
*
* @note The returned buffer must be released with delete[].
*/
char *get_file(const char *filename, unsigned int *size)
{
	FILE	*file;
	char	*buffer;
	int	file_size, bytes_read;

	file = fopen(filename, "rb");
	if (file == NULL)
		return 0;
	fseek(file, 0, SEEK_END);
	file_size = ftell(file);
	fseek(file, 0, SEEK_SET);
	buffer = new char[file_size + 1];
	bytes_read = (int)fread(buffer, sizeof(char), file_size, file);
	if (bytes_read != file_size)
	{
		delete[] buffer;
		fclose(file);
		return 0;
	}
	fclose(file);
	buffer[file_size] = '\0';

	if (size != NULL)
	{
		*size = file_size;
	}

	return buffer;
}

/**
* @brief Read a text file and split it into newline-delimited strings.
*
* Loads the specified file into memory and tokenizes it using newline
* characters. Lines beginning with '//' are ignored. Carriage returns
* are removed from Windows-style line endings.
*
* @param filename Path to the text file.
* @param list Array of character pointers to receive line pointers.
* @param num Reference to receive the number of valid lines found.
* @param file Pointer to receive the allocated file buffer.
*
* @return 0 on success, -1 on failure.
*
* @note The returned file buffer must remain valid while using the line list.
*/
int newlinelist(const char *filename, char **list, unsigned int &num, char **file)
{
	if (filename == NULL || list == NULL)
	{
		printf("newlinelist given null values\n");
		return -1;
	}

	*file = get_file(filename, NULL);
	if (*file == NULL)
	{
		printf("Unable to open %s\n", filename);
		return -1;
	}

	num = 0;

	char *line = strtok(*file, "\n");
	while (line)
	{
		if (strlen(line) >= 2 && line[0] != '/' && line[1] != '/')
		{
			int last = strlen(line) - 1;
			if (line[last] == '\r')
				line[last] = '\0';
			list[num++] = line;
		}
		line = strtok(NULL, "\n");
	}
	return 0;
}

/**
* @brief Open and parse a CSV file.
*
* Reads the CSV file into memory and populates the csv_handle_t structure
* with pointers to each parsed line.
*
* @param filename Path to the CSV file.
* @param handle Pointer to the CSV handle structure to initialize.
*
* @return 0 on success, -1 on failure.
*/
int OpenCSV(const char *filename, csv_handle_t *handle)
{
	int ret = newlinelist(filename, handle->line_list, handle->num_line, &handle->pfile);

	if (ret != 0)
	{
		printf("Failed to read %s\r\n", filename);
		return -1;
	}

	return 0;
}

/**
* @brief Close a CSV handle and free associated memory.
*
* Releases the file buffer allocated during OpenCSV().
*
* @param handle Pointer to the CSV handle structure.
*
* @return 0 on success, -1 if no file buffer exists.
*/
int CloseCSV(csv_handle_t *handle)
{
	if (handle->pfile)
	{
		delete[] handle->pfile;
		return 0;
	}

	return -1;
}

/**
* @brief Convert a spreadsheet-style column label to an integer index.
*
* Converts alphabetic column labels such as "A", "Z", "AA", or "BC"
* into a 1-based integer index.
*
* @param col_char Null-terminated column label string.
*
* @return 1-based column index.
*/
int Base26ToInt(const char *col_char)
{
	int col = 0;
	int clen = strlen(col_char);
	for (int i = 0; i < clen; i++)
	{
		int num = col_char[i] - 'A' + 1; // convert A-Z to 1-26
		int power = 1;
		for (int j = 1; j < (clen - i); j++)
		{
			power *= 26;
		}
		col += num * power;
	}

	return col;
}

/**
* @brief Convert a numeric column index to spreadsheet-style notation.
*
* Converts a 1-based integer index into alphabetic column notation
* such as "A", "Z", "AA", or "BC".
*
* @param output Output buffer to receive the column string.
* @param pos 1-based column index.
*/
void IntToBase26(char *output, int pos)
{
	char set[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int i;
	int length = 0;

	int temp = pos;

	// find output string length (or else need to reverse string later)
	for (i = 0; temp; i++)
	{
		temp = (temp - 1) / 26;
		length++;
	}

	// Convert 1-N to Alphabet (base 26 number)
	for (i = 0; pos; i++)
	{
		output[length - i - 1] = set[(pos - 1) % 26];
		pos = (pos - 1) / 26;
	}
}


/**
* @brief Read a single cell value from a CSV file.
*
* Retrieves the value at the specified row and column from the parsed
* CSV data and copies it into the provided output buffer.
*
* @param handle Pointer to the CSV handle.
* @param col_char Spreadsheet-style column label (e.g. "A", "AA").
* @param row 1-based row index.
* @param value Output buffer to receive the cell contents.
* @param length Size of the output buffer in bytes.
*
* @return 0 on success, -1 on failure.
*/
int readCSVCell(const csv_handle_t *handle, const char *col_char, int row, char *value, int length)
{
	static char line[MAX_CSV_LINE] = { 0 };
	int col = Base26ToInt(col_char);


	if (row - 1 >= (int)handle->num_line)
	{
		printf("Error: Row %d exceeds num_line %d\r\n", row, handle->num_line);
		return -1;
	}

	memset(line, 0, sizeof(line));
	strncpy(line, handle->line_list[row - 1], MAX_CSV_LINE - 1);

	char *pdata = line;

#ifdef DEBUG
	printf("line = %s\r\n", line);
#endif

	memset(value, 0, length);
	char *token = strtok(pdata, ",");
	int column_index = 0;
	while (token != NULL)
	{
#ifdef DEBUG
		printf("token search = %s\r\n", token);
		printf("waiting for %d to match %d\r\n", col - 1, column_index);
#endif
		if (col - 1 == column_index)
		{
			//printf("break\r\n");
			break;
		}
		column_index++;
		token = strtok(NULL, ",");
	}

	if (token != NULL)
	{
#ifdef DEBUG
		printf("token = %s\r\n", token);
#endif
		strncpy(value, token, length - 1);
	}
	return 0;
}


/**
* @brief Compare two CSV cell values as doubles.
*
* Reads values from two CSV cells, converts them to double precision,
* and computes the absolute difference between them.
*
* @param a Pointer to the first CSV handle.
* @param col_a Column label in the first CSV.
* @param row_a Row index in the first CSV.
* @param b Pointer to the second CSV handle.
* @param col_b Column label in the second CSV.
* @param row_b Row index in the second CSV.
* @param diff Reference to receive the absolute difference.
* @param data_a Reference to receive the parsed value from CSV A.
* @param data_b Reference to receive the parsed value from CSV B.
*
* @return 0 on success, -1 on failure.
*/
int compareCSVCellDouble(const csv_handle_t *a, char *col_a, int row_a, const csv_handle_t *b, char *col_b, int row_b, double &diff, double &data_a, double &data_b)
{
	char value_a[LINE_LENGTH] = { 0 };
	char value_b[LINE_LENGTH] = { 0 };

#ifdef DEBUG
	printf("Comparing %s %d to %s %d\r\n", col_a, row_a, col_b, row_b);
#endif

	if (readCSVCell(a, col_a, row_a, value_a, LINE_LENGTH) != 0)
	{
		return -1;
	}

	if (1 != sscanf(value_a, "%lf", &data_a))
	{
		printf("Failed to scan data_a\n");
		return -1;
	}

	if (readCSVCell(b, col_b, row_b, value_b, LINE_LENGTH) != 0)
	{
		return -1;
	}

	if (1 != sscanf(value_b, "%lf", &data_b))
	{
		printf("Failed to scan data_b\n");
		return -1;
	}

#ifdef DEBUG
	printf("a = %s\n", value_a);
	printf("b = %s\n", value_b);
#endif

	if (isnan(data_a))
	{
		printf("data_a is NaN\n");
		return -1;
	}

	if (isnan(data_b))
	{
		printf("data_b is NaN\n");
		return -1;
	}


	diff = fabs(data_a - data_b);

#ifdef DEBUG
	printf("diff = %lf\n", diff);
#endif
	return 0;
}

/**
* @brief Read a rectangular block of CSV data into a numeric array.
*
* Reads a specified range of CSV cells, converts each value to double,
* and stores the results in row-major order.
*
* @param handle Pointer to the CSV handle.
* @param rbase Starting row index (1-based).
* @param cbase Starting column index (1-based).
* @param rows Number of rows to read.
* @param cols Number of columns to read.
* @param data Output array to receive the parsed values.
*
* @return 0 on success, -1 on failure.
*/
int GetArray(csv_handle_t *handle, int rbase, int cbase, int rows, int cols, double *data)
{
	int num_read = 0;
	bool exit_loop = false;

	if (rbase == 0)
	{
		printf("Rows start at one\r\n");
		return -1;
	}

	for (int i = 0; i < rows && exit_loop == false; i++)
	{
		for (int j = 0; j < cols && exit_loop == false; j++)
		{
			char value[LINE_LENGTH] = { 0 };
			char col[32] = { 0 };

			IntToBase26(col, cbase + j);

			if (readCSVCell(handle, col, rbase + i, value, LINE_LENGTH) != 0)
			{
				exit_loop = true;
				break;
			}


			sscanf(value, "%lf", &data[num_read]);
			num_read++;
		}
	}
	//printf("\t%d of %d read\n", num_read, cols * rows);
	if (num_read != cols * rows)
	{
		printf("Failed to read all rows\r\n");
		return -1;
	}

	if (exit_loop)
	{
		printf("Exited loop due to read failure\r\n");
		return -1;
	}

	return 0;
}


/**
* @brief Compare two rectangular regions of CSV data.
*
* Compares corresponding cells from two CSV files as doubles and verifies
* that the absolute difference does not exceed the specified threshold.
*
* @param a Reference to the first CSV handle.
* @param rbase_a Starting row index in CSV A (1-based).
* @param cbase_a Starting column index in CSV A (1-based).
* @param b Reference to the second CSV handle.
* @param rbase_b Starting row index in CSV B (1-based).
* @param cbase_b Starting column index in CSV B (1-based).
* @param rows Number of rows to compare.
* @param cols Number of columns to compare.
* @param threshold Maximum allowable difference.
*
* @return 0 if all values match within threshold, -1 on mismatch or error.
*/
int CompareArray(csv_handle_t &a, int rbase_a, int cbase_a, csv_handle_t &b, int rbase_b, int cbase_b, int rows, int cols, double threshold)
{
	int num_compare = 0;
	bool exit_loop = false;

	for (int i = 0; i < rows && exit_loop == false; i++)
	{
		for (int j = 0; j < cols && exit_loop == false; j++)
		{
			double diff = 10000.0;
			double data_a = 0.0;
			double data_b = 0.0f;

			char col_a[32] = { 0 };
			char col_b[32] = { 0 };

			IntToBase26(col_a, cbase_a + j);
			IntToBase26(col_b, cbase_b + j);
			if (compareCSVCellDouble(&a, col_a, rbase_a + i, &b, col_b, rbase_b + i, diff, data_a, data_b) != 0)
			{
				printf("compareCSVCellDouble failed\r\n");
				exit_loop = true;
				break;
			}

			if (diff > threshold)
			{
				printf("Difference exceeded threshold %lf\r\n", threshold);

				printf("%s %d != %s %d\r\n",
					col_a, rbase_a + i,
					col_b, rbase_b + i);
				printf("%lf != %lf\r\n", data_a, data_b);
				printf("difference %lf\r\n", diff);
				exit_loop = true;
// give time to view the message before we exit
#ifdef __linux__
				sleep(10);
#else
				Sleep(10000);
#endif
				break;
			}
			num_compare++;
		}
	}
	printf("\t%d of %d matched\n", num_compare, cols * rows);
	if (num_compare != cols * rows)
	{
		printf("Failed to compare all rows\r\n");
		return -1;
	}

	if (exit_loop)
	{
		printf("Exited loop due to mismatch\r\n");
		return -1;
	}

	return 0;
}




