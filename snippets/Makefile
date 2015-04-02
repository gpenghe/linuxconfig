# Makefile for enics on PC Linux
# Last Updated: Jason Geng, 2015-03-20

# checking for whether ./configure has been run
CFG_MAK = $(PROJ_ROOT)/src/cfg.mak
ifeq ("$(wildcard $(CFG_MAK))" , "")
CONFIG_DONE = 0
else
CONFIG_DONE = 1
include $(CFG_MAK)
endif

# SHELL := /bin/bash

OUTPUT_FILE = $(PROJ_ROOT)/bin/enics

CXX = g++
CFLAGS += -Wall -I $(PROJ_ROOT)/src -g -I $(PROJ_ROOT)/src/include

SRC_DIR := $(PROJ_ROOT)/src
SRC_FILES:= 							\
	$(SRC_DIR)/Analytics.cpp 			\
	$(SRC_DIR)/BgCodebook.cpp 			\
	$(SRC_DIR)/drawing.cpp 				\
	$(SRC_DIR)/ics_main.cpp 			\
	$(SRC_DIR)/OptimalAssignment.cpp	\
	$(SRC_DIR)/util.cpp 				\

ifdef CFG_SEND_ICS_SERVER
SRC_FILES += $(SRC_DIR)/comm.cpp
endif

SRC_FILES_LIST = $(wildcard $(SRC_FILES))
SRC_TO_OBJ = $(addsuffix .o, $(notdir $(basename $1)))
OBJ_FILES = $(call SRC_TO_OBJ, $(SRC_FILES_LIST))

ifdef CFG_OPENCV
CFLAGS += `pkg-config opencv --cflags`
LIBS 	= `pkg-config opencv --libs` 
endif

ifdef CFG_OPENNI
CFLAGS 	+= `pkg-config --cflags libopenni`
LIBS 	+= `pkg-config --libs libopenni`
ifdef CFG_OPENNI2
LDFLAGS += -lOpenNI2 # Can be used on Pi
endif
endif

ifdef CFG_RELEASE
NDEBUG = 1
CFLAGS 		+= 	-O3
endif

ifdef CFG_USE_GPROF
CFLAGS 		+= 	-pg
LDFLAGS 	+= 	-pg
endif

ifdef CFG_USE_GOOGLE_PERFTOOLS
LIBS 	+= `pkg-config --libs libprofiler`
endif

# the following flags are copied from OpenNI build. Are they for performance reasons?
CFLAGS += -fvisibility=hidden
# CFLAGS += -mtune=arm1176jzf-s CFLAGS += -mfpu=vfp -mfloat-abi=hard # Can be used on Pi
CFLAGS += -fno-tree-pre -fno-strict-aliasing -ftree-vectorize -ffast-math -funsafe-math-optimizations  -flax-vector-conversions 

OUTPUT_COMMAND = $(CXX) -o $(OUTPUT_FILE) $(OBJ_FILES) $(LDFLAGS) $(LIBS)

# create a target for the object file
define CREATE_SRC_TARGETS 
$(call SRC_TO_OBJ, $1): $1
	$(CXX) -c $(CFLAGS) -o $$@ $$<
endef

##################################################
# Make Targets
##################################################
$(OUTPUT_FILE): sanity_check $(OBJ_FILES)
	$(OUTPUT_COMMAND)

sanity_check:
ifeq ("$(PROJ_ROOT)", "")
	@echo -ne "\033[1;31m"
	@echo  "Must run '. linux.env' first!"
	@echo -ne "\033[0m"
	@exit -1
endif
ifeq ($(CONFIG_DONE), 0)
	@echo -ne "\033[1;31m"
	@echo  "$(CFG_MAK) not found! Must run ./configure first!"
	@echo -ne "\033[0m"
	@exit -1
endif

# create targets for each source file
$(foreach src, $(SRC_FILES_LIST), $(eval $(call CREATE_SRC_TARGETS, $(src))))

clean-objs:
	-rm -rf $(OBJ_FILES)
	
clean: clean-objs 
	-rm $(OUTPUT_FILE)

distclean: clean
	-rm cfg.h cfg.mak

#-----------------------------------------------
regtest: $(OUTPUT_FILE)
	@for d in ../../testcase*; do                   \
		cd $$d;                                     \
		echo -e "Running test: $$d...\t";           \
		if ./run.sh; then                           \
			echo -e "\033[1;32;40mPassed! \033[0m"; \
		else                                        \
			echo -e "\033[1;31;40mFailed!\033[0m";  \
		fi;                                         \
		cd -;                                       \
	done
