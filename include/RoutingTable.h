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
#include <time.h>
#include <sys/time.h>
#include <vector>

#include "RouteEntry.h"
#include "Logger.h"


using namespace std;

typedef std::vector<RouteEntry> RouteEntryVector;

class RoutingTable {
public:

	Logger *logger;
	RouteEntryVector routingTableVector;
	int INFINITY_VALUE;
	int DEFAULT_TTL;
	RoutingTable();
	virtual ~RoutingTable();
	void addRouteEntry(RouteEntry routeEntry);
	void initialize(string fileName);
};

#endif /* ROUTINGTABLE_H_ */
