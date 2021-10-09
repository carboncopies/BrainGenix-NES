PROGRAM = main

SRCS = brianlib/randomkit/randomkit.c code_objects/neurongroup_group_variable_set_conditional_codeobject.cpp code_objects/neurongroup_group_variable_set_conditional_codeobject_1.cpp code_objects/neurongroup_resetter_codeobject.cpp code_objects/neurongroup_run_regularly_codeobject.cpp code_objects/neurongroup_stateupdater_codeobject.cpp code_objects/neurongroup_thresholder_codeobject.cpp code_objects/spikemonitor_codeobject.cpp main.cpp network.cpp objects.cpp run.cpp synapses_classes.cpp
H_SRCS = brianlib/clocks.h brianlib/common_math.h brianlib/dynamic_array.h code_objects/neurongroup_group_variable_set_conditional_codeobject.h code_objects/neurongroup_group_variable_set_conditional_codeobject_1.h code_objects/neurongroup_resetter_codeobject.h code_objects/neurongroup_run_regularly_codeobject.h code_objects/neurongroup_stateupdater_codeobject.h code_objects/neurongroup_thresholder_codeobject.h code_objects/spikemonitor_codeobject.h network.h objects.h run.h synapses_classes.h
OBJS = ${SRCS:.cpp=.o}
OBJS := ${OBJS:.c=.o}
CC = @g++
OPTIMISATIONS = -DVIDEO_WIDTH=720 -DVIDEO_HEIGHT=528 -Ibrianlib/randomkit -I/usr/include -w -O3 -ffast-math -fno-finite-math-only -march=native -std=c++11
CFLAGS = -c -Wno-write-strings $(OPTIMISATIONS) -I.  
LFLAGS =  -Lbrianlib/randomkit -L/usr/lib -Wl,--enable-new-dtags,-R/usr/lib -lopencv_core -lopencv_highgui -lopencv_videoio 
DEPS = make.deps

all: $(PROGRAM)

.PHONY: all clean

$(PROGRAM): $(OBJS) $(DEPS) makefile
	$(CC) $(OBJS) -o $(PROGRAM) $(LFLAGS)

clean:
	rm $(OBJS) $(PROGRAM) $(DEPS)

make.deps: $(SRCS) $(H_SRCS)
	$(CC) $(CFLAGS) -MM $(SRCS) > make.deps
	
ifneq ($(wildcard $(DEPS)), )
include $(DEPS)
endif

%.o : %.cpp makefile
	$(CC) $(CFLAGS) $< -o $@