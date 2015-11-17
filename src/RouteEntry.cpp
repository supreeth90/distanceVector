/*
 * RouteEntry.cpp
 *
 *  Created on: Nov 12, 2015
 *      Author: supreeth
 */

#include "../include/RouteEntry.h"
#include <arpa/inet.h>

using namespace std;
RouteEntry::RouteEntry() {
	// TODO Auto-generated constructor stub

}

RouteEntry::~RouteEntry() {
	// TODO Auto-generated destructor stub
}

string RouteEntry::getFormattedRouteEntry() {
	stringstream routeEntryString;
	routeEntryString << "Destination:" << inet_ntoa(this->destination)
			<< " nextHop:" << inet_ntoa(this->nextHop) << " cost:" << this->cost
			<< " ttl:" << this->ttl;
	return routeEntryString.str();
}
