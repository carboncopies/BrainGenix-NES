
#include "objects.h"
#include "synapses_classes.h"
#include "brianlib/clocks.h"
#include "brianlib/dynamic_array.h"
#include "brianlib/stdint_compat.h"
#include "network.h"
#include "randomkit.h"
#include<vector>
#include<iostream>
#include<fstream>

namespace brian {

std::vector< rk_state* > _mersenne_twister_states;

//////////////// networks /////////////////
Network magicnetwork;

//////////////// arrays ///////////////////
double * _array_defaultclock_dt;
const int _num__array_defaultclock_dt = 1;
double * _array_defaultclock_t;
const int _num__array_defaultclock_t = 1;
int64_t * _array_defaultclock_timestep;
const int _num__array_defaultclock_timestep = 1;
int32_t * _array_neurongroup__spikespace;
const int _num__array_neurongroup__spikespace = 380161;
int32_t * _array_neurongroup_column;
const int _num__array_neurongroup_column = 380160;
int32_t * _array_neurongroup_i;
const int _num__array_neurongroup_i = 380160;
double * _array_neurongroup_I;
const int _num__array_neurongroup_I = 380160;
int32_t * _array_neurongroup_row;
const int _num__array_neurongroup_row = 380160;
double * _array_neurongroup_run_regularly_clock_dt;
const int _num__array_neurongroup_run_regularly_clock_dt = 1;
double * _array_neurongroup_run_regularly_clock_t;
const int _num__array_neurongroup_run_regularly_clock_t = 1;
int64_t * _array_neurongroup_run_regularly_clock_timestep;
const int _num__array_neurongroup_run_regularly_clock_timestep = 1;
double * _array_neurongroup_v;
const int _num__array_neurongroup_v = 380160;
double * _array_neurongroup_v_th;
const int _num__array_neurongroup_v_th = 380160;
int32_t * _array_spikemonitor__source_idx;
const int _num__array_spikemonitor__source_idx = 380160;
int32_t * _array_spikemonitor_count;
const int _num__array_spikemonitor_count = 380160;
int32_t * _array_spikemonitor_N;
const int _num__array_spikemonitor_N = 1;

//////////////// dynamic arrays 1d /////////
std::vector<int32_t> _dynamic_array_spikemonitor_i;
std::vector<double> _dynamic_array_spikemonitor_t;

//////////////// dynamic arrays 2d /////////

/////////////// static arrays /////////////

//////////////// synapses /////////////////

//////////////// clocks ///////////////////
Clock defaultclock;  // attributes will be set in run.cpp
Clock neurongroup_run_regularly_clock;  // attributes will be set in run.cpp

// Profiling information for each code object
}

void _init_arrays()
{
	using namespace brian;

    // Arrays initialized to 0
	_array_defaultclock_dt = new double[1];
    
	for(int i=0; i<1; i++) _array_defaultclock_dt[i] = 0;

	_array_defaultclock_t = new double[1];
    
	for(int i=0; i<1; i++) _array_defaultclock_t[i] = 0;

	_array_defaultclock_timestep = new int64_t[1];
    
	for(int i=0; i<1; i++) _array_defaultclock_timestep[i] = 0;

	_array_neurongroup__spikespace = new int32_t[380161];
    
	for(int i=0; i<380161; i++) _array_neurongroup__spikespace[i] = 0;

	_array_neurongroup_column = new int32_t[380160];
    
	for(int i=0; i<380160; i++) _array_neurongroup_column[i] = 0;

	_array_neurongroup_i = new int32_t[380160];
    
	for(int i=0; i<380160; i++) _array_neurongroup_i[i] = 0;

	_array_neurongroup_I = new double[380160];
    
	for(int i=0; i<380160; i++) _array_neurongroup_I[i] = 0;

	_array_neurongroup_row = new int32_t[380160];
    
	for(int i=0; i<380160; i++) _array_neurongroup_row[i] = 0;

	_array_neurongroup_run_regularly_clock_dt = new double[1];
    
	for(int i=0; i<1; i++) _array_neurongroup_run_regularly_clock_dt[i] = 0;

	_array_neurongroup_run_regularly_clock_t = new double[1];
    
	for(int i=0; i<1; i++) _array_neurongroup_run_regularly_clock_t[i] = 0;

	_array_neurongroup_run_regularly_clock_timestep = new int64_t[1];
    
	for(int i=0; i<1; i++) _array_neurongroup_run_regularly_clock_timestep[i] = 0;

	_array_neurongroup_v = new double[380160];
    
	for(int i=0; i<380160; i++) _array_neurongroup_v[i] = 0;

	_array_neurongroup_v_th = new double[380160];
    
	for(int i=0; i<380160; i++) _array_neurongroup_v_th[i] = 0;

	_array_spikemonitor__source_idx = new int32_t[380160];
    
	for(int i=0; i<380160; i++) _array_spikemonitor__source_idx[i] = 0;

	_array_spikemonitor_count = new int32_t[380160];
    
	for(int i=0; i<380160; i++) _array_spikemonitor_count[i] = 0;

	_array_spikemonitor_N = new int32_t[1];
    
	for(int i=0; i<1; i++) _array_spikemonitor_N[i] = 0;


	// Arrays initialized to an "arange"
	_array_neurongroup_i = new int32_t[380160];
    
	for(int i=0; i<380160; i++) _array_neurongroup_i[i] = 0 + i;

	_array_spikemonitor__source_idx = new int32_t[380160];
    
	for(int i=0; i<380160; i++) _array_spikemonitor__source_idx[i] = 0 + i;


	// static arrays

	// Random number generator states
	for (int i=0; i<1; i++)
	    _mersenne_twister_states.push_back(new rk_state());
}

