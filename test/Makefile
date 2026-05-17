# Makefile for Google Test project

CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -I../include/ -g
LDFLAGS := -lgtest -lgtest_main -pthread

# Source files
SRC := \
    ../src/sim.cpp \
    ../src/queue.cpp \
    ../src/types.cpp \
    ../src/util.cpp \
    ../src/csv_io.cpp

# Test source
TEST_SRC := test1.cpp

# Output binary
TARGET := test1

# Default target
all: $(TARGET)

# Build rule
$(TARGET): $(TEST_SRC) $(SRC)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

# Run tests
run: $(TARGET)
	./$(TARGET)

# Clean build artifacts
clean:
	rm -f $(TARGET)

.PHONY: all run clean
