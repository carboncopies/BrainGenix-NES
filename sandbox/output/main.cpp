#include <stdlib.h>
#include "objects.h"
#include <ctime>
#include <time.h>

#include "run.h"
#include "brianlib/common_math.h"
#include "randomkit.h"

#include "code_objects/neurongroup_group_variable_set_conditional_codeobject.h"
#include "code_objects/neurongroup_group_variable_set_conditional_codeobject_1.h"
#include "code_objects/neurongroup_resetter_codeobject.h"
#include "code_objects/neurongroup_run_regularly_codeobject.h"
#include "code_objects/neurongroup_stateupdater_codeobject.h"
#include "code_objects/neurongroup_thresholder_codeobject.h"
#include "code_objects/spikemonitor_codeobject.h"


#include <iostream>
#include <fstream>
#include <string>


        std::string _format_time(float time_in_s)
        {
            float divisors[] = {24*60*60, 60*60, 60, 1};
            char letters[] = {'d', 'h', 'm', 's'};
            float remaining = time_in_s;
            std::string text = "";
            int time_to_represent;
            for (int i =0; i < sizeof(divisors)/sizeof(float); i++)
            {
                time_to_represent = int(remaining / divisors[i]);
                remaining -= time_to_represent * divisors[i];
                if (time_to_represent > 0 || text.length())
                {
                    if(text.length() > 0)
                    {
                        text += " ";
                    }
                    text += (std::to_string(time_to_represent)+letters[i]);
                }
            }
            //less than one second
            if(text.length() == 0) 
            {
                text = "< 1s";
            }
            return text;
        }
        void report_progress(const double elapsed, const double completed, const double start, const double duration)
        {
            if (completed == 0.0)
            {
                std::cout << "Starting simulation at t=" << start << " s for duration " << duration << " s";
            } else
            {
                std::cout << completed*duration << " s (" << (int)(completed*100.) << "%) simulated in " << _format_time(elapsed);
                if (completed < 1.0)
                {
                    const int remaining = (int)((1-completed)/completed*elapsed+0.5);
                    std::cout << ", estimated " << _format_time(remaining) << " remaining.";
                }
            }

            std::cout << std::endl << std::flush;
        }
        


int main(int argc, char **argv)
{
        

	brian_start();
        

	{
		using namespace brian;

		
                
        _array_defaultclock_dt[0] = 0.0001;
        _array_defaultclock_dt[0] = 0.0001;
        _array_defaultclock_dt[0] = 0.001;
        
                        
                        for(int i=0; i<_num__array_neurongroup_v_th; i++)
                        {
                            _array_neurongroup_v_th[i] = 1;
                        }
                        
        _run_neurongroup_group_variable_set_conditional_codeobject();
        _run_neurongroup_group_variable_set_conditional_codeobject_1();
        _array_neurongroup_run_regularly_clock_dt[0] = 0.041666666666666664;
        _array_neurongroup_run_regularly_clock_dt[0] = 0.041666666666666664;
        _array_defaultclock_timestep[0] = 0;
        _array_defaultclock_t[0] = 0.0;
        _array_neurongroup_run_regularly_clock_timestep[0] = 0;
        _array_neurongroup_run_regularly_clock_t[0] = 0.0;
        magicnetwork.clear();
        magicnetwork.add(&neurongroup_run_regularly_clock, _run_neurongroup_run_regularly_codeobject);
        magicnetwork.add(&defaultclock, _run_neurongroup_stateupdater_codeobject);
        magicnetwork.add(&defaultclock, _run_neurongroup_thresholder_codeobject);
        magicnetwork.add(&defaultclock, _run_spikemonitor_codeobject);
        magicnetwork.add(&defaultclock, _run_neurongroup_resetter_codeobject);
        magicnetwork.run(11.25, report_progress, 10.0);
        #ifdef DEBUG
        _debugmsg_spikemonitor_codeobject();
        #endif

	}
        

	brian_end();
        

	return 0;
}