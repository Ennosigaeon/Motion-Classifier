#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <map>
#include <string>

class Properties {
private:
	std::map<std::string, std::string> values;

	const std::string& find(std::string& key) const;

public:
	Properties();

	Properties(std::string path);
	
	void load(std::string path);
	
	void set(std::string key, std::string value);

	void store(std::string path);

	std::string get(std::string key) const;

	int getInt(std::string key) const;

	long getLong(std::string key) const;

	double getDouble(std::string key) const;

	bool getBool(std::string key) const;

	std::map<std::string, std::string>& getValues();
};

#endif