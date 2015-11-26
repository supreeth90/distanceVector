/*
 * RoutingTable.cpp
 *
 *  Created on: Nov 12, 2015
 *      Author: supreeth
 */

#include "../include/Advertisement.h"
#include "../include/RoutingTable.h"
#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
		( std::ostringstream() << std::dec << x ) ).str()
const int MAX_PACKET_SIZE = 1472;

using namespace std;

RoutingTable::RoutingTable(int sockfd, int portNum, double ttl,
		int infinityValue, bool split_horizon) {
	// TODO Auto-generated constructor stub
	logger = new Logger();
	this->sockfd = sockfd;
	this->portNum = portNum;
	this->DEFAULT_TTL = ttl;
	this->INFINITY_VALUE = infinityValue;
	this->splitHorizon = split_horizon;
}

RoutingTable::~RoutingTable() {
	// TODO Auto-generated destructor stub
}

void RoutingTable::addRouteEntry(RouteEntry routeEntry) {
	routingTableVector.push_back(routeEntry);
}

string RoutingTable::getFormattedRoutingTable() {

	logger->logDebug(SSTR("In getFormattedRoutingTable"));
	cout << "Printing the routing table" << endl;
	stringstream routeTableStream;
	int routingTableSize = this->routingTableVector.size();
	for (int i = 0; i < routingTableSize; i++) {
		routeTableStream
		<< this->routingTableVector.at(i).getFormattedRouteEntry()
		<< endl;
		logger->logDebug(SSTR(this->routingTableVector.at(i).getFormattedRouteEntry()));
	}
	logger->logDebug(SSTR("End of printRoutingTable "));
	cout << "End of the routing table" << endl;
	return routeTableStream.str();
}

string RoutingTable::getFormattedGraphTable() {
	logger->logDebug(SSTR("In getFormattedGraphTable"));
	stringstream graphTableStream;
	cout << "Printing the routing table" << endl;
	//int routingTableSize=this->routingTableVector.size();
	for (int j = 0; j < numOfNodes; j++) {
		for (int k = 0; k < numOfNodes; k++) {
			graphTableStream << graph[j][k] << " ";
		}
		graphTableStream << endl;
	}
	string graphTable = graphTableStream.str();
	logger->logDebug(SSTR("End of printRoutingTable "));
	cout << "End of the routing table" << endl;
	return graphTable;
}

void RoutingTable::initialize(string fileName) {
	logger->logDebug(SSTR("Entering initialize with Filename: "<<fileName));
	fstream configFile;
	string line;
	numOfNodes = 0;

	char ipAddress[20] = { 0 }, isNeighbor[4] = { 0 };
	configFile.open(fileName.c_str(), ios::in);
	if (configFile.is_open()) {
		logger->logDebug(SSTR("Config File open success"));

		struct in_addr source = getSourceIpAddress();

		logger->logDebug(SSTR(" IP:" << inet_ntoa(source) <<" -> index:" << numOfNodes));
		hostToIndexMap.insert(make_pair(source.s_addr, numOfNodes));
		RouteEntry *routeEntry = new RouteEntry();
		routeEntry->destination = source;
		routeEntry->nextHop = source;
		routeEntry->cost = 0;
		struct timeval tv;
		gettimeofday(&tv, NULL);
		routeEntry->ttl = (long) tv.tv_sec + DEFAULT_TTL; // check back how to give infinity
		//Including my current node
		numOfNodes++;
		addRouteEntry(*routeEntry);

		while (getline(configFile, line)) {
			logger->logDebug(SSTR("Config File line read: " << line));
			sscanf(line.c_str(), "%s %s", ipAddress, isNeighbor);
			logger->logDebug(SSTR("Config File line current IpAddress: " << ipAddress << " neighbor: " << isNeighbor));
			RouteEntry *routeEntry = new RouteEntry();
			if (strstr(isNeighbor, "yes") != NULL) {
				routeEntry->cost = 1;
				logger->logDebug(SSTR("Adding to Neighbor list"));
			} else {
				logger->logDebug(SSTR("Adding to non-neighbor list"));
				routeEntry->cost = INFINITY_VALUE;
			}
			struct in_addr destination;
			if (inet_aton(ipAddress, &destination) == 0) {
				logger->logError(SSTR("Error in Dest ip conversion: " << ipAddress));
				cout << "Error in converting " << endl;
			}
			routeEntry->destination = destination;

			//TODO: Should be set only for neighbor.. check back
			//			 struct in_addr nextHop;
			//			if (inet_aton(ipAddress, &nextHop) == 0) {
			//				cout << "Error in converting " << endl;
			//			}
			routeEntry->nextHop = destination;

			struct timeval tv;
			gettimeofday(&tv, NULL);
			routeEntry->ttl = (long) tv.tv_sec + DEFAULT_TTL;

			logger->logDebug(SSTR(" IP:" << inet_ntoa(source) <<" -> index:" << numOfNodes));
			hostToIndexMap.insert(make_pair(destination.s_addr, numOfNodes));
			numOfNodes++;
			addRouteEntry(*routeEntry);
		}

		//Initializing graph nodes
		initiaizeGraph();

		cout << "graphInit done " << endl << getFormattedGraphTable() << endl;
	}
}

