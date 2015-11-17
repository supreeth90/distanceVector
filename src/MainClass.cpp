/*
 * MainClass.cpp
 *
 *  Created on: Nov 12, 2015
 *      Author: supreeth
 */

#include "../include/MainClass.h"
#include <pthread.h>

#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
		( std::ostringstream() << std::dec << x ) ).str()

MainClass::MainClass() {
	// TODO Auto-generated constructor stub
}

MainClass::MainClass(int argc, char *argv[]) {
	// TODO Auto-generated constructor stub
	logger= new Logger();

	filename = argv[1];
	port_number = std::atoi(argv[2]);
	ttl = std::atof(argv[3]);
	infinity = std::atoi(argv[4]);
	period = std::atof(argv[5]);


	if (((std::string)argv[6]).compare("true") == 0){
		split_horizon = true;
	}
	else split_horizon = false;

}

MainClass::~MainClass() {
	// TODO Auto-generated destructor stub
}

void MainClass::createAndInitializeRoutingTable(string configFileName,double ttl, int infinity,int port_number) {
	logger->logDebug(SSTR("In createAndInitializeRoutingTable " <<ttl << " inf:"<< infinity <<" port:"<< port_number));
	routingTable=new RoutingTable(sockfd,port_number);
	routingTable->DEFAULT_TTL=ttl;
	routingTable->INFINITY_VALUE=infinity;
	routingTable->initialize(configFileName);
	routingTable->printRoutingTable();

}
void MainClass::startServer(int portNum) {
	struct sockaddr_in server_addr;
	logger->logDebug(SSTR("Starting the node... port:" <<portNum));
	//	logger->logFile.close();
	cout << "Starting the Reliable UDP based File Server at port:" << portNum << endl;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (sockfd < 0) {
		cout << "Socket open failure" << endl;
		exit(0);
	}
	bzero((char *) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(portNum);

	/**binding socket to the required port**/
	if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
		logger->logError(SSTR("binding error"));
		exit(0);
	}
	cout << "**Server Binding set to :" << server_addr.sin_addr.s_addr << endl;
	cout << "**Server Binding set to port:" << server_addr.sin_port << endl;
	cout << "**Server Binding set to family:" << server_addr.sin_family << endl;
	//	listen(sfd, 5);
	logger->logDebug(SSTR("Reliable UDP FileServer started successfully"));
	cout << "Reliable UDP FileServer started successfully" << endl;
}
void MainClass::BellmanFord(RouteEntryVector graph, int src)
{
/*
	int V = graph.size();
	int E = 0;
	int dist[V];
	for(int i=0;i<V;i++) {
		if (1 == graph.at(i).cost)
			E++;
	}



	// Step 1: Initialize distances from src to all other vertices
	// as INFINITE
	for (int i = 0; i < V; i++)
		dist[i]   = infinity;
	dist[src] = 0;

	// Step 2: Relax all edges |V| - 1 times. A simple shortest
	// path from src to any other vertex can have at-most |V| - 1
	// edges
	for (int i = 1; i <= V-1; i++)
	{
		for (int j = 0; j < E; j++)
		{
			int u = graph->edge[j].src;graph.at(i).
			int v = graph->edge[j].dest;
			int weight = graph->edge[j].weight;
			if (dist[u] != infinity && dist[u] + weight < dist[v])
				dist[v] = dist[u] + weight;
		}
	}

	// Step 3: check for negative-weight cycles.  The above step
	// guarantees shortest distances if graph doesn't contain
	// negative weight cycle.  If we get a shorter path, then there
	// is a cycle.
	for (int i = 0; i < E; i++)
	{
		int u = graph->edge[i].src;
		int v = graph->edge[i].dest;
		int weight = graph->edge[i].weight;
		if (dist[u] != INT_MAX && dist[u] + weight < dist[v])
			printf("Graph contains negative weight cycle");
	}

	//printArr(dist, V);

	return;
*/
}
//void MainClass::receiveAds() {
//
//	while(1) {
//		routingTable->receiveAdvertisement();
//	}
//
//
//}
int main(int argc, char *argv[]) {

	pthread_t threads[2];
	Logger *logger= new Logger();

	// Check the number of parameters
	if (argc < 6) {

		std::cerr << "Usage: " << argv[0] << " <Config> <Portnumber> <TTL> <Infinity> <Period> <Split Horizon bool> " << std::endl;
		return 1;//Description of arguments if less than 6
	}

	MainClass *main=new MainClass(argc,argv);
	main->startServer(main->port_number);
	main->createAndInitializeRoutingTable(string(argv[1]),main->ttl,main->infinity,main->port_number);
	main->routingTable->sendAdvertisement();
    main->routingTable->receiveAdvertisement();

//	 for (int i = 0; i < 2; i++) {
//	    if ((rc = pthread_create(&thr[i], NULL, thr_func, &thr_data[i]))) {
//
//	    }
//	 }

	//TODO: Command line arguments input and processing
	// argv[1] config file name
	// argv[2]	port number
	// argv[3]	ttl
	// argv[4]	infinity
	// argv[5]	period
	// argv[6]	split horizon ("yes" or "no"


	//	main->startServer(portNum);


	//	logger->logDebug("Starting the node!!");
	cout << "Hola amigos!!" << endl;

}
