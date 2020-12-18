/*
 * StateMachine.h
 *
 *  Created on: Oct 6, 2010
 *      Author: nathan
 */

#ifndef STATEMACHINE_H_
#define STATEMACHINE_H_

#include "State.h"
#include "Navigator.h"
#include <string>
#include <fstream>
#include <list>
using std::fstream;
using std::string;
using std::list;

class StateMachine
{
private:
	State internalMachine;
	Navigator nav;
public:
	StateMachine();
	StateMachine(fstream& file);
	~StateMachine();

	//construction
	bool addState(string name);
	bool deleteState(string name);
	bool addTransition(string from, string to, string trigger);
	bool deleteTransition(string name, string trigger);

	//execution
	bool setState(string name);
	bool transition(string trigger);
	int transition(list<string>& triggers);
	string currentState() const;

	//save/load
	void save(fstream& file);
};

#endif /* STATEMACHINE_H_ */
