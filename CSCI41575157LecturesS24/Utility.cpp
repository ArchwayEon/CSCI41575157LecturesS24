#include "Utility.h"
#include <sstream>
#include <fstream>

void Utility::Trim(std::string& str)
{
	const std::string delimiters = " \f\n\r\t\v";
	str.erase(str.find_last_not_of(delimiters) + 1);
	str.erase(0, str.find_first_not_of(delimiters));
}

std::string Utility::ReadFromFile(const std::string& filePath)
{
	std::stringstream ss{};
	std::ifstream fin{};
	fin.open(filePath.c_str());
	if (fin.fail()) {
		ss << "Could not open: " << filePath << std::endl;
		return ss.str();
	}

	std::string line;
	while (!fin.eof()) {
		getline(fin, line);
		Utility::Trim(line);
		if (line != "") { // Skip blank lines
			ss << line << std::endl;
		}
	}
	fin.close();
	return ss.str();
}
