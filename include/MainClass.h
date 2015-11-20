/*
 * MainClass.h
 *
 *  Created on: Nov 12, 2015
 *      Author: supreeth
 */

#ifndef MAINCLASS_H_
#define MAINCLASS_H_

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
using namespace std;

#include "Logger.h"
#include "RoutingTable.h"

class MainClass {
public:
	Logger *logger;
	int sockfd;
	RoutingTable *routingTable;
	string filename;
	int port_number, infinity;
	double ttl, period;
	bool split_horizon;
	MainClass();
	MainClass(int argc, char *argv[]);
	virtual ~MainClass();
	void createAndInitializeRoutingTable(string configFileName,double ttl, int infinity,int port_number,bool split_horizon);
	void startServer(int portNum);
	void BellmanFord(RouteEntryVector graph, int src);
};

#endif /* MAINCLASS_H_ */
