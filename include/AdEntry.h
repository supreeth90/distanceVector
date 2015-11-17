/*
 * AdEntry.h
 *
 *  Created on: Nov 16, 2015
 *      Author: supreeth
 */

#ifndef SRC_ADENTRY_H_
#define SRC_ADENTRY_H_

class AdEntry {
public:

	long destination;
	long cost;

	AdEntry();
	virtual ~AdEntry();
	char* serializeToCharArray();
};

#endif /* SRC_ADENTRY_H_ */
