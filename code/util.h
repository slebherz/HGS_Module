#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

/*
	Returns true if the file exists, otherwise false.
*/
bool FileExists(const std::string filename);

/*
	Reads the contents of a file into a string.
*/
std::string GetFileContents(const std::string filename);

/*
	Splits a string by the specified delimeter.
*/
std::vector<std::string> &Split(const std::string &s, char delim, std::vector<std::string> &elems);

/*
	Splits a string by the specified delimeter.
*/
std::vector<std::string> Split(const std::string &s, char delim);

/*
	Trim from start of string.
*/
static inline std::string &TrimLeft(std::string &s);

/*
	Trim from end of string.
*/
static inline std::string &TrimRight(std::string &s);

/*
	Trim from both ends of string.
*/
static inline std::string &Trim(std::string &s);