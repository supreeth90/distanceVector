/*
 * Advertisement.cpp
 *
 *  Created on: Nov 16, 2015
 *      Author: supreeth
 */

#include "../include/Advertisement.h"
#include "../include/AdEntry.h"
#include <stdlib.h>
#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

Advertisement::Advertisement() {
	logger= new Logger();
}

void Advertisement::loadAdFromRoutingTable(RoutingTable *routingTable, RouteEntry neighborAddress) {
	logger->logDebug(SSTR("In loadAdFromRoutingTable"));
	numOfEntries=routingTable->routingTableVector.size();
	logger->logDebug(SSTR("numOfEntries In loadAdFromRoutingTable" << numOfEntries));
	for(int i=0;i<numOfEntries;i++) {
		AdEntry adEntry;
		if(routingTable->splitHorizon) {
			if (neighborAddress.nextHop.s_addr
					== routingTable->routingTableVector.at(i).destination.s_addr) {
				//Dont advertise this
				continue;
			}
		}
		adEntry.destination=(long)routingTable->routingTableVector.at(i).destination.s_addr;
		adEntry.cost=(long)routingTable->routingTableVector.at(i).cost;
		this->adEntryVector.push_back(adEntry);
		logger->logDebug(SSTR("Creating an ad entry for " << inet_ntoa(routingTable->routingTableVector.at(i).destination)));
	}

}

Advertisement::~Advertisement() {
	// TODO Auto-generated destructor stub
}

char* Advertisement::serializeToCharArray() {

	logger->logDebug(SSTR("In serializeToCharArray::"));
	numOfEntries=this->adEntryVector.size();
	logger->logDebug(SSTR("numOfEntries In serializeToCharArray::"<<numOfEntries));
	char *finalPacket;
	finalPacket = (char *) calloc(numOfEntries, AD_ENTRY_SIZE);
	for(int i=0;i<numOfEntries;i++) {
		AdEntry adEntry=this->adEntryVector.at(i);
		memcpy(finalPacket + (AD_ENTRY_SIZE*i) , &(adEntry.destination), sizeof(int));
		memcpy(finalPacket + 4 + (AD_ENTRY_SIZE*i), &(adEntry.cost), sizeof(int));
	}
	return finalPacket;
}

void Advertisement::deserializeToAdvertisement(unsigned char *datagramChars, int length) {

	cout << "length in deSerializeee::" << length << endl;
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
