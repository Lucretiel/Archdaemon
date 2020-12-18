/*
 * Navigator.h
 *
 *  Created on: Oct 6, 2010
 *      Author: nathan
 */

#ifndef NAVIGATOR_H_
#define NAVIGATOR_H_

#include "State.h"

class Navigator
{
private:
	State* currentState;
	vector<string> hierarchyCache; //this is mainly here to reduce the overhead with repeated calls to the second version of hierarchy
public:
	Navigator(State* initialState = 0);
	Navigator(State& initialState);
	~Navigator();

	void reset();
	void begin(State* root);
	void begin(State& root);
	void begin();
	bool transition(string trigger);
	bool transition();
	bool transition(list<string>& triggers);

	string state() const {return currentState->getName();}
	vector<string> hierarchy();
	string hierarchy(int n);
	list<string> transitions() const {return currentState->getTransitions();}

	State* getState() const {return currentState;}
	string trigger;
};

#endif /* NAVIGATOR_H_ */
