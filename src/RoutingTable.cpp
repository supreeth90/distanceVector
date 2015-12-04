/*
 * RoutingTable.cpp
 *
 *  Created on: Nov 12, 2015
 *      Author: supreeth
 */

#include "../include/Advertisement.h"
#include "../include/RoutingTable.h"
const int MAX_PACKET_SIZE = 1472;

using namespace std;

RoutingTable::RoutingTable(int sockfd, int portNum, double ttl,
		int infinityValue, bool split_horizon) {
	logger = new Logger();
	this->sockfd = sockfd;
	this->portNum = portNum;
	this->DEFAULT_TTL = ttl;
	this->INFINITY_VALUE = infinityValue;
	this->splitHorizon = split_horizon;
}

RoutingTable::~RoutingTable() {
}

void RoutingTable::addRouteEntry(RouteEntry routeEntry) {
	routingTableVector.push_back(routeEntry);
}

string RoutingTable::getFormattedRoutingTable() {
	logger->logDebug(SSTR("In getFormattedRoutingTable"));
	stringstream routeTableStream;
	int routingTableSize = this->routingTableVector.size();
	routeTableStream << endl;
	for (int i = 0; i < routingTableSize; i++) {
		routeTableStream
		<< this->routingTableVector.at(i).getFormattedRouteEntry()
		<< endl;
	}
	return routeTableStream.str();
}

string RoutingTable::getFormattedGraphTable() {
	logger->logDebug(SSTR("In getFormattedGraphTable"));
	stringstream graphTableStream;
	graphTableStream << endl;
	for (int j = 0; j < numOfNodes; j++) {
		for (int k = 0; k < numOfNodes; k++) {
			graphTableStream << graph[j][k] << "   ";
		}
		graphTableStream << endl;
	}
	string graphTable = graphTableStream.str();
	return graphTable;
}

void RoutingTable::initialize(string fileName) {
	logger->logDebug(SSTR("Entering initialize with Filename: "<<fileName));
	fstream configFile;
	string line;
	numOfNodes = 0;

	char ipAddress[50] = { 0 }, isNeighbor[4] = { 0 };
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
		routeEntry->ttl = (long) tv.tv_sec + DEFAULT_TTL;
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
//			gethostbyname()
			destination=getIpFromHostName(ipAddress);
			routeEntry->destination = destination;
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

	for (int j = 0; j < numOfNodes; j++) {
		for (int k = 0; k < numOfNodes; k++) {
			if (j == 0) {
				graph[j][k] = routingTableVector.at(k).cost;
			} else {
				graph[j][k] = INFINITY_VALUE;
			}
		}
	}
}

struct in_addr RoutingTable::getSourceIpAddress() {
	char sourceHostName[50] = { 0 };
	struct in_addr source;
	gethostname(sourceHostName, sizeof(sourceHostName));
	struct hostent *he = gethostbyname(sourceHostName);
	if (he == NULL) {
		cerr << "source hostent: is null" << endl;
		exit(0);
	}
	struct in_addr **addr_list;

	addr_list = (struct in_addr **) he->h_addr_list;
	if (addr_list[0] != NULL) {
		cout << "Source IP is " << inet_ntoa(*addr_list[0]) << endl;
		logger->logDebug(
				SSTR("Source IP Address: " << inet_ntoa(*addr_list[0])));
		source = *addr_list[0];
	} else {
		cerr << "Couldn't resolve source IP" << endl;
		exit(0);
	}
	return source;
}

struct in_addr RoutingTable::getIpFromHostName(char *hostname){
	struct in_addr hostIp;
	struct hostent *he = gethostbyname(hostname);
	if (he == NULL) {
		cerr << " hostname resolution failure for " << hostname << endl;
		exit(0);
	}
	struct in_addr **addr_list;
	addr_list = (struct in_addr **) he->h_addr_list;
	if (addr_list[0] != NULL) {
		logger->logDebug(
				SSTR("HostName: " << hostname << "IP: "<< inet_ntoa(*addr_list[0])));
		hostIp = *addr_list[0];
	} else {
		cerr << "Couldn't resolve hostname of " << hostname << endl;
		exit(0);
	}
	return hostIp;

}

void RoutingTable::sendAdvertisement() {
	logger->logDebug(SSTR("In sendAdvertisement"));
	char *adPacket;
	int routingTableSize = this->routingTableVector.size();

	//Start the lock
	pthread_mutex_lock(&rtmutex);

	logger->logDebug(SSTR("Entering TTL validation"));
	//TTL Validation (will occur regardless of splitHorizon)
	checkTtl();

	logger->logDebug(SSTR("Routing table before sending advertisement" << getFormattedRoutingTable()));
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
			cout << "Advertisement sent to " << inet_ntoa(this->routingTableVector.at(i).destination) << endl;
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
			logger->logDebug(SSTR("TTL Expired for " << inet_ntoa(this->routingTableVector.at(i).destination)));
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
		cout << "receivedAdvertisement from " << inet_ntoa(neighborAddress.sin_addr) << endl;

		logger->logDebug(SSTR("Routing table when advertisement received" << getFormattedRoutingTable()));

		int numOfEntries = n / AD_ENTRY_SIZE;
		cout << "numOfEntries in the Ad" << numOfEntries << endl;
		Advertisement *adv = new Advertisement();
		adv->deserializeToAdvertisement(buffer, numOfEntries);

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
		checkTtl();
		pthread_mutex_unlock(&rtmutex);

		logger->logDebug(SSTR(getFormattedGraphTable()));
		for (int i = 0; i < advVecSize; i++) {
			struct in_addr address;
			address.s_addr = adv->adEntryVector.at(i).destination;
			logger->logDebug(SSTR("receivedAdvertisement Entries::" << inet_ntoa(address) << " Cost:" << adv->adEntryVector.at(i).cost));
		}
		pthread_mutex_lock(&rtmutex);
		valueChanged=BellmanFord(graph, indexEntry);
		pthread_mutex_unlock(&rtmutex);

		//Send Triggered updates when value is changed
		if(valueChanged) {
			logger->logDebug(SSTR("Sending triggered updates as value is changed " << endl));
			sendAdvertisement();
		}
	}
}

