#include <fstream>
#include "util/server_config.h"

using namespace std;

void ServerConfig::load(const char* configFile)
{
	ifstream iss(configFile, ifstream::binary);
	Json::Value root;
	Json::Reader reader;
	if(!reader.parse(iss, root, false)){
		throw "no config file found";
	}

	string prefix = "";
	this->_travel(prefix, root);
	iss.close();
}

int ServerConfig::getIntValue(const string& key) const
{
	map<string, int>::const_iterator iter = this->_map2Int.find(key);
	if(iter == this->_map2Int.end()){
		throw "config error";
	}
	return iter->second;
}

void ServerConfig::setIntValue(const string& key, int value) 
{
	map<string, int>::iterator iter = this->_map2Int.find(key);	
	if(iter == this->_map2Int.end()){
		this->_map2Int.insert(pair<string, int>(key, value));
	} else {
		iter->second = value;
	}
}

const string& ServerConfig::getStrValue(const string& key) const
{
	map<string, string>::const_iterator iter = this->_map2Str.find(key);
	if(iter == this->_map2Str.end()){
		throw "config error";
	}
	return iter->second;
}

void ServerConfig::setStrValue(const string& key, const string& value)
{
	map<string, string>::iterator iter = this->_map2Str.find(key);
	if(iter == this->_map2Str.end()){
		this->_map2Str.insert(pair<string, string>(key, value));
	} else {
		iter->second = value;
	}
}

double ServerConfig::getDoubleValue(const string& key) const
{
	map<string, double>::const_iterator iter = this->_map2Double.find(key);
	if(iter == this->_map2Double.end()){
		throw "config error";
	}
	return iter->second;
}

void ServerConfig::setDoubleValue(const string& key, double value)
{
	map<string, double>::iterator iter = this->_map2Double.find(key);
	if(iter == this->_map2Double.end()){
		this->_map2Double.insert(pair<string, double>(key, value));
	} else {
		iter->second = value;
	}
}

bool ServerConfig::getBoolValue(const string& key) const
{
	map<string, bool>::const_iterator iter = this->_map2Bool.find(key);
	if(iter == this->_map2Bool.end()){
		throw "config error";
	}
	return iter->second;
}

void ServerConfig::setBoolValue(const string& key, bool value)
{
	map<string, bool>::iterator iter = this->_map2Bool.find(key);
	if(iter == this->_map2Bool.end()){
		this->_map2Bool.insert(pair<string, bool>(key, value));
	} else {
		iter->second = value;
	}
}

//////////////////////////////////////////////////

void ServerConfig::_travel(string prefix, Json::Value& root)
{
	if(root.size() > 0){
		for(Json::ValueIterator itr = root.begin() ; itr != root.end() ; itr++){
			string subPrefix = prefix;
			if(prefix.length() > 0)
				subPrefix += ".";
			subPrefix += itr.key().asString();
			this->_travel(subPrefix, *itr);
		}
	}
	else {
		this->_add(prefix, root);
	}
}

void ServerConfig::_add(const string& key, Json::Value val)
{
	if(val.isString()){
		this->_map2Str[key] = val.asString();
	}
	else if(val.isBool()){
		this->_map2Bool[key] = val.asBool();
	}
	else if(val.isInt()) {
		this->_map2Int[key] = val.asInt();
	}
	else if(val.isUInt()) {
		this->_map2Int[key] = val.asUInt();
	}
	else if(val.isDouble()) {
		this->_map2Double[key] = val.asDouble();
	}
}