void _load_arrays()
{
	using namespace brian;

}

void _write_arrays()
{
	using namespace brian;

	ofstream outfile__array_defaultclock_dt;
	outfile__array_defaultclock_dt.open("results/_array_defaultclock_dt_3947968958000263025", ios::binary | ios::out);
	if(outfile__array_defaultclock_dt.is_open())
	{
		outfile__array_defaultclock_dt.write(reinterpret_cast<char*>(_array_defaultclock_dt), 1*sizeof(_array_defaultclock_dt[0]));
		outfile__array_defaultclock_dt.close();
	} else
	{
		std::cout << "Error writing output file for _array_defaultclock_dt." << endl;
	}
	ofstream outfile__array_defaultclock_t;
	outfile__array_defaultclock_t.open("results/_array_defaultclock_t_3095085171552498076", ios::binary | ios::out);
	if(outfile__array_defaultclock_t.is_open())
	{
		outfile__array_defaultclock_t.write(reinterpret_cast<char*>(_array_defaultclock_t), 1*sizeof(_array_defaultclock_t[0]));
		outfile__array_defaultclock_t.close();
	} else
	{
		std::cout << "Error writing output file for _array_defaultclock_t." << endl;
	}
	ofstream outfile__array_defaultclock_timestep;
	outfile__array_defaultclock_timestep.open("results/_array_defaultclock_timestep_-1476287742203165559", ios::binary | ios::out);
	if(outfile__array_defaultclock_timestep.is_open())
	{
		outfile__array_defaultclock_timestep.write(reinterpret_cast<char*>(_array_defaultclock_timestep), 1*sizeof(_array_defaultclock_timestep[0]));
		outfile__array_defaultclock_timestep.close();
	} else
	{
		std::cout << "Error writing output file for _array_defaultclock_timestep." << endl;
	}
	ofstream outfile__array_neurongroup__spikespace;
	outfile__array_neurongroup__spikespace.open("results/_array_neurongroup__spikespace_1135156423735319248", ios::binary | ios::out);
	if(outfile__array_neurongroup__spikespace.is_open())
	{
		outfile__array_neurongroup__spikespace.write(reinterpret_cast<char*>(_array_neurongroup__spikespace), 380161*sizeof(_array_neurongroup__spikespace[0]));
		outfile__array_neurongroup__spikespace.close();
	} else
	{
		std::cout << "Error writing output file for _array_neurongroup__spikespace." << endl;
	}
	ofstream outfile__array_neurongroup_column;
	outfile__array_neurongroup_column.open("results/_array_neurongroup_column_-358419807202320839", ios::binary | ios::out);
	if(outfile__array_neurongroup_column.is_open())
	{
		outfile__array_neurongroup_column.write(reinterpret_cast<char*>(_array_neurongroup_column), 380160*sizeof(_array_neurongroup_column[0]));
		outfile__array_neurongroup_column.close();
	} else
	{
		std::cout << "Error writing output file for _array_neurongroup_column." << endl;
	}
	ofstream outfile__array_neurongroup_i;
	outfile__array_neurongroup_i.open("results/_array_neurongroup_i_5325905484722736820", ios::binary | ios::out);
	if(outfile__array_neurongroup_i.is_open())
	{
		outfile__array_neurongroup_i.write(reinterpret_cast<char*>(_array_neurongroup_i), 380160*sizeof(_array_neurongroup_i[0]));
		outfile__array_neurongroup_i.close();
	} else
	{
		std::cout << "Error writing output file for _array_neurongroup_i." << endl;
	}
	ofstream outfile__array_neurongroup_I;
	outfile__array_neurongroup_I.open("results/_array_neurongroup_I_8524534600138785151", ios::binary | ios::out);
	if(outfile__array_neurongroup_I.is_open())
	{
		outfile__array_neurongroup_I.write(reinterpret_cast<char*>(_array_neurongroup_I), 380160*sizeof(_array_neurongroup_I[0]));
		outfile__array_neurongroup_I.close();
	} else
	{
		std::cout << "Error writing output file for _array_neurongroup_I." << endl;
	}
	ofstream outfile__array_neurongroup_row;
	outfile__array_neurongroup_row.open("results/_array_neurongroup_row_-5567881703417665508", ios::binary | ios::out);
	if(outfile__array_neurongroup_row.is_open())
	{
		outfile__array_neurongroup_row.write(reinterpret_cast<char*>(_array_neurongroup_row), 380160*sizeof(_array_neurongroup_row[0]));
		outfile__array_neurongroup_row.close();
	} else
	{
		std::cout << "Error writing output file for _array_neurongroup_row." << endl;
	}
	ofstream outfile__array_neurongroup_run_regularly_clock_dt;
	outfile__array_neurongroup_run_regularly_clock_dt.open("results/_array_neurongroup_run_regularly_clock_dt_957075398099290704", ios::binary | ios::out);
	if(outfile__array_neurongroup_run_regularly_clock_dt.is_open())
	{
		outfile__array_neurongroup_run_regularly_clock_dt.write(reinterpret_cast<char*>(_array_neurongroup_run_regularly_clock_dt), 1*sizeof(_array_neurongroup_run_regularly_clock_dt[0]));
		outfile__array_neurongroup_run_regularly_clock_dt.close();
	} else
	{
		std::cout << "Error writing output file for _array_neurongroup_run_regularly_clock_dt." << endl;
	}
	ofstream outfile__array_neurongroup_run_regularly_clock_t;
	outfile__array_neurongroup_run_regularly_clock_t.open("results/_array_neurongroup_run_regularly_clock_t_7248245446955106043", ios::binary | ios::out);
	if(outfile__array_neurongroup_run_regularly_clock_t.is_open())
	{
		outfile__array_neurongroup_run_regularly_clock_t.write(reinterpret_cast<char*>(_array_neurongroup_run_regularly_clock_t), 1*sizeof(_array_neurongroup_run_regularly_clock_t[0]));
		outfile__array_neurongroup_run_regularly_clock_t.close();
	} else
	{
		std::cout << "Error writing output file for _array_neurongroup_run_regularly_clock_t." << endl;
	}
	ofstream outfile__array_neurongroup_run_regularly_clock_timestep;
	outfile__array_neurongroup_run_regularly_clock_timestep.open("results/_array_neurongroup_run_regularly_clock_timestep_2040558698380549041", ios::binary | ios::out);
	if(outfile__array_neurongroup_run_regularly_clock_timestep.is_open())
	{
		outfile__array_neurongroup_run_regularly_clock_timestep.write(reinterpret_cast<char*>(_array_neurongroup_run_regularly_clock_timestep), 1*sizeof(_array_neurongroup_run_regularly_clock_timestep[0]));
		outfile__array_neurongroup_run_regularly_clock_timestep.close();
	} else
	{
		std::cout << "Error writing output file for _array_neurongroup_run_regularly_clock_timestep." << endl;
	}
	ofstream outfile__array_neurongroup_v;
	outfile__array_neurongroup_v.open("results/_array_neurongroup_v_5615621598797318357", ios::binary | ios::out);
	if(outfile__array_neurongroup_v.is_open())
	{
		outfile__array_neurongroup_v.write(reinterpret_cast<char*>(_array_neurongroup_v), 380160*sizeof(_array_neurongroup_v[0]));
		outfile__array_neurongroup_v.close();
	} else
	{
		std::cout << "Error writing output file for _array_neurongroup_v." << endl;
	}
	ofstream outfile__array_neurongroup_v_th;
	outfile__array_neurongroup_v_th.open("results/_array_neurongroup_v_th_-6412841852332127763", ios::binary | ios::out);
	if(outfile__array_neurongroup_v_th.is_open())
	{
		outfile__array_neurongroup_v_th.write(reinterpret_cast<char*>(_array_neurongroup_v_th), 380160*sizeof(_array_neurongroup_v_th[0]));
		outfile__array_neurongroup_v_th.close();
	} else
	{
		std::cout << "Error writing output file for _array_neurongroup_v_th." << endl;
	}
	ofstream outfile__array_spikemonitor__source_idx;
	outfile__array_spikemonitor__source_idx.open("results/_array_spikemonitor__source_idx_4988203113850141703", ios::binary | ios::out);
	if(outfile__array_spikemonitor__source_idx.is_open())
	{
		outfile__array_spikemonitor__source_idx.write(reinterpret_cast<char*>(_array_spikemonitor__source_idx), 380160*sizeof(_array_spikemonitor__source_idx[0]));
		outfile__array_spikemonitor__source_idx.close();
	} else
	{
		std::cout << "Error writing output file for _array_spikemonitor__source_idx." << endl;
	}
	ofstream outfile__array_spikemonitor_count;
	outfile__array_spikemonitor_count.open("results/_array_spikemonitor_count_-4817312754176645856", ios::binary | ios::out);
	if(outfile__array_spikemonitor_count.is_open())
	{
		outfile__array_spikemonitor_count.write(reinterpret_cast<char*>(_array_spikemonitor_count), 380160*sizeof(_array_spikemonitor_count[0]));
		outfile__array_spikemonitor_count.close();
	} else
	{
		std::cout << "Error writing output file for _array_spikemonitor_count." << endl;
	}
	ofstream outfile__array_spikemonitor_N;
	outfile__array_spikemonitor_N.open("results/_array_spikemonitor_N_6023593379563505723", ios::binary | ios::out);
	if(outfile__array_spikemonitor_N.is_open())
	{
		outfile__array_spikemonitor_N.write(reinterpret_cast<char*>(_array_spikemonitor_N), 1*sizeof(_array_spikemonitor_N[0]));
		outfile__array_spikemonitor_N.close();
	} else
	{
		std::cout << "Error writing output file for _array_spikemonitor_N." << endl;
	}

	ofstream outfile__dynamic_array_spikemonitor_i;
	outfile__dynamic_array_spikemonitor_i.open("results/_dynamic_array_spikemonitor_i_7083765326768497614", ios::binary | ios::out);
	if(outfile__dynamic_array_spikemonitor_i.is_open())
	{
        if (! _dynamic_array_spikemonitor_i.empty() )
        {
			outfile__dynamic_array_spikemonitor_i.write(reinterpret_cast<char*>(&_dynamic_array_spikemonitor_i[0]), _dynamic_array_spikemonitor_i.size()*sizeof(_dynamic_array_spikemonitor_i[0]));
		    outfile__dynamic_array_spikemonitor_i.close();
		}
	} else
	{
		std::cout << "Error writing output file for _dynamic_array_spikemonitor_i." << endl;
	}
	ofstream outfile__dynamic_array_spikemonitor_t;
	outfile__dynamic_array_spikemonitor_t.open("results/_dynamic_array_spikemonitor_t_-981115047428173460", ios::binary | ios::out);
	if(outfile__dynamic_array_spikemonitor_t.is_open())
	{
        if (! _dynamic_array_spikemonitor_t.empty() )
        {
			outfile__dynamic_array_spikemonitor_t.write(reinterpret_cast<char*>(&_dynamic_array_spikemonitor_t[0]), _dynamic_array_spikemonitor_t.size()*sizeof(_dynamic_array_spikemonitor_t[0]));
		    outfile__dynamic_array_spikemonitor_t.close();
		}
	} else
	{
		std::cout << "Error writing output file for _dynamic_array_spikemonitor_t." << endl;
	}

	// Write last run info to disk
	ofstream outfile_last_run_info;
	outfile_last_run_info.open("results/last_run_info.txt", ios::out);
	if(outfile_last_run_info.is_open())
	{
		outfile_last_run_info << (Network::_last_run_time) << " " << (Network::_last_run_completed_fraction) << std::endl;
		outfile_last_run_info.close();
	} else
	{
	    std::cout << "Error writing last run info to file." << std::endl;
	}
}

void _dealloc_arrays()
{
	using namespace brian;


	// static arrays
}

