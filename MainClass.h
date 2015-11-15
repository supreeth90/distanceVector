/*
 * MainClass.h
 *
 *  Created on: Nov 12, 2015
 *      Author: supreeth
 */

#ifndef MAINCLASS_H_
#define MAINCLASS_H_

#include "Logger.h"
#include "RoutingTable.h"

class MainClass {
public:
	Logger *logger;
	int sockfd;
	RoutingTable *routingTable;

	MainClass();
	MainClass(int argc, char *argv[]);
	virtual ~MainClass();
	void createAndInitializeRoutingTable(string configFileName,int defaultTtl, int infinityValue);
	void startServer(int portNum);
};

#endif /* MAINCLASS_H_ */
