#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm> 
#include <functional> 
#include <cctype>

/*
	Returns true if the file exists, otherwise false.
*/
bool FileExists(const std::string filename)
{
	std::ifstream file(filename);
	// file is 0 if file was not found.
	return (bool)file;
}

/*
	Reads the contents of a file into a string.
*/
std::string GetFileContents(const std::string filename)
{
	// Abort on nonexistent files.
	if(!FileExists(filename)) return "";

	std::ifstream stream(filename);
	// The string is constructed by iterating through the file stream.
	std::string content((std::istreambuf_iterator<char>(stream)),
		(std::istreambuf_iterator<char>()));
	return content;
}

/*
	Splits a string by the specified delimeter.
*/
std::vector<std::string> &Split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

/*
	Splits a string by the specified delimeter.
*/
std::vector<std::string> Split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    Split(s, delim, elems);
    return elems;
}

/*
	Trim from start of string.
*/
static inline std::string &TrimLeft(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), 
			std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

/*
	Trim from end of string.
*/
static inline std::string &TrimRight(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), 
			std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

/*
	Trim from both ends of string.
*/
static inline std::string &Trim(std::string &s) {
        return TrimLeft(TrimRight(s));
}