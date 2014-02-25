/*
	Declarations used by module interactions with OASIS/OCL.
	
	Oasis function declarations are generally assigned in the module
	init function by retrieving the appropriate function pointers from
	init parameter OASIS_funcs_n_vars. The available functions are defined
	in the Oasis Model.exe source.
*/
#include <string>

typedef __int8 ibyte;

#define YES 1
#define NO  0

// Oasis function to shutdown the module with an error message.
typedef void (ERR_SHUT)(char[]); 
extern ERR_SHUT _stdcall* mod_err_shutdown;

// Oasis function to get/set the variable that indicates whether the user
// has decided to terminate model execution by closing the model window.
typedef ibyte (QUIT_FLAG)(ibyte, ibyte); 
extern QUIT_FLAG _stdcall* quit_flag;

// Directory of the executing run.
extern std::string runDirectory;