/*
 * MainClass.h
 *
 *  Created on: Nov 12, 2015
 *      Author: supreeth
 */

#ifndef MAINCLASS_H_
#define MAINCLASS_H_

#include "Logger.h"

class MainClass {
public:
	MainClass();
	virtual ~MainClass();
	Logger *logger;
};

#endif /* MAINCLASS_H_ */
