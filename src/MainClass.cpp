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
	logger->cleanup();
	filename = argv[1];
	port_number = std::atoi(argv[2]);
	ttl = std::atof(argv[3]);
	infinity = std::atoi(argv[4]);
	period = std::atof(argv[5]);

	if (((std::string) argv[6]).compare("true") == 0) {
		split_horizon = true;
	} else {
		split_horizon = false;
	}
}

MainClass::~MainClass() {
	// TODO Auto-generated destructor stub
}

void MainClass::createAndInitializeRoutingTable(string configFileName,double ttl, int infinity,int port_number,bool split_horizon) {
	logger->logDebug(SSTR("In createAndInitializeRoutingTable " <<ttl << " inf:"<< infinity <<" port:"<< port_number));
	routingTable=new RoutingTable(sockfd,port_number,ttl,infinity,split_horizon);
	routingTable->initialize(configFileName);
	cout << routingTable->getFormattedRoutingTable() << endl;
}

void MainClass::startServer(int portNum) {
	struct sockaddr_in server_addr;
	logger->logDebug(SSTR("Starting the node... port:" <<portNum));
	cout << "Starting the Node at port:" << portNum << endl;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (sockfd < 0) {
		logger->logError(SSTR("Socket open failure"));
		cerr << "Socket open failure" << endl;
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
	logger->logDebug(SSTR("Node started successfully"));
	cout << "Node started successfully" << endl;

}

int main(int argc, char *argv[]) {

	// Check the number of parameters
	if (argc < 6) {

		std::cerr << "Usage: " << argv[0] << " <Config> <Portnumber> <TTL> <Infinity> <Period> <Split Horizon bool> " << std::endl;
		return 1;//Description of arguments if less than 6
	}
	int period=atoi(argv[5]);
	MainClass *main=new MainClass(argc,argv);
	main->startServer(main->port_number);
	main->createAndInitializeRoutingTable(string(argv[1]),main->ttl,main->infinity,main->port_number,main->split_horizon);
	main->routingTable->createThreads();

	while(1) {
		main->routingTable->sendAdvertisement();
		cout << "Sleeping for " << period << " s before sending an Ad" << endl;
		sleep(period);
	}

}
