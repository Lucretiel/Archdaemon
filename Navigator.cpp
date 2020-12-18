/*
 * Navigator.cpp
 *
 *  Created on: Oct 6, 2010
 *      Author: nathan
 */

#include "Navigator.h"

Navigator::Navigator(State* initialState):
currentState(initialState)
{
}

Navigator::Navigator(State& initialState):
currentState(&initialState)
{
}

Navigator::~Navigator()
{
}

void Navigator::reset()
{
	currentState = 0;
	trigger.clear();
	hierarchyCache.clear();
}

void Navigator::begin(State& root)
{
	begin(&root);
}

void Navigator::begin(State* root)
{
	if(root)
		currentState = root->enter();
	hierarchyCache.clear();
}

void Navigator::begin()
{
	begin(currentState);
}

bool Navigator::transition(string trigger)
{
	if(!currentState) return false;
	State* trans = currentState->transition(trigger);
	if(trans != 0)
	{
		currentState = trans;
		hierarchyCache.clear();
		return true;
	}
	return false;
}

bool Navigator::transition()
{
	return transition(trigger);
}

bool Navigator::transition(list<string>& triggers)
{
	for(list<string>::iterator it = triggers.begin(); it != triggers.end(); it++)
		if(!transition(*it))
			return false;

	return true;
}

vector<string> Navigator::hierarchy()
{
	if(hierarchyCache.empty())
		hierarchyCache = currentState->getHierarchy(); //optimizes for repeated calls to the second version of hierarchy
	return hierarchyCache;
}

string Navigator::hierarchy(int n)
{
	return hierarchy().at(n);
}
