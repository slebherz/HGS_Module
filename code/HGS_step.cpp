#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <Windows.h>
#include "util.h"
#include "HGS.h"

/*
	Writes Oasis output to files that will be processed by HGS.
*/
void WriteOasisOutput(float argument[]);
/*
	Wait until HGS has produced output that Oasis needs to solve its
	next timestep.
*/
void WaitForHGS(int period, std::string fnameHGS);
/*
	Reads HGS output into variables passed back to Oasis for further
	processing in OCL.
*/
void ReadHGSInput(float argument[], std::string fnameHGS);

/*
	This function defines the work performed with each OCL call to run
	the HGS module.
*/
extern "C" __declspec( dllexport )
    short  MODULE_STEP(float argument[])
{   
	bool isPreSolve = argument[0] == 0;
	int period = (int)argument[1];

	if(isPreSolve)
	{
		// The models run independently for the first timestep so there's no need
		// to wait for and read HGS output.
		if(period == 1) return 1;

		// The absolute period *of the previous timestep* is used to identify
		// the files written by HGS to be incorporated into the current OASIS
		// timestep -- files are written with a .[timestep#] extension.
		std::ostringstream fnameHGS;
		fnameHGS << ::runDirectory << "/HGS/" << "LRRBo.OASIS_output."
		<< std::setfill('0') << std::setw(3) << period - 1;	

		WaitForHGS(period, fnameHGS.str());

		ReadHGSInput(argument, fnameHGS.str());
	}
	else
	{
		WriteOasisOutput(argument);
	}

    return 1;
}

/*
	Writes Oasis output to files that will be processed by HGS.
*/
void WriteOasisOutput(float argument[])
{
	// Initialize the input variables from OASIS passed to the module via
	// the call(s) to run the module in OCL.
	//
	// The period is used to generate *.[period#] file extensions for the output.
	int period = (int)argument[1];
	// Deliveries (in cms, negative values) to all demands in the model.
	double delivery350 = argument[2];
	double delivery650 = argument[3];
	double delivery651 = argument[4];
	double delivery652 = argument[5];
	double delivery850 = argument[6];
	double delivery950 = argument[7];
	double delivery951 = argument[8];
	// Releases (in cms) from all reservoirs in the model.
	double flow295_290 = argument[9];
	double flow395_390 = argument[10];
	double flow995_980 = argument[11];
	// Boolean that identifies whether MDS is met for the timestep.
	bool MDS_IsMet = argument[12];

	// Prepare the file that will contain the deliveries.
	std::stringstream fnameOasisDeliveries;
	fnameOasisDeliveries << ::runDirectory << "/HGS/" << "oasis_deliveries."
		<< std::setfill('0') << std::setw(3) << period;
	std::ofstream fOasisDeliveries;
	fOasisDeliveries.open(fnameOasisDeliveries.str());
	fOasisDeliveries.precision(2);
	// Write the deliveries.
	fOasisDeliveries << "7\t\t!number of values" << std::endl;
	fOasisDeliveries << std::fixed << delivery350 << "\t\t!350 delivery rate m3/s" << std::endl;
	fOasisDeliveries << std::fixed << delivery650 << "\t\t!650 delivery rate m3/s" << std::endl;
	fOasisDeliveries << std::fixed << delivery651 << "\t\t!651 delivery rate m3/s" << std::endl;
	fOasisDeliveries << std::fixed << delivery652 << "\t\t!652 delivery rate m3/s" << std::endl;
	fOasisDeliveries << std::fixed << delivery850 << "\t\t!850 delivery rate m3/s" << std::endl;
	fOasisDeliveries << std::fixed << delivery950 << "\t\t!950 delivery rate m3/s" << std::endl;
	fOasisDeliveries << std::fixed << delivery951 << "\t\t!951 delivery rate m3/s" << std::endl;
	fOasisDeliveries.close();

	// Prepare the file that will contain the releases.
	std::stringstream fnameOasisReleases;
	fnameOasisReleases << ::runDirectory << "/HGS/" << "oasis_releases."
		<< std::setfill('0') << std::setw(3) << period;
	std::ofstream fOasisReleases;
	fOasisReleases.open(fnameOasisReleases.str());
	fOasisReleases.precision(2);
	// Write the releases.
	fOasisReleases << "3\t\t!number of values" << std::endl;
	fOasisReleases << std::fixed << flow295_290 << "\t\t!discharge rate for 295 m3/s" << std::endl;
	fOasisReleases << std::fixed << flow395_390 << "\t\t!discharge rate for 395 m3/s" << std::endl;
	fOasisReleases << std::fixed << flow995_980 << "\t\t!discharge rate for 995 m3/s" << std::endl;
	fOasisReleases.close();

	// Prepare the file that will contain the MDS flag.
	std::stringstream fnameOasisMDS;
	fnameOasisMDS << ::runDirectory << "/HGS/" << "oasis_MDS."
		<< std::setfill('0') << std::setw(3) << period;
	std::ofstream fOasisMDS;
	fOasisMDS.open(fnameOasisMDS.str());
	fOasisMDS.precision(0);
	// Write the MDS flag.
	fOasisMDS << std::fixed << MDS_IsMet << "\t\t!1: MDS satisfied, 0: MDS not satisfied" << std::endl;
	fOasisMDS.close();
}

/*
	Wait until HGS has produced output that Oasis needs to solve its
	next timestep.
*/
void WaitForHGS(int period, std::string fnameHGS)
{
	// Wait here (prevent OASIS from continuing) until:
	//	a. HGS reads the OASIS output for the current timestep.
	//	b. HGS solves for the current timestep.
	//	c. HGS writes output for the current timestep.
	while(FileExists(fnameHGS) == false)
	{
		// This Oasis function checks whether the user has hit the [X] button
		// in the upper right corner of the window. If they have then the module
		// should immediately halt execution.
		if(quit_flag(NO, NO))
		{
			mod_err_shutdown("The user has canceled the Oasis simulation.");
			return;
		}

		Sleep(250); // milliseconds
	}
}

/*
	Reads HGS output into variables passed back to Oasis for further
	processing in OCL.
*/
void ReadHGSInput(float argument[], std::string fnameHGS)
{
	// The absolute period is used to write the .[per#] extension on files
	// to be read by HGS.
	int period = (int)argument[1];

	// Initialize the variables to store data written by HGS. The data is
	// loaded onto the arguments array. Upon completion of the module
	// execution, the values of the array are assigned to OCL Udefs that are
	// specified in the OCL call(s) to run the module.
	std::fill(argument, argument + sizeof(argument), 0);
	std::ifstream file(fnameHGS);
	std::string line;
	int idx = 0;
	// Each line in the file contains data in the expected format:
	// [value]\t![description]
	// 10.26	!flow for node 100 (m^3/s)
	while(std::getline(file, line))
	{
		std::stringstream lineStream(line);
		std::string valueString;
		double value;
		// Read the value from the line ('>>' stops at the first whitespace).
		lineStream >> value;
		// Store the inflow adjustment (flow, m^3/s).
		argument[idx] = value;
		++idx;
	}
}