void RoutingTable::updateTtl(int index) {
	logger->logDebug(SSTR("In updateTtl for " << index));
	struct timeval tv;
	gettimeofday(&tv, NULL);
	routingTableVector.at(index).ttl = tv.tv_sec + DEFAULT_TTL;
	for(int i=0;i<routingTableVector.size();i++) {  //Updating TTL whose nextHop is index
		if(routingTableVector.at(i).nextHop.s_addr==indexToHost(index)) {
			logger->logDebug(SSTR("Updating TTL for " << inet_ntoa(routingTableVector.at(i).destination)));
			routingTableVector.at(i).ttl=tv.tv_sec + DEFAULT_TTL;
		}
	}
}

long RoutingTable::indexToHost(int index) {
	for (std::map<in_addr_t, int>::iterator it = hostToIndexMap.begin();
			it != (std::map<in_addr_t, int>::iterator) (hostToIndexMap.end());
			it++) {
		if (index == it->second) {
//			logger->logDebug(SSTR("In indexToHost for " << index << " is " << (long) it->first));
			return (long) it->first;
		}
	}
	return 0;
}

bool RoutingTable::BellmanFord(int** graph, int AdvIndexEntry) {
    bool valueChanged=false;
    int srcIndexEntry = hostToIndexMap.at(
            (long) (routingTableVector.at(0).destination.s_addr));
    int V = hostToIndexMap.size();
    int *d;
    logger->logDebug(SSTR("Running BellmanFord for " << AdvIndexEntry << " V:" << V));
    logger->logDebug(SSTR("Graph before bellman ford " << getFormattedGraphTable() << endl));
    d=new int[V];

    //Initialization step of Bellman ford
    for (int j = 1; j < V; j++) {
        d[j] = INFINITY_VALUE;
    }

    d[srcIndexEntry] = 0;


    //Relax step of bellman ford
    for (int i = 1; i < V; i++)
    {
        for (int j = 0; j < V; j++)
        {
            for (int k = 0; k < V; k++)
            {
                if (0 != graph[j][k])
                {
                    if ((INFINITY_VALUE != d[j]) && ((d[j] + graph[j][k]) < d[k] ))
                    {
                        d[k] = d[j] + graph[j][k];
                        logger->logDebug(SSTR("777:j " << j << " k " << k << " d[k]" << d[k]));
                        struct in_addr hop;
                        if(j == 0) { //Directly connected hence assigning the hop itself as next hop
                        	hop.s_addr=indexToHost(k);
                        } else {
                        	hop.s_addr=indexToHost(j);
                        }
                        logger->logDebug(SSTR(inet_ntoa(hop)));
                        routingTableVector.at(k).nextHop=hop;
                    }
                }
            }

            /*if(routingTableVector.at(i).nextHop.s_addr != routingTableVector.at(i).destination.s_addr) {
            nextHopIndex=hostToIndexMap.at(
                        (long) (routingTableVector.at(i).nextHop.s_addr));

            if (graph[srcIndexEntry][i] != INFINITY_VALUE
                    && graph[srcIndexEntry][nextHopIndex]
                            + graph[nextHopIndex][i]
                            != graph[srcIndexEntry][i]) {
                graph[srcIndexEntry][i] = graph[srcIndexEntry][nextHopIndex]
                        + graph[nextHopIndex][i];
                routingTableVector.at(i).cost = graph[srcIndexEntry][i];
                if (graph[srcIndexEntry][i] >= INFINITY_VALUE) {
                    routingTableVector.at(i).cost = INFINITY_VALUE;
                    graph[srcIndexEntry][i] = INFINITY_VALUE;
                }
                valueChanged = true;
                logger->logDebug(
                        SSTR(
                                "Cost changed because the nextHop's cost changed" << getFormattedGraphTable() << endl));
            }
        }*/

            /*if (((graph[AdvIndexEntry][i] + 1)
                < graph[srcIndexEntry][i]) && (0 != graph[AdvIndexEntry][i]) && (0 != graph[srcIndexEntry][i]))
                {
            graph[srcIndexEntry][i] = graph[AdvIndexEntry][i] + 1;
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

        }*/
        }
    }
    logger->logDebug(SSTR("Distance vector after bellman ford"));

    //Compare and check if there is any route changes
	for (int k = 0; k < V; k++) {
		if (routingTableVector.at(k).cost != d[k]) {
			routingTableVector.at(k).cost = d[k];
			updateTtl(k);
			valueChanged = true;
		}
		logger->logDebug(SSTR(d[k]<< " "));
	}
    return valueChanged;
}
