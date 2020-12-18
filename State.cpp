/*
 * StateBase.cpp
 *
 *  Created on: Sep 30, 2010
 *      Author: nathan
 */

#include "State.h"
#include <set>
#include <vector>
#include <list>
using namespace std;

State::State(string name, State* parent):
name(name),
parent(parent),
entryChild(0)
{
}

State::State(string name):
name(name),
parent(0),
entryChild(0)
{
}

State::State(fstream& file):
parent(0),
entryChild(0)
{
	file >> name;
	load(file);
}

State::~State()
{
	map<string, State*>::iterator it;

	for(it = children.begin(); it != children.end(); it++)
		delete it->second;
}

State* State::enter()
{
	return entryChild ? entryChild->enter() : this;
}

string State::getName() const
{
	return name;
}

vector<string> State::getHierarchy()
{
	State* s = this;
	vector<string> h;
	while(s)
	{
		h.push_back(s->name);
		s = s->parent;
	}
	return h;
}

string State::getHierarchy(int n)
{
	return getHierarchy().at(n);
}

list<string> State::getTransitions() const
{
	set<string> trans;
	list<string> local;
	const State* get = this;

	while(get)
	{
		local = get->getImmediateTransitions();
		trans.insert(local.begin(), local.end());
		get = get->parent;
	}

	return list<string>(trans.begin(), trans.end());
}

list<string> State::getImmediateTransitions() const
{
	list<string> trans;
	for(map<string, State*>::const_iterator it = transitions.begin(); it != transitions.end(); it++)
		trans.push_back(it->first);

	return trans;
}

State* State::newChild(string name)
{
	if (children.find(name) == children.end())
	{
		State* child = new State(name, this);
		children[name] = child;
		if(!entryChild)
			entryChild = child;
		return child;
	}
	return 0;
}

vector<State*> State::newLinkedChildren(vector<string>& names, string trigger)
{
	vector<State*> nChildren(newChildren(names));
	for(unsigned i = 0; i < nChildren.size(); i++)
		if(nChildren[i] && nChildren[i+1])
			nChildren[i]->addTransition(nChildren[i+1], trigger);

	return nChildren;
}

vector<State*> State::newChildren(vector<string>& names)
{
	vector<State*> newChildren(names.size(), 0);
	for(unsigned i = 0; i < names.size(); i++)
		newChildren[i] = newChild(names[i]);
	return newChildren;
}

State* State::linkChildren(string from, string to, string trigger)
{
	return linkChildren(findChild(from), findChild(to), trigger);
}

State* State::linkChildren(State* from, State* to, string trigger)
{
	if(from == 0 || to == 0 || from->parent != this || to->parent != this) return 0;

	from->addTransition(to, trigger);
	return to;
}

bool State::deleteChild(State* state)
{
	if(!state || state->parent != this)
		return false;

	map<string, State*>::iterator erase;
	for(map<string, State*>::iterator it = children.begin(); it != children.end(); it++)
	{
		it->second->deleteTransition(state);
		if(it->second == state) erase = it;
	}
	if(erase == children.end())
		return false;
	if(entryChild == erase->second)
		entryChild = 0;
	children.erase(erase);
	if(entryChild == 0 && children.size() != 0)
		entryChild = children.begin()->second;
	return true;
}

bool State::deleteChild(string name)
{
	State* erase = findChild(name);
	if(!erase) return false;
	return deleteChild(erase);
}

State* State::newState(string name)
{
	return parent->newChild(name);
}

State* State::newState(string name, string trigger)
{
	return addTransition(newState(name), trigger);
}

State* State::addTransition(State* state, string trigger)
{
	if(state == 0 || state->parent != this->parent || trigger == "") return 0;

	if(transitions.find(trigger) == transitions.end())
	{
		transitions[trigger] = state;
		return state;
	}
	return 0;
}

State* State::addTransition(string state, string trigger)
{
	return addTransition(find(state), trigger);
}

bool State::deleteTransition(string transtion)
{
	map<string, State*>::iterator erase = transitions.find(transtion);
	if(erase == transitions.end()) return false;
	transitions.erase(erase);
	return true;
}

bool State::deleteTransition(State* target)
{
	map<string, State*>::iterator it;
	bool success = false;
	for(it = transitions.begin(); it != transitions.end(); it++)
		if(it->second == target)
		{
			transitions.erase(it);
			it = transitions.begin();
			success = true;
		}
	return success;
}

State* State::addSelfTransition(string trigger)
{
	return addTransition(this, trigger);
}

State* State::newLoopbackState(string name, string trigger, string returnTrigger)
{
	State* state = newState(name, trigger);
	if(state) state->addTransition(this, returnTrigger);
	return state;
}

State* State::find(string name) const
{
	return parent ? parent->findChild(name) : findChild(name);
}

State* State::findChild(string name) const
{
	map<string, State*>::const_iterator it = children.find(name);

	if(it != children.end())
		return it->second;
	return 0;
}

State* State::transition(string trigger) const
{
	map<string, State*>::const_iterator it = transitions.find(trigger);

	if(it == transitions.end())
	{
		if(parent) return parent->transition(trigger);
		else return 0;
	}
	return it->second->enter();
}

State* State::setEntryChild(string name)
{
	return setEntryChild(findChild(name));
}

State* State::setEntryChild(State* state)
{
	if(!state || state->parent != this)
		return 0;
	entryChild = state;
	return state;
}

pair<string, string> State::readLine(fstream& file)
{
	string read;
	string str;

	while(file >> read)
	{
		if(read[0] == ':' && read[read.size()-1] == ':') return make_pair(str, read);
		if(!str.empty()) str += " ";
		str += read;
	}
	return make_pair(str, "");
}

void State::load(fstream& file, string flag)
{
	pair<string, string> read;

	while(flag != ":end:")
	{
		if(flag == "")
		file >> flag;

		if(flag == ":children:")
		{
			read = readLine(file);
			while(!read.first.empty())
			{
				newChild(read.first)->load(file, read.second);
				read = readLine(file);
			}
			flag = read.second;
		}

		if(flag == ":transitions:")
		{
			string from;
			string to;
			string trigger;
			read = readLine(file);
			while(!read.first.empty())
			{
				from = read.first;
				read = readLine(file);
				to = read.first;
				read = readLine(file);
				trigger = read.first;

				linkChildren(from, to, trigger);
				read = readLine(file);
			}
			flag = read.second;
		}
	}
}

void State::save(fstream& file, int tabs)
{
	string tab(tabs, '\t');
	file << tab << getName() << " ";
	if(!children.empty())
	{
		//write child data
		file << ":children: \n";
		//write entry child first, so that it is read first
		if(entryChild)
			entryChild->save(file, tabs + 1);
		for(map<string, State*>::iterator it = children.begin(); it != children.end(); it++)
			if(it->second != entryChild)
				it->second->save(file, tabs + 1);

		/*
		 * write transition data. Normally there would be a check here
		 * to make sure there are any transitions, but that'd involve
		 * looping through every child. Because there are guaranteed
		 * to be children here, it is likely enough that there are
		 * transitions that we skip the if.
		 */
		file << tab << ":transitions: \n";
		for(map<string, State*>::iterator child = children.begin(); child != children.end(); child++) //loop through every child
			for(map<string, State*>::iterator trans = child->second->transitions.begin(); trans != child->second->transitions.end(); trans++) //loop through every transition
				file << tab << '\t' << child->first << " :to: " << trans->second->getName() << " :is: " << trans->first << " :end: \n";
		file << tab;
	}

	file << ":end: " << endl;
}

void State::save(fstream& file)
{
	save(file, 0);
}
