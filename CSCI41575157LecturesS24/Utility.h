#pragma once
#include <string>

class Utility
{
public:
	static void Trim(std::string& str);
	static std::string ReadFromFile(const std::string& filePath);
};

