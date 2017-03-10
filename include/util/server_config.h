#ifndef _UTIL_SERVER_CONFIG_H
#define _UTIL_SERVER_CONFIG_H

#include <string>
#include <map>
#include "json/json.h"

class ServerConfig
{
public:
	void load(const char* configFile);
	int getIntValue(const std::string& key) const;
	const std::string& getStrValue(const std::string& key) const;
	double getDoubleValue(const std::string& key) const;
	bool getBoolValue(const std::string& key) const;

	void setIntValue(const std::string& key, int value);
	void setStrValue(const std::string& key, const std::string& value);
	void setDoubleValue(const std::string& key, double value);
	void setBoolValue(const std::string& key, bool value);

private:
	std::map<std::string, int> _map2Int;
	std::map<std::string, std::string> _map2Str;
	std::map<std::string, bool> _map2Bool;
	std::map<std::string, double> _map2Double;

private:
	void _travel(std::string prefix, Json::Value& root);
	void _add(const std::string& key, Json::Value val);
};
#endif
