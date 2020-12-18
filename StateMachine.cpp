/*
 * StateMachine.cpp
 *
 *  Created on: Oct 6, 2010
 *      Author: nathan
 */

#include "StateMachine.h"

StateMachine::StateMachine():
internalMachine("root"),
nav(0)
{
}

StateMachine::StateMachine(fstream& file):
internalMachine(file),
nav(0)
{
}

StateMachine::~StateMachine()
{
}

bool StateMachine::addState(string name)
{
	return internalMachine.newChild(name);
}

bool StateMachine::deleteState(string name)
{
	State* erase = internalMachine.findChild(name);
	bool ret = internalMachine.deleteChild(erase);
	if(erase == nav.getState()) nav.reset();
	return ret;
}

bool StateMachine::addTransition(string from, string to, string trigger)
{
	return internalMachine.linkChildren(from, to, trigger) != 0;
}

bool StateMachine::deleteTransition(string from, string trigger)
{
	return internalMachine.findChild(from)->deleteTransition(trigger);
}

bool StateMachine::setState(string name)
{
	State* state = internalMachine.findChild(name);
	if(!state) return false;
	nav.begin(state);
	return true;
}

bool StateMachine::transition(string trigger)
{
	return nav.transition(trigger);
}

int StateMachine::transition(list<string>& triggers)
{
	return nav.transition(triggers);
}

string StateMachine::currentState() const
{
	return nav.state();
}

void StateMachine::save(fstream& file)
{
	internalMachine.save(file);
}
