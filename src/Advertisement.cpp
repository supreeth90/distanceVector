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
	logger= new Logger();
}

void Advertisement::loadAdFromRoutingTable(RoutingTable *routingTable, RouteEntry neighborAddress) {
	logger->logDebug(SSTR("In loadAdFromRoutingTable for " << inet_ntoa(neighborAddress.destination)));
	numOfEntries=routingTable->routingTableVector.size();
	for(int i=0;i<numOfEntries;i++) {
		AdEntry adEntry;
		if(routingTable->splitHorizon) {
			if ((neighborAddress.destination.s_addr
					== routingTable->routingTableVector.at(i).nextHop.s_addr)
					&& (routingTable->routingTableVector.at(i).destination.s_addr
							!= routingTable->routingTableVector.at(i).nextHop.s_addr)) {
				//Dont advertise this
				continue;
			}
		}
		adEntry.destination=(long)routingTable->routingTableVector.at(i).destination.s_addr;
		adEntry.cost=(long)routingTable->routingTableVector.at(i).cost;
		this->adEntryVector.push_back(adEntry);
		logger->logDebug(SSTR("Ad entry to send Destination:" << inet_ntoa(routingTable->routingTableVector.at(i).destination) << " cost:" << routingTable->routingTableVector.at(i).cost));
	}

}

Advertisement::~Advertisement() {
}

char* Advertisement::serializeToCharArray() {
	numOfEntries=this->adEntryVector.size();
	logger->logDebug(SSTR("numOfEntries in the advertisement "<<numOfEntries));
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
