/*
 * Advertisement.cpp
 *
 *  Created on: Nov 16, 2015
 *      Author: supreeth
 */

#include "../include/Advertisement.h"
#include "../include/AdEntry.h"
#include <stdlib.h>

Advertisement::Advertisement() {
}

void Advertisement::loadAdFromRoutingTable(RoutingTable *routingTable) {

	numOfEntries=routingTable->routingTableVector.size();

	for(int i=0;i<numOfEntries;i++) {
		AdEntry adEntry;
		adEntry.destination=(long)routingTable->routingTableVector.at(i).destination.s_addr;
		adEntry.cost=(long)routingTable->routingTableVector.at(i).cost;
		this->adEntryVector.push_back(adEntry);
	}

}

Advertisement::~Advertisement() {
	// TODO Auto-generated destructor stub
}

char* Advertisement::serializeToCharArray() {

	numOfEntries=this->adEntryVector.size();

	char *finalPacket;
	finalPacket = (char *) calloc(numOfEntries, AD_ENTRY_SIZE);

	for(int i=0;i<numOfEntries;i++) {
		AdEntry adEntry=this->adEntryVector.at(i);
		memcpy(finalPacket + (AD_ENTRY_SIZE*i) , &adEntry.destination, sizeof(long));
		memcpy(finalPacket + 4 + (AD_ENTRY_SIZE*i), &adEntry.cost, sizeof(long));
	}
	return finalPacket;
}

void Advertisement::deserializeToAdvertisement(unsigned char *datagramChars, int length) {

	this->numOfEntries=length;
	for(int i=0;i<length;i++) {
		AdEntry adEntry;
		adEntry.destination = convertToLong(datagramChars, 0 + (AD_ENTRY_SIZE*i));
		adEntry.cost = convertToLong(datagramChars, 4 + (AD_ENTRY_SIZE*i));
		this->adEntryVector.push_back(adEntry);
	}
}

long Advertisement::convertToLong(unsigned char *buffer, int startIndex) {
		long integerValue = (buffer[startIndex + 3] << 24)
				| (buffer[startIndex + 2] << 16) | (buffer[startIndex + 1] << 8)
				| (buffer[startIndex]);
		return integerValue;
}
