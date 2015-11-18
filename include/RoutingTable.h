/*
 * RoutingTable.h
 *
 *  Created on: Nov 12, 2015
 *      Author: supreeth
 */

#ifndef ROUTINGTABLE_H_
#define ROUTINGTABLE_H_

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <fstream>
#include <string>
#include <cstring>
#include <strings.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/time.h>
#include <vector>
#include <pthread.h>

#include "RouteEntry.h"
#include "Logger.h"


using namespace std;

typedef std::vector<RouteEntry> RouteEntryVector;

class RoutingTable {
public:
	pthread_t threads[2];
	int sockfd;
	int portNum;
	Logger *logger;
	RouteEntryVector routingTableVector;
	int INFINITY_VALUE;
	int DEFAULT_TTL;
	RoutingTable(int sockfd,int portNum);
	virtual ~RoutingTable();
	void addRouteEntry(RouteEntry routeEntry);
	void initialize(string fileName);
	void printRoutingTable();
	void sendAdvertisement();
	void receiveAdvertisement();
	void createThreads();
	static void * testRun(void * This);
};

#endif /* ROUTINGTABLE_H_ */