void RoutingTable::initiaizeGraph() {
	graph = new int*[numOfNodes];
	for (int i = 0; i < numOfNodes; i++) {
		graph[i] = new int[numOfNodes];
	}

	cout << "Initialized graph table" << endl;
	for (int j = 0; j < numOfNodes; j++) {
		for (int k = 0; k < numOfNodes; k++) {
			if (j == 0) {
				graph[j][k] = routingTableVector.at(k).cost;
			} else {
				graph[j][k] = INFINITY_VALUE;
			}
			cout << graph[j][k] << " ";
		}
		cout << endl;
	}
}

struct in_addr RoutingTable::getSourceIpAddress() {
	char sourceHostName[50] = { 0 };
	struct in_addr source;
	gethostname(sourceHostName, sizeof(sourceHostName));
	cout << "sourceHostName:" << sourceHostName << endl;
	struct hostent *he = gethostbyname(sourceHostName);
	if (he == NULL) {
		cout << "source hostent: is null" << endl;
	}
	struct in_addr **addr_list;

	addr_list = (struct in_addr **) he->h_addr_list;
	if (addr_list[0] != NULL) {
		cout << "Source IP resolved is " << inet_ntoa(*addr_list[0]) << endl;
		logger->logDebug(
				SSTR("Source IP Address: " << inet_ntoa(*addr_list[0])));
		source = *addr_list[0];
	} else {
		cerr << "Couldnt resolve source IP" << endl;
		exit(0);
	}
	return source;
}

void RoutingTable::sendAdvertisement() {
	cout << "Sending Advertisement" << endl;
	logger->logDebug(SSTR("In sendAdvertisement"));
	char *adPacket;
	int routingTableSize = this->routingTableVector.size();

	//Start the lock
	pthread_mutex_lock(&rtmutex);

	logger->logDebug(SSTR("Entering TTL validation" << this->routingTableVector.size()));
	//TTL Validation
	checkTtl();

	logger->logDebug(SSTR("Create and Send the Advertisement"));
	//Create and Send Ads

	for (int i = 0; i < routingTableSize; i++) {

		//Send the Ads to the neighbors
		if (this->routingTableVector.at(i).cost == 1) {

			//Get the Advertisement to be sent
			Advertisement *advertisement = new Advertisement();
			advertisement->loadAdFromRoutingTable(this,
					this->routingTableVector.at(i));
			adPacket = advertisement->serializeToCharArray();
			struct sockaddr_in neighborAddress;
			bzero((char *) &neighborAddress, sizeof(neighborAddress));
			neighborAddress.sin_addr =
					this->routingTableVector.at(i).destination;
			neighborAddress.sin_family = AF_INET;
			neighborAddress.sin_port = htons(portNum);
			sendto(sockfd, adPacket,
					advertisement->numOfEntries * AD_ENTRY_SIZE, 0,
					(struct sockaddr *) &neighborAddress,
					sizeof(neighborAddress));
			free(advertisement);
			logger->logDebug(SSTR("Advertisement sent to "<< inet_ntoa(this->routingTableVector.at(i).destination)));
		}
	}
	pthread_mutex_unlock(&rtmutex);
}

