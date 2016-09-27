#pragma once

#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctype.h>

class Settings
{
public:
	static Settings& Settings::get()
	{
		static Settings instance;
		return instance;
	}

	bool getBool(std::string key)
	{
		return storedConfigBool.at(key);
	}
	float getFloat(std::string key)
	{
		return storedConfigFloat.at(key);
	}
	int getInt(std::string key)
	{
		return storedConfigInt.at(key);
	}
	std::string getString(std::string key)
	{
		return storedConfigString.at(key);
	}

	void setBool(std::string key, bool value)
	{
		storedConfigBool[key] = value;
	}
	void setFloat(std::string key, float value)
	{
		storedConfigFloat[key] = value;
	}
	void setInt(std::string key, int value)
	{
		storedConfigInt[key] = value;
	}
	void setString(std::string key, std::string value)
	{
		storedConfigString[key] = value;
	}

	static void Settings::parseFile(std::ifstream& inStream)
	{
		if (!inStream.is_open())
		{
			std::cout << "Failed to open config file" << std::endl;
			exit(1);
		}
		std::string prefix;
		for (;;)
		{
			if (!inStream) break;
			std::string line;
			std::getline(inStream, line);
			if (!line.empty())
			{
				std::istringstream iss(line);
				if (iss.peek() == '[')
				{
					//Pop off '['
					iss.get();
					//Read line up to ']'
					std::getline(iss, prefix, ']');
					//Then discard line
				}
				else
				{
					std::string key, value;
					std::getline(iss, key, '=');
					std::getline(iss, value);
					key = prefix + "." + key;
					if (isdigit(value[0]) || value[0] == '-')	//Numeric
					{
						if (value.find(".", 0) != std::string::npos)	//Treat as float
						{
							get().setFloat(key, std::stof(value));
						}
						else  //It's an int
						{
							get().setInt(key, std::stoi(value));
						}
					}
					else  //Non-numeric, i.e. alpha
					{
						if(value == "true")
						{
							get().setBool(key, true);
						} 
						else if (value == "false")
						{
							get().setBool(key, false);
						} 
						else
						{
							get().setString(key, value);
						}
						
					}
				}
			}
		}
		inStream.close();
	}
private:
	Settings::Settings() {}
	Settings(const Settings&);
	Settings& operator=(const Settings&) = default;
	std::unordered_map<std::string, bool> storedConfigBool;
	std::unordered_map<std::string, float> storedConfigFloat;
	std::unordered_map<std::string, int> storedConfigInt;
	std::unordered_map<std::string, std::string> storedConfigString;
};