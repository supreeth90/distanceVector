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
#include <map>
#include <sys/unistd.h>
#include <netdb.h>
#include <iterator>
#include <stdlib.h>

#include "RouteEntry.h"
#include "Logger.h"

using namespace std;

typedef std::vector<RouteEntry> RouteEntryVector;

class RoutingTable {
public:
	pthread_t threads[2];
	pthread_mutex_t rtmutex;
	int sockfd;
	int portNum;
	Logger *logger;
	RouteEntryVector routingTableVector;
	int **graph;
	map<in_addr_t, int> hostToIndexMap;
	int numOfNodes;
	int INFINITY_VALUE;
	int DEFAULT_TTL;
	int splitHorizon;
	struct timeval startTime;
	RoutingTable(int sockfd, int portNum, double ttl, int infinityValue,
			bool split_horizon);
	virtual ~RoutingTable();
	long indexToHost(int index);
	void addRouteEntry(RouteEntry routeEntry);
	void initialize(string fileName);
	void initiaizeGraph();
	struct in_addr getSourceIpAddress();
	struct in_addr getIpFromHostName(char *hostname);
	string getFormattedRoutingTable();
	string getFormattedGraphTable();
	void sendAdvertisement();
	void receiveAdvertisement();
	void updateTtl(int index);
	void checkTtl();
	void createThreads();
	static void * testRun(void * This);
	bool BellmanFord(int** graph, int indexEntry);
};

#endif /* ROUTINGTABLE_H_ */