void RoutingTable::checkTtl() {
	long currentTime = 0;
	int routingTableSize = this->routingTableVector.size();
	//should not check ttl for its own node, i.e expecting advertisement from its own self to itself! so i = 1
	for (int i = 1; i < routingTableSize; i++) {
		struct timeval tv;
		gettimeofday(&tv, NULL);
		currentTime = (long) tv.tv_sec;
		if (currentTime > this->routingTableVector.at(i).ttl) {
			logger->logDebug(SSTR("Current time greater"));
			this->routingTableVector.at(i).cost = INFINITY_VALUE;
			graph[0][i] = INFINITY_VALUE;
		}
	}
}

void RoutingTable::createThreads() {

	logger->logDebug(SSTR("Creating Receive thread"));
	pthread_mutex_init(&rtmutex, NULL);
	int threadStatus;
	if ((threadStatus = pthread_create(&threads[0], NULL, testRun, this))) {
		cout << "Thread creation failed: " << threadStatus << endl;
	}
	pthread_mutex_destroy(&rtmutex);
	//	pthread_exit(NULL);
}

void * RoutingTable::testRun(void * This) {
	RoutingTable *routingTable = (RoutingTable *) This;
	cout << "Entered testRun" << endl;
	routingTable->logger->logDebug(SSTR("Entered testRun"));
	routingTable->receiveAdvertisement();
	return NULL;
}

void RoutingTable::receiveAdvertisement() {

	logger->logDebug(SSTR("In receiveAdvertisement "));
	int n = 0;
	bool valueChanged=false;
	unsigned char buffer[MAX_PACKET_SIZE];
	bzero(buffer, MAX_PACKET_SIZE);
	socklen_t addr_size;
	struct sockaddr_in neighborAddress;
	addr_size = sizeof(neighborAddress);
	int indexEntry;
	while (1) {
		cout << "Waiting for an Advertisement" << endl;
		while ((n = recvfrom(sockfd, buffer, MAX_PACKET_SIZE, 0,
				(struct sockaddr *) &neighborAddress, &addr_size)) <= 0)
			;
		logger->logDebug(SSTR("receivedAdvertisement from "<< inet_ntoa(neighborAddress.sin_addr)));

		int numOfEntries = n / AD_ENTRY_SIZE;
		cout << "numOfEntries in the Ad" << numOfEntries << endl;
		Advertisement *adv = new Advertisement();
		adv->deserializeToAdvertisement(buffer, numOfEntries);

		//TODO: Call process Ad to be processed using Bellman-ford algorithm
		//TODO: Triggered Update happens if there is any update in the routing table

		//Updating graph based on advertisement from a neighbor
		indexEntry = hostToIndexMap.at(
				(long) (neighborAddress.sin_addr.s_addr));
		int iIndex = 0;
		int advVecSize = adv->adEntryVector.size();
		pthread_mutex_lock(&rtmutex);
		for (int j = 0; j < advVecSize; j++) {
			iIndex = hostToIndexMap.at(adv->adEntryVector.at(j).destination);
			graph[indexEntry][iIndex] = adv->adEntryVector.at(j).cost;
		}
		updateTtl(indexEntry);
		pthread_mutex_unlock(&rtmutex);

		logger->logDebug(SSTR(getFormattedGraphTable()));
		for (int i = 0; i < advVecSize; i++) {
			struct in_addr address;
			address.s_addr = adv->adEntryVector.at(i).destination;
			cout << "dest: " << inet_ntoa(address);
			logger->logDebug(SSTR("In receiveAdvertisement ::" << address.s_addr << ".." << inet_ntoa(address)));
			cout << " cost: " << adv->adEntryVector.at(i).cost << endl;
		}
		logger->logDebug(SSTR("Routing table before bellman ford " << getFormattedRoutingTable() << endl));
		pthread_mutex_lock(&rtmutex);
		int routingTSize = routingTableVector.size();
		cout << "Routing Table before Bellman Ford:";
		for (int i =0; i< routingTSize; i++)
		{
			cout << "\nDestination:" << inet_ntoa(routingTableVector.at(i).destination);
			cout << "\nNext Hop:" << inet_ntoa(routingTableVector.at(i).nextHop);
			cout << "\n TTL:" << routingTableVector.at(i).ttl ;

			cout << "\n Cost:" << routingTableVector.at(i).cost ;

		}


		valueChanged=BellmanFord(graph, indexEntry);
		cout << "\n\nRouting Table after Bellman Ford:";
		for (int i =0; i< routingTSize; i++)
		{
			cout << "\nDestination:" << inet_ntoa(routingTableVector.at(i).destination);
			cout << "\nNext Hop:" << inet_ntoa(routingTableVector.at(i).nextHop);
			cout << "\n TTL:" << routingTableVector.at(i).ttl ;

			cout << "\n Cost:" << routingTableVector.at(i).cost ;
		}
		pthread_mutex_unlock(&rtmutex);
		logger->logDebug(SSTR("Routing table after bellman ford " << getFormattedRoutingTable() << endl));
		//Send Triggered updates when value is changed
		if(valueChanged) {
			logger->logDebug(SSTR("Sending triggered updates as value is changed " << endl));
			sendAdvertisement();
		}
	}
}

