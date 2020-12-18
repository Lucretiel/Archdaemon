/*
 * main.cpp
 *
 *  Created on: Oct 1, 2010
 *      Author: nathan
 */

#include "State.h"
#include "Navigator.h"
#include <iostream>
#include <fstream>
#define SAVE //change this to LOAD to have the program load the door from a file instead of save to one
using namespace std;

int main(int argc, char **argv)
{
	fstream file;
	Navigator nav;
#ifdef SAVE
	file.open("door.txt", ios::out);
	State door("door");
	door.newChild("locked")->newLoopbackState("unlocked", "unlock", "lock")->newChild("closed")->newLoopbackState("opened", "open", "close");
	door.find("locked")->newChild("closed");
#endif

#ifdef LOAD
	file.open("door.txt", ios::in);
	State door(file);
#endif

	nav.begin(door);
	while(nav.trigger != "exit")
	{
		cout << "door is " << nav.hierarchy(0) << " and " << nav.hierarchy(1) << ":";
		cin >> nav.trigger;
		nav.transition();
	}

#ifdef SAVE
	door.save(file);
	file.flush();
#endif
	return 0;
}
