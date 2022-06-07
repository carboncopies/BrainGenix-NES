#include<stdlib.h>
#include "objects.h"
#include<ctime>
#include "randomkit.h"

#include "code_objects/neurongroup_group_variable_set_conditional_codeobject.h"
#include "code_objects/neurongroup_group_variable_set_conditional_codeobject_1.h"
#include "code_objects/neurongroup_resetter_codeobject.h"
#include "code_objects/neurongroup_run_regularly_codeobject.h"
#include "code_objects/neurongroup_stateupdater_codeobject.h"
#include "code_objects/neurongroup_thresholder_codeobject.h"
#include "code_objects/spikemonitor_codeobject.h"


void brian_start()
{
	_init_arrays();
	_load_arrays();
	// Initialize clocks (link timestep and dt to the respective arrays)
    brian::defaultclock.timestep = brian::_array_defaultclock_timestep;
    brian::defaultclock.dt = brian::_array_defaultclock_dt;
    brian::defaultclock.t = brian::_array_defaultclock_t;
    brian::neurongroup_run_regularly_clock.timestep = brian::_array_neurongroup_run_regularly_clock_timestep;
    brian::neurongroup_run_regularly_clock.dt = brian::_array_neurongroup_run_regularly_clock_dt;
    brian::neurongroup_run_regularly_clock.t = brian::_array_neurongroup_run_regularly_clock_t;
    for (int i=0; i<1; i++)
	    rk_randomseed(brian::_mersenne_twister_states[i]);  // Note that this seed can be potentially replaced in main.cpp
}

void brian_end()
{
	_write_arrays();
	_dealloc_arrays();
}


