/*
 * Logger.cpp
 *
 *  Created on: Oct 12, 2015
 *      Author: supreeth
 */

#include "../include/Logger.h"
#include <time.h>
#include <sys/time.h>

using namespace std;

Logger::Logger() {
	logFile.open ("../dvnode.log",ios::out | ios::app);
}

void Logger::cleanup() {
	cout << "Log Cleanup is called" << endl;
	logFile.open ("../dvnode.log",ios::out );
}

void Logger::logDebug(string logText) {
	char buffer[30];
	struct timeval tv;
	time_t curtime;
	gettimeofday(&tv, NULL);
	curtime = tv.tv_sec;
	strftime(buffer, 30, "%m-%d-%Y %T.", localtime(&curtime));
	logFile << buffer << tv.tv_usec << "[DEBUG] " << logText << endl;
}

void Logger::logError(string logText) {
	char buffer[30];
	struct timeval tv;
	time_t curtime;
	gettimeofday(&tv, NULL);
	curtime = tv.tv_sec;
	strftime(buffer, 30, "%m-%d-%Y %T.", localtime(&curtime));
	logFile << buffer << tv.tv_usec << "[ERROR] " << logText << endl;
}

void Logger::logInfo(string logText) {
	 char buffer[30];
	  struct timeval tv;
	  time_t curtime;
	 gettimeofday(&tv, NULL);
	 curtime=tv.tv_sec;

	 strftime(buffer,30,"%m-%d-%Y %T.",localtime(&curtime));
	 logFile << buffer << tv.tv_usec << "[INFO] " << logText << endl;
}
Logger::~Logger() {
	// TODO Auto-generated destructor stub
}

