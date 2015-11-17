/*
 * RoutingTable.cpp
 *
 *  Created on: Nov 12, 2015
 *      Author: supreeth
 */

#include "../include/RoutingTable.h"
#include "../include/Advertisement.h"

const int MAX_PACKET_SIZE=1472;

using namespace std;

#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

RoutingTable::RoutingTable(int sockfd,int portNum) {
	// TODO Auto-generated constructor stub
	logger= new Logger();
	this->sockfd=sockfd;
	this->portNum=portNum;
}

RoutingTable::~RoutingTable() {
	// TODO Auto-generated destructor stub
}

void RoutingTable::addRouteEntry(RouteEntry routeEntry) {
	routingTableVector.push_back(routeEntry);
}

void RoutingTable::printRoutingTable(){
	logger->logDebug(SSTR("In printRoutingTable "));
	cout << "Printing the routing table" << endl;
	int routingTableSize=this->routingTableVector.size();
	for(int i=0;i<routingTableSize;i++) {
		cout << this->routingTableVector.at(i).getFormattedRouteEntry() << endl;
	}
	logger->logDebug(SSTR("End of printRoutingTable "));
	cout << "End of the routing table" << endl;
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

void RoutingTable::sendAdvertisement(){
	logger->logDebug(SSTR("In sendAdvertisement"));
	long currentTime=0;
	char *adPacket;

	logger->logDebug(SSTR("Entering TTL validation"));
	//TTL Validation
	for(int i=0;i<this->routingTableVector.size();i++) {
		struct timeval tv;
		gettimeofday(&tv, NULL);
		currentTime=(long)tv.tv_sec;
		if(currentTime > this->routingTableVector.at(i).ttl) {
			this->routingTableVector.at(i).cost=INFINITY_VALUE;
		}
	}

	logger->logDebug(SSTR("Create and Send the Advertisement"));
	//Create and Send Ads
	Advertisement *advertisement=new Advertisement();
	advertisement->loadAdFromRoutingTable(this);
	adPacket=advertisement->serializeToCharArray();
	for(int i=0;i<this->routingTableVector.size();i++) {
		//Send the Ads to the neighbors
		if(this->routingTableVector.at(i).cost == 1) {
			struct sockaddr_in neighborAddress;
			bzero((char *) &neighborAddress, sizeof(neighborAddress));
			neighborAddress.sin_addr=this->routingTableVector.at(i).destination;
			neighborAddress.sin_family=AF_INET;
			neighborAddress.sin_port=htons(portNum);
			sendto(sockfd, adPacket, advertisement->numOfEntries , 0, (struct sockaddr *) &neighborAddress,
							sizeof(neighborAddress));
			logger->logDebug(SSTR("Advertisement sent to "<< inet_ntoa(this->routingTableVector.at(i).destination)));
		}

	}

}

void RoutingTable::receiveAdvertisement() {

	logger->logDebug(SSTR("In receiveAdvertisement "));
	int n=0;
	char buffer[MAX_PACKET_SIZE];
	bzero(buffer,MAX_PACKET_SIZE);
	socklen_t addr_size;
	struct sockaddr_in neighborAddress;
	addr_size = sizeof(neighborAddress);

	while ((n=recvfrom(sockfd, buffer, MAX_PACKET_SIZE, 0,
				(struct sockaddr *) &neighborAddress, &addr_size)) <= 0);
	logger->logDebug(SSTR("receiveAdvertisement from "<< inet_ntoa(neighborAddress.sin_addr)));

//	int numOfEntries=n/AD_ENTRY_SIZE;

}