void RoutingTable::updateTtl(int index) {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	routingTableVector.at(index).ttl = tv.tv_sec + DEFAULT_TTL;
}

long RoutingTable::indexToHost(int index) {
	for (std::map<in_addr_t, int>::iterator it = hostToIndexMap.begin();
			it != (std::map<in_addr_t, int>::iterator) (hostToIndexMap.end());
			it++) {
		if (index == it->second) {
			return (long) it->first;
		}
	}
	return 0;
}

bool RoutingTable::BellmanFord(int** graph, int AdvIndexEntry) {
	logger->logDebug(SSTR("Running BellmanFord for " << AdvIndexEntry));
	bool valueChanged=false;
	int srcIndexEntry = hostToIndexMap.at(
			(long) (routingTableVector.at(0).destination.s_addr));
	int V = hostToIndexMap.size();

	logger->logDebug(SSTR("Running BellmanFord srcIndexEntry " << srcIndexEntry << " V:" << V));

	logger->logDebug(SSTR("Graph before bellman ford " << getFormattedGraphTable() << endl));
	//Bellman Ford Algorithm
	for (int i = 0; i < V; i++) {
		if (((graph[AdvIndexEntry][i] + graph[srcIndexEntry][AdvIndexEntry])
				< graph[srcIndexEntry][i]) && (0 != graph[AdvIndexEntry][i]) && (0 != graph[srcIndexEntry][i])) {
			//		if (((graph[AdvIndexEntry][i] + graph[srcIndexEntry][AdvIndexEntry])
			//			< INFINITY_VALUE) && (0 != graph[AdvIndexEntry][i])) {
			graph[srcIndexEntry][i] = graph[AdvIndexEntry][i]
			                                               + graph[srcIndexEntry][AdvIndexEntry];
			routingTableVector.at(i).cost = graph[srcIndexEntry][i];
			valueChanged=true;
			for (std::map<in_addr_t, int>::iterator it = hostToIndexMap.begin();
					it != (std::map<in_addr_t, int>::iterator) (hostToIndexMap.end());
					it++) {

				if (i == it->second)
					routingTableVector.at(i).destination.s_addr = it->first;

				if (AdvIndexEntry == it->second)
				{
					routingTableVector.at(i).nextHop.s_addr = it->first;
					struct timeval tv;
					gettimeofday(&tv, NULL);
					routingTableVector.at(i).ttl =   tv.tv_sec + DEFAULT_TTL;

				}

			}

		}
	}
	logger->logDebug(SSTR("Graph after bellman ford " << getFormattedGraphTable() << endl));
	return valueChanged;

}
