/*
 * MainClass.cpp
 *
 *  Created on: Nov 12, 2015
 *      Author: supreeth
 */

#include "MainClass.h"

using namespace std;

MainClass::MainClass() {
	// TODO Auto-generated constructor stub

}

MainClass::~MainClass() {
	// TODO Auto-generated destructor stub
}

int main() {

	Logger *logger= new Logger();
	logger->logDebug("Starting the node!!");
	cout << "Hola amigos!!" << endl;

}
