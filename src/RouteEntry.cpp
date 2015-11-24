/*
 * RouteEntry.cpp
 *
 *  Created on: Nov 12, 2015
 *      Author: supreeth
 */

#include "../include/RouteEntry.h"
#include <arpa/inet.h>
#include <iostream>
#include <string>

using namespace std;
RouteEntry::RouteEntry() {
	// TODO Auto-generated constructor stub

}

RouteEntry::~RouteEntry() {
	// TODO Auto-generated destructor stub
}

string RouteEntry::getFormattedRouteEntry() {
	stringstream routeEntryString;
	string routeEntry;
//	routeEntryString << "Destination:" << inet_ntoa(this->destination)
//			<< " nextHop:" << inet_ntoa(this->nextHop) << " cost:" << this->cost
//			<< " ttl:" << this->ttl;
	routeEntry.append("Destination:");
	routeEntry.append(inet_ntoa(this->destination));
	routeEntry.append(" nextHop:");
	routeEntry.append(inet_ntoa(this->nextHop));
	routeEntryString << " cost:" << this->cost;
	routeEntryString << " ttl:" << this->ttl;
	routeEntry.append(routeEntryString.str());
//	cout << routeEntry << endl;
//	cout << "----" << endl;
//	cout << routeEntryString.str() << endl;
	return routeEntry;
}
