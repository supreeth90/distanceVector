/*
 * RouteEntry.h
 *
 *  Created on: Nov 12, 2015
 *      Author: supreeth
 */

#ifndef ROUTEENTRY_H_
#define ROUTEENTRY_H_

#include <netinet/in.h>
#include <string>
#include <sstream>

class RouteEntry {
public:
	RouteEntry();
	virtual ~RouteEntry();
	struct in_addr destination;
	struct in_addr nextHop;
	int cost;
	long ttl;
	std::string getFormattedRouteEntry();
};

#endif /* ROUTEENTRY_H_ */
