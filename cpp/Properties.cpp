#include <fstream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "../h/Properties.h"

Properties::Properties() {
};

Properties::Properties(std::string path) {
	load(path);
}

const std::string& Properties::find(std::string& key) const {
	auto search = values.find(key);
	if (search == values.end())
		throw std::invalid_argument(key + " does not exist");
	return search->second;
}

void Properties::load(std::string path) {
	std::ifstream in{ path };
	if (!in.is_open())
		throw std::invalid_argument{ "unable to open file" };

	while (!in.eof()) {
		std::string line;
		std::vector<std::string> vec;
		std::getline(in, line);
		boost::trim(line);

		//the line is empty or a comment, ignore it
		if (line.empty() || line.at(0) == '#')
			continue;

		//The boost::split method creates a msvc warning. See Problems.txt->Unsafe string copy
		boost::split(vec, line, boost::is_any_of("="), boost::token_compress_on);
		if (vec.size() < 2)
			continue;
		boost::trim(vec.at(0));
		boost::trim(vec.at(1));

		set(vec.at(0), vec.at(1));
	}
	in.close();
}

void Properties::store(std::string path) {
	std::ofstream out{ path };
	if (!out.is_open())
		throw std::invalid_argument{ "unable to open file" };

	for (auto it = values.begin(); it != values.end(); ++it)
		out << it->first << "=" << it->second << std::endl;
	out.close();
}

//TODO: may it is necessary to create string on heap
void Properties::set(std::string key, std::string value) {
	try {
		std::string s = find(key);
		s = value;
	}
	catch (std::invalid_argument& ex) {
		values.insert(std::make_pair(key, value));
	}
}

std::string Properties::get(std::string key) const {
	try {
		return find(key);
	}
	catch (std::invalid_argument& ex) {
		std::cerr << key << " is not stored in the properties. '' is returned as default value" << std::endl;
		return 0;
	}
}

int Properties::getInt(std::string key) const {
	try {
		return boost::lexical_cast<int>(find(key));
	}
	catch (std::invalid_argument& ex) {
		std::cerr << key << " is not stored in the properties. 0 is returned as default value" << std::endl;
		return 0;
	}
}

long Properties::getLong(std::string key) const {
	try {
		return boost::lexical_cast<long>(find(key));
	}
	catch (std::invalid_argument& ex) {
		std::cerr << key << " is not stored in the properties. 0 is returned as default value" << std::endl;
		return 0;
	}
}

double Properties::getDouble(std::string key) const {
	try {
		return boost::lexical_cast<double>(find(key));
	}
	catch (std::invalid_argument& ex) {
		std::cerr << key << " is not stored in the properties. 0 is returned as default value" << std::endl;
		return 0;
	}
}

bool Properties::getBool(std::string key) const {
	try {
		return boost::lexical_cast<bool>(find(key));
	}
	catch (std::invalid_argument& ex) {
		std::cerr << key << " is not stored in the properties. false is returned as default value" << std::endl;
		return 0;
	}
}

std::map<std::string, std::string>& Properties::getValues() {
	return values;
}