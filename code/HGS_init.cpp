#include <stdio.h>
#include <string.h>
#include <string>
#include "HGS.h"

// Oasis function to shutdown the module with an error message.
ERR_SHUT _stdcall* mod_err_shutdown;

// Oasis function to get/set the variable that indicates whether the user
// has decided to terminate model execution by closing the model window.
QUIT_FLAG _stdcall* quit_flag;

// Directory of the executing run.
std::string runDirectory;

/*
	MODULE_INITIALIZE. This function is called after OASIS initializes
	the module, before simulation begins. All start-up tasks for this module
	should be done from this function.
*/
extern "C" __declspec( dllexport )
    short  MODULE_INITIALIZE(char run_directory[], unsigned int dir_name_length,
		void* OASIS_funcs_n_vars[])
{
    // OASIS passed an array of OASIS functions that the module is allowed
	// to call and and OASIS variables that the module is allowed to set.
    mod_err_shutdown = (ERR_SHUT*)OASIS_funcs_n_vars[0];
	quit_flag = (QUIT_FLAG*)OASIS_funcs_n_vars[2];

	quit_flag(NO,NO);

	// Store the run directory as global state. It'll be referenced by the
	// module step method to determine where to write output.
	runDirectory = std::string(run_directory);

    return 1;
}
