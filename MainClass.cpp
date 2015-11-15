/*
 * MainClass.cpp
 *
 *  Created on: Nov 12, 2015
 *      Author: supreeth
 */

#include "MainClass.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <strings.h>
#include <sstream>

using namespace std;

#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

MainClass::MainClass() {
	// TODO Auto-generated constructor stub
}

MainClass::MainClass(int argc, char *argv[]) {
	// TODO Auto-generated constructor stub
	logger= new Logger();

	int portNum=atoi(argv[2]);
	int defaultTtl=atoi(argv[3]);
	int infinityValue=atoi(argv[4]);
	startServer(portNum);
	createAndInitializeRoutingTable(string(argv[1]),defaultTtl,infinityValue);

}

MainClass::~MainClass() {
	// TODO Auto-generated destructor stub
}

void MainClass::createAndInitializeRoutingTable(string configFileName,int defaultTtl, int infinityValue) {
	routingTable=new RoutingTable();
	routingTable->DEFAULT_TTL=defaultTtl;
	routingTable->INFINITY_VALUE=infinityValue;
	routingTable->initialize(configFileName);

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

int main(int argc, char *argv[]) {

	Logger *logger= new Logger();

	MainClass *main=new MainClass(argc,argv);

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
