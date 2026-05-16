CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -g -Iinclude

TARGET = sim

SRC_DIR = src
OBJ_DIR = obj

SRCS = $(SRC_DIR)/main.cpp \
       $(SRC_DIR)/csv_io.cpp \
       $(SRC_DIR)/queue.cpp \
       $(SRC_DIR)/sim.cpp \
       $(SRC_DIR)/util.cpp \
       $(SRC_DIR)/types.cpp

OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean
