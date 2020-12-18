/*
 * State.h
 *
 *  Created on: Sep 30, 2010
 *      Author: nathan
 *
 *  State is the base class for states in a finite state machine.
 *  All states, except for the root State, should exist as children
 *  of another state.
 */

#ifndef STATE_H_
#define STATE_H_

#include <map>
#include <string>
#include <vector>
#include <list>
#include <fstream>
using namespace std;

class State
{
private:
	string name;
	map<string, State*> transitions; //<trigger, state>
	State* parent;
	map<string, State*> children; //<name, state>
	State* entryChild;
	State(string name, State* parent);
	State* transition(string trigger, State* source);

	//loading & saving functions functions
	void load(fstream& file, string flag = "");
	pair<string, string> readLine(fstream& file);
	void save(fstream& file, int tabs);
public:
	State(string name = "root");
	State(fstream& file);
	~State();
	//State(State& copy); TODO: write copy constructor

	//data
	string getName() const;
	vector<string> getHierarchy();
	string getHierarchy(int n);
	list<string> getTransitions() const;
	list<string> getImmediateTransitions() const;

	//basic state machine construction
	State* newState(string name);
	State* newState(string name, string trigger); //fails if the state or trigger already exists
	State* addTransition(State* state, string trigger); //fails if the target doesn't have the same parent, or the trigger already exists
	State* addTransition(string state, string trigger); //searches the parent for a state name. Otherwise the same as above
	State* addSelfTransition(string trigger);
	State* newLoopbackState(string name, string trigger, string returnTrigger);
	State* find(string name) const; //find a sibling of this State
	bool deleteTransition(string transition);
	bool deleteTransition(State* target);

	//parent/child controls
	State* newChild(string name);
	//State* newChild(fstream& file); TODO: write new child from file method
	vector<State*> newLinkedChildren(vector<string>& names, string trigger);
	vector<State*> newChildren(vector<string>& names);
	State* linkChildren(State* from, State* to, string trigger);
	State* linkChildren(string from, string to, string trigger);
	State* findChild(string name) const; //find within this state's direct children
	State* setEntryChild(string name);
	State* setEntryChild(State* state);
	bool deleteChild(State* state);
	bool deleteChild(string name);

	//state machine execution
	State* transition(string trigger) const;
	State* enter();

	//save/load
	void save(fstream& file);
};

#endif /* STATE_H_ */
