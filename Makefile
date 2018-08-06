CC = gcc
CXX = g++
TARGET = SpeedTest
# TARGET_rf = $(TARGET)_rf_beenseletction
TARGET_rf = $(TARGET)_exhausting
CFLAG = -Wall

RM = rm -rf

LIBDIR += -L$(PWD)/lib
HEADER += -I$(PWD)/include -I$(shell pwd)/include
OBJ_DIR := obj
LIBS += -lMrLoopBF -lusb-1.0 -lpthread
FLAG = -DRF_STATUS

LBITS := $(shell getconf LONG_BIT)

ifeq ($(ML_ARCH),x86_64)
ML_LIBDIR = -L$(PWD)/lib/x86_64/
ML_LPATH = ../lib/x86_64/
else ifeq ($(ML_ARCH), arm)
ML_LIBDIR = -L$(PWD)/lib/armv7
ML_LPATH = ../lib/armv7/
endif

ML_LFLAGS = $(ML_LIBDIR) $(LIBS) -Wl,-rpath='$$ORIGIN:$$ORIGIN/lib:$$ORIGIN/$(ML_LPATH)'


CXX_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(addprefix obj/,$(notdir $(CXX_FILES:.cpp=.o)))

all:$(TARGET_rf)

$(TARGET_rf): $(OBJ_DIR)/$(TARGET_rf).o $(OBJ_DIR)/transmit_header.o

#ifeq ($(ML_ARCH),arm)
#	@echo "build arm"
#	@#$(CXX) -o $@ $^ $(ARM_LFLAGS)
#else
#	@echo "build x86_64"
#	@#$(CXX) -o $@ $^ $(LFLAGS)
#endif

	$(CXX) -o $@ $^ $(ML_LFLAGS)
	$(RM) $(OBJ_DIR)


obj/%.o: $(OBJDIR) src/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(HEADER) $(CFLAG) -std=c++11 $(FLAG) -c -o $@ $<

clean:
	$(RM) $(TARGET) $(OBJ_DIR) $(TARGET_rf)

install:
	cp ./$(TARGET) $(TARGET_rf) ../bin

.PHONY: all clean install
