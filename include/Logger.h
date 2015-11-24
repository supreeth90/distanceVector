/*
 * Logger.h
 *
 *  Created on: Oct 12, 2015
 *      Author: supreeth
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
		( std::ostringstream() << std::dec << x ) ).str()

using namespace std;
class Logger {
public:
	Logger();
	virtual ~Logger();
	fstream logFile;
	void cleanup();
	void logDebug(string logText);
	void logError(string logText);
	void logInfo(string logText);
};

#endif /* LOGGER_H_ */
