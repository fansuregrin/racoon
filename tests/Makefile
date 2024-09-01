CXX = g++
CFLAGS = -std=c++14 -Wall

BIN_DIR = ../bin
TARGET = racoon_test
LIBS = -lgtest -ltinyxml2
OBJS = ./main.cc\
	   ./config_unittest.cc ../src/racoon/config/config.cc\
	   ./util_unittest.cc ../src/racoon/util/util.cc\
	   ./log_unittest.cc ../src/racoon/log/log.cc\
	   ./blkqueue_unittest.cc

all: $(OBJS)
	mkdir -p $(BIN_DIR)
	cp ../conf/racoon.xml $(BIN_DIR)
	$(CXX) $(CFLAGS) $(OBJS) -o $(BIN_DIR)/$(TARGET) $(LIBS) -pthread

clean:
	rm -rf $(BIN_DIR)/$(TARGET)