/*
 * Advertisement.h
 *
 *  Created on: Nov 16, 2015
 *      Author: supreeth
 */

#ifndef SRC_ADVERTISEMENT_H_
#define SRC_ADVERTISEMENT_H_

#include "AdEntry.h"
#include "RoutingTable.h"
#include <vector>

using namespace std;
const int AD_ENTRY_SIZE=8;
class Advertisement {
public:
	int numOfEntries;
	std::vector<AdEntry> adEntryVector;
	Advertisement();
	void loadAdFromRoutingTable(RoutingTable *routingTable);
	virtual ~Advertisement();
	char* serializeToCharArray();
	void deserializeToAdvertisement(unsigned char *datagramChars, int length);
	long convertToLong(unsigned char *buffer, int startIndex);
};

#endif /* SRC_ADVERTISEMENT_H_ */
