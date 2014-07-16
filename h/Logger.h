#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <boost/log/trivial.hpp>

class Logger {
private:
	static Logger *instance;
	int level;
	Logger();

protected:
	virtual void log(int level, std::string message);
	void(*logger2)(std::string message) = NULL;

public:
	static Logger* getInstance();
	static void release();
	static void init(int level, std::string file);
	void addLogFunction(void (*pointer)(std::string));
	
	void trace(std::string message);
	void debug(std::string message);
	void info(std::string message);
	void warn(std::string message);
	void error(std::string message);
	void fatal(std::string message);
};

#endif