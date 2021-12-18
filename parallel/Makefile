#CC=g++ -g -std=c++11
#CFLAGS=-I.
#
#all: readImg.o filters.o
#
#readImg.o: readImg.cpp filters.hpp
#	$(CC) readImg.cpp  -g -o readImg.o
#
#filters.o: filters.hpp filters.cpp
#	$(CC) filters.cpp -g -o filters.o
#
#.PHONY: clean
#clean:
#	rm -f *.o


TARGET_EXEC ?= main.out

BUILD_DIR ?= ./build
CC=g++ -g -std=c++11


SRCS := $(shell find -name '*.cpp' )
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

CPPFLAGS ?= -std=c++11
LDFLAGS ?= -g

$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_BUILD) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@ $(LDFLAGS)


.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_BUILD ?= mkdir -p