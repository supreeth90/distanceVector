/*
 * RoutingTable.cpp
 *
 *  Created on: Nov 12, 2015
 *      Author: supreeth
 */

#include "../include/RoutingTable.h"



using namespace std;

#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

RoutingTable::RoutingTable() {
	// TODO Auto-generated constructor stub
	logger= new Logger();
}

RoutingTable::~RoutingTable() {
	// TODO Auto-generated destructor stub
}

void RoutingTable::addRouteEntry(RouteEntry routeEntry) {
	routingTableVector.push_back(routeEntry);
}

void RoutingTable::initialize(string fileName){
	//TODO:Read the file, parse and create a Route Entry object
	logger->logDebug(SSTR("Entering initialize with Filename: "<<fileName));
	INFINITY_VALUE=16;
	DEFAULT_TTL=120;
	fstream configFile;
	string line;

	char ipAddress[20]={0},isNeighbor[4]={0};
	configFile.open(fileName.c_str(), ios::in);
	if (configFile.is_open()) {
		logger->logDebug(SSTR("Config File open success"));
		 while ( getline (configFile,line) ) {
			 logger->logDebug(SSTR("Config File line read: " << line));
			 sscanf(line.c_str(),"%s %s",ipAddress,isNeighbor);
			 logger->logDebug(SSTR("Config File line current IpAddress: " << ipAddress << " neighbor: " << isNeighbor));
			 RouteEntry *routeEntry=new RouteEntry();
			 if(strstr(isNeighbor,"yes") != NULL) {
				 routeEntry->cost=1;
				 logger->logDebug(SSTR("Adding to Neighbor list"));
			 } else {
				 logger->logDebug(SSTR("Adding to non-neighbor list"));
				 routeEntry->cost=INFINITY_VALUE;
			 }
			 struct in_addr destination;
			 if (inet_aton(ipAddress, &destination) == 0) {
				 logger->logError(SSTR("Error in Dest ip conversion: " << ipAddress));
				 cout << "Error in converting " << endl;
			 }
			 routeEntry->destination=destination;

			 //TODO: Should be set only for neighbor.. check back
//			 struct in_addr nextHop;
//			if (inet_aton(ipAddress, &nextHop) == 0) {
//				cout << "Error in converting " << endl;
//			}
			routeEntry->nextHop=destination;

			struct timeval tv;
			gettimeofday(&tv, NULL);
			routeEntry->ttl=(long) tv.tv_sec + DEFAULT_TTL;

			addRouteEntry(*routeEntry);
		 }

	}



}
