# Archdaemon Finite State Machine Engine

“I apologize profusely for any typos or editorial mistakes”

## Basic use

The easiest way to use the finite state machine is with the `StateMachine` class. This provides all the basic functionality as specified by the state machine assignment, but none of the advanced functionality of the base `State` class.

Instantiating a `StateMachine` object will create an empty state machine:

```cpp
StateMachine light; //an empty state machine
```

Alternatively, you can read in an `std::fstream` file with a premade state machine:

```cpp
fstream file(“light.txt”);
Statemachine light(file); //load the state machine from file
```

Note that after the State Machine is created, you cannot load it from a file. You can only edit it with the built in construction methods.

Once the state machine is created, you can begin adding states and transitions to it:

```cpp
light.addState(“Light is off”);
light.addState(“Light is on”);

//make “turn on” as a transition from “Light is off” to “Light is on”
light.addTransition(“Light is off”, “Light is on”, “turn on”);

//and vice versa
light.addTransition(“Light is on”, “Light is off”, “turn off”);
```

Note that the first state you create will be marked as the initial state.

After the state machine is created, you can begin navigating through it. First, set the initial state:

```cpp
light.setState(“Light is off”); //begin navigation
You can then use transition(string) to move through the machine, and currentState() to view the current state:
cout << light.currentState() << endl;
light.transition(“turn on”);
cout << light.currentState() << endl;
light.transition(“turn off”);
cout << light.currentState() << endl;
```

```
Light is off
Light is on
Light is off
```

As you can see, states and transitions are represented and invoked by their names. Note that `transition(...)` is argument-safe; if an invalid or nonexistent string is passed in, the state will not change. Also not that states are unique: if you try to create a state of the same name of one that already exists, the operation will fail.

After the state machine is created, you can save it to a file which can be loaded later:

```cpp
light.save(file);
```

The state machine files are designed to be human readable and editable; see section 3 for details on the syntax of the files.

## Advanced Use (Skip if programming scares you)

In order to use the more advanced features of Archdaemon, you need to make use of the `State` and `Navigator` classes. The `State` class provides all of the functionality of states and state machines, while the `Navigator` class provides a way to navigate through it.

Internally, all states in the Archdaemon exist as children of another state (except the root state, which is the state machine itself). Upon entering a state, the navigator will immediately enter that state's entry child, if it has one. In this way, more elaborate state machines can be constructed and organized to make their use much easier. In turn, the `Navigator` objects are little more than pointers to a state, while the states themselves handle transitions.

The example here shows the Door state machine demonstrated in the included door.cpp, with the exception that the “locked” state will have no “closed” child.

Similarly to the `StateMachine` class, you either can create an empty state machine or load one from a file. Remember that all states, even the base state, have names:

```cpp
State light(file); //not part of Door example.
State door(“door”);
```

Once the `State` is created, you can add children to it. Again, the first state added is the entry state. However, you can change the entry state with `setEntryChild(...)`

```cpp
door.newChild(“unlocked”);
door.newChild(“locked”);
door.setEntryChild(“locked”);
```

You can then add transitions between a state's children. The arguments are the same as in `StateMachine`: (from state, to state, transition name):

```cpp
door.linkChildren(“locked”, “unlocked”, “unlock”);
```

You can use `findChild` to get a pointer to a child, which you can then use to construct the state machine around that child:

```cpp
door.findChild(“unlocked)->addTransition(“locked”, “lock”);
```

Many of the functions besides `findChild` return pointers to states, which can be used to chain construction functions together, without creating a lot of temporary objects (See section 4 for details on exactly what state pointers are returned by the various member functions).

```cpp
door.findChild(“unlocked”)
    ->newChild(“closed”)
    ->newLoopbackState(“opened”, “open”, “close”);
```

This adds closed as a child of unlocked, and opened as a sibling of closed (and thus also a child of unlocked). `newLoopbackState` creates a cycle state; the second argument transitions to it, while the third argument transitions back from the new state to the current one. In this example, it means that closed will transition to opened with open, while opened will transition back to closed with.

Once the state machine is created, use a Navigator object to navigate through it:

```cpp
Navigator nav;
nav.begin(door); //current state is locked
You can then move through the state machine, and view information about the current state:
nav.transition(“unlock”); //current state is closed and unlocked
nav.trigger = “open”; /_ Navigator provides a string to store the next transition_/
nav.transition() //uses the stored transition.
nav.hierarchy(); //{“opened” “unlocked” “door”}
nav.transitions(); //{“close” “lock”}
nav.reset(); //no current state
nav.begin(door); //current state is closed, again
nav.hierarchy(); //{“locked” “door”}
```

Both Navigator and State have other member functions not explained here; see section 4 for details on their use.

## Save File Formatting

The save file format is designed to be human readable and relatively flexible; it allows for state and transition names of virtually any length. The files are divided by flags, which are simply strings that start and end with a colon, such as `:children:` and `:transitions:`. The files are whitespace insensitive; only the position of the flags influences the final state machine. This example shows the Door state machine demonstrated in the included `door.cpp`.

To begin, define your initial State, by typing the name, followed by the `:end:` flag:

```
door :end:
```

All children states are defined within the children block of their parents, indicated by the `:children:` flag.

```
door :children:
    [children defined here]
:end:
```

In this example, the children of door are locked and unlocked (we will add opened and closed later). Add them the same way you added the initial door state:

```
door :children:
    locked :end:
    unlocked :end:
:end:
```

All transitions are defined only after the children have been defined, and are indicated by the `:transitions:` flag.

```
door :children:
    locked :end:
    unlocked :end:
:transitions:
    [transitions defined here]
:end:
```

Each individual transition is defined with the following syntax:

```
[initial state name] :to: [final state name] :is: [transition name] :end:
```

This adds a transition from the initial transition to the final transition, with the given name. So, in this example, we need to be able to lock and unlock the door:

```
door :children:
    locked :end:
    unlocked :end:
:transitions:
    locked :to: unlocked :is: unlock :end:
    unlocked :to: locked :is: lock :end:
:end:
```

This could serve as a complete state machine. However, we need to give the door opened and closed states. Because the door can be opened and closed when it is unlocked, we will add these as children of unlocked:

```
door :children:
    locked :end:
    unlocked :children:
        closed :end:
        opened :end:
    :end:
:transitions:
    locked :to: unlocked :is: unlock :end:
    unlocked :to: locked :is: lock :end:
:end:
```

Notice how they are defined within unlocked, just as unlocked is defined within door. We will define the transitions between them within the unlocked state as well:

```
door :children:
    locked :end:
    unlocked :children:
        closed :end:
        opened :end:
    :transitions:
        closed :to: opened :is: open :end:
        opened :to: closed :is: close :end:
    :end:
:transitions:
    locked :to: unlocked :is: unlock :end:
    unlocked :to: locked :is: lock :end:
:end:
```

For completeness' sake, we will also add closed as the only child of locked:

```
door :children:
    locked :children:
        closed :end:
    :end:
    unlocked :children:
        closed :end:
        opened :end:
    :transitions:
        closed :to: opened :is: open :end:
        opened :to: closed :is: close :end:
    :end:
:transitions:
    locked :to: unlocked :is: unlock :end:
    unlocked :to: locked :is: lock :end:
:end:
```

Note the order of the child states; the first child is always the entry child of its parent, or the state that will automatically be entered when the state itself is entered. So locked is the entry child of door, and closed is the entry child of unlocked.

We now have a complete state machine. When it is first executed, it will be a closed, locked door. Using the unlock transition will cause it to become closed and unlocked. Using the open transition afterwards will cause it to become an opened and unlocked. Observe what happens when we lock an open door: It will check and see that opened has no transition called lock. It will then check opened's parent, unlocked. It sees that unlocked has lock as a transition, and executes it, causing the door to become locked and closed again. If it had not found the transition, it would go to the next parent up, and all the way up until the transition is found or the root is reached.

While this is not a perfect state machine, it serves as an excellent example of how to effectively use parent-child relationships. Keep in mind as well that you are not limited to one-word states and transitions; you can use and number of words and characters, the one limit being that any string that starts and ends with a colon will be marked as a flag, even if it isn't one of the flags mentioned above. Also keep in mind that the formatting used above is not at all required; the only thing seen when loading the file is the flags and the whitespace between them.

BE WARNED: The current implementation does not check for validity of the file, and will attempt to load it as-is. Double check to make sure the file is correct before loading it.

## Reference

### `State.h`

`State` is the main class for states and state machines. Each state can have child states, and in fact the state machine is just a state with no parent. Each state has a name in the form of a string, but the machine's state must be a base state, or state with no children. Each state also has a list of transitions to other states, also defined by strings. States can only have transitions to their siblings; however, during the execution the navigator works its way from the base state up to the root state when looking for transitions.

#### Constructor/Destructor

```cpp
State::State(std::string name = “root”)
State::State(std::fstream& file)
```

First Version: Creates a childless, parentless state with the given name.

Second Version: Builds a state machine out of the given file. file must have the `ios::in` flag.

```cpp
State::~State()
```

Destroys a state and all its children. WARNING: States are designed to be managed by their parent state. You should NEVER call delete on a pointer to a child state; this will cause wild pointers within the structure. Use `State::deleteChild` to delete a child state.

#### State Information

These member functions allow the user to view information about the current state of the state machine. It's generally advisable to use a `Navigator` object to look up this information in runtime, instead of calling these functions directly.

```cpp
std::string State::getName()
```

Returns the current state's name.

```cpp
std::vector<std::string> State::getHierarchy()
std::string State::getHierarchy(int n)
```

First Version: returns a vector of strings corresponding to the current branch of the state machine, starting at the current state and going back up to the root. So, index `0` is the current state, index `1` is its parent, and index `(size-1)` is the root of the state machine.

Second Version: returns the name of the nth state in the hierarchy. So, `n=0` is equivalent to `getName()`

```cpp
std::list<std::string> State::getTransitions()
```

Returns a list of all the transitions this state can make. This includes all the transitions that can be made from this state's parents.

```cpp
std::list<std::string> State::getImmediateTransitions()
```

Returns a list of only this state's transitions; it excludes the transitions that can be made from this state's parents.

#### Parent/ Child Controls

These functions relate to the parent-child relationships between states. Most of the state machine's internal management takes place at this level. Most of these functions return `State*` objects; this is to allow these methods to be chained together when building state machines. In addition, most of these have 2 versions: one with a pointer to a state, and one using the state's name; They are generally identical except that the versions with the name incur a slight overhead to look up the name.

```cpp
State* newChild(std::string name)
```

Attempts to create a new child with the given name. Returns a pointer to the new State if successful, 0 otherwise. Each child of a state must have a unique name; attempting to create a new child with the same name as another child will result in a failure. If this is the first created child, it is marked as this state's entry child, which is the child that is immediately entered when this state is entered during execution.

```cpp
std::vector<State*> newChildren(std::vector<std::string>& names)
```

Attempts to create new children with the names provided in the vector. Returns a vector of associated pointers to the new states.

```cpp
std::vector<State*> newLinkedChildren(std::vector<std::string>& names, std::string trigger)
```

Attempts to create new children with the names in the vector, where ea ch state transitions to the next one in the sequence with the given trigger. Returns a vector of associated pointers to the new states.

```cpp
State* linkChildren(State* from, State* to, std::string trigger)
State* linkChildren(std::string from, std::string second, std::string trigger)
```

First version: attempts to add a transition from the first state to the second, with the given trigger. The 2 states must be direct children of this state. Returns a pointer to the second state if successful, or 0 otherwise.

Second version: same as the first, but takes in state names as arguments instead of pointers. Takes slightly longer than the first version as it has to look up the states.

```cpp
State* findChild(std::string name)
```

Returns a pointer to the child with the provided name. Returns 0 if no such child exists. Only searches direct children, as subchildren may have the same name as other children.

```cpp
State* setEntryChild(State* state)
State* setEntryChild(std::string name)
```

First version: sets this State's entry child to be the given child. The entry child is the child that is immediately entered when this state is entered during the execution of the state machine. Returns a pointer to that state, or 0 if it fails.

Second version: same as the first, but takes in a state name as the argument instead of a pointer to the state.

```cpp
bool deleteChild(State* state)
bool deleteChild(std::string name)
```

First version: deletes the child and all of its children. This also removes all transitions to the deleted child. Returns true if the child was deleted, false otherwise. If the deleted child was the entry child, the entry child is set to an arbitrary child; it is recommended that setEntryChild is called afterward to prevent unexpected behavior. In addition, this method has no bearing on Navigator objects that are using this state machine; do not call it when the state machine is executing, unless you are sure that the Navigator is not in this state or any of its children. Use Navigator::reset if unsure.

Second versions: same as the first, but takes in a state name.

#### Sibling Controls

These methods all relate to manipulating and accessing siblings of the current state. Most of these are internally implemented through calls to Parent/Child methods; the exception is addTransition(...). Again, most of the methods return pointers to states, so that the methods can be chained together with minimal temporary objects.

```cpp
State* newState(std::string name)
State* newState(std::string name, std::string trigger)
```

First version: Attempts to create a new state as a sibling to this one. Returns a pointer to the new state, or 0 if the operation fails.

Second version: Same as above, but also adds a transition from this state to the new one. Returns a pointer to the new state, or 0 if the operation fails. If the new state is created successfully but the transition is not, the method will still return 0.

```cpp
State* addTransition(State* state, std::string name)
State* addTransition(std::string name, std::string trigger)
```

First version: attempts to add a transition from this state to the given one, using the given trigger. Returns a pointer to the target state, or 0 if the operation fails. Each state can only have one transition of a given name, but it can have multiple transitions to the same target state.

Second version: Same as above, but using the name of the state instead of a pointer.

```cpp
State\* addSelfTransition(std::string trigger)
```

Attempts to add a transition from this state back to itself. Returns a pointer to this state, or 0 if the operation fails. Equivalent to a call to addTranstion(this, trigger);

```cpp
State* newLoopbackState(std::string name, std::string trigger, std::string returnTrigger)
```

Attempts to create a new state; if successful, adds a transition from this state to the new one using trigger and a transition from the new state to this one using returnTrigger. Returns a pointer to the new state, or 0 if any of the operations fail. If any of the operations fail, the state will still be created.

```cpp
State* find(std::string name)
```

Returns a pointer to the sibling with the provided name, or 0 if no state is found. If the state doesn't have a parent, it instead finds the child with the provided name. This function can return a pointer to the state it was called from, as it searches all the children of the parent.

```cpp
bool deleteTransition(std::string transition)
bool deleteTransition(State* target)
```

First version: deletes the transition with the given name. Has no affect on the state pointed to by the transition. Returns true if successful or false if failed.

Second version: deletes all transitions to the target state. Otherwise identical to the first version. Be careful not to confuse this with the first version: the first version reads in the name of a transition, not the name of a state that is being transitioned to.

#### Execution Controls

These methods allow execution of a state machine. They automatically handle entering and exiting child states.

```cpp
State* enter()
```

Enters the state, by descending the entry children from this state and returning a pointer to the bottommost child. Returns a pointer to itself if it has no children.

```cpp
State* transition(std::string trigger)
```

Returns a pointer to the result of executing the provided transition. This is achieved in two parts. First, the transition is found: if this state does not have a transition with the given name, its parent is searched, and so on back up to the root or until the transition is found. Second, the targeted state is entered by calling its enter method. If no valid transition is found, it returns 0.

#### Save/Load

These methods allow you to save and load to a state machine to a text file (See section 3). Note that once the machine has been created, it can only be modified by code; you must use the constructor to load from a file.

```cpp
void save(std::fstream& file)
```

Writes current state machine to a save file. The save file is designed to be human-readable; see section 3 for details on the syntax. file must have the `ios::out` flag.

### `Navigator.h`

The Navigator class is responsible for navigating through a state machine. It mainly acts as a wrapper for a pointer to a state, but the methods ensure that the pointer is handled safely. None of its methods modify an already existing state machine, although getState returns a pointer to a state which can then be used to modify the state.

#### Constructor/Destructor

```cpp
Navigator(State* initialState = 0)
Navigator(State& initialState)
```

Creates a navigator with the initial state given. This does not actually enter the state machine; it only adds the pointer to the root state; this is to allow a Navigator to be created before the state machine is constructed. Use begin to actually begin navigation.

```cpp
~Navigator()
```

Destroys the navigator. Has no affect on the state it currently references.

#### State Machine Execution

These functions are how the navigator moves through the state machine. The return values, instead of being `State*` objects as in State objects, are generally booleans that indicate whether the operation was successful or not.

```cpp
std::string trigger
```

A string object provided by the class that indicates the next transition. If transition is called with no arguments, this string is used as the trigger.

```cpp
void reset()
```

Restores the navigator to an initial state by setting the internal pointer to 0 and the public string to an empty string. Does not preserve any of the information about the state before the reset.

```cpp
void begin()
void begin(State\* root)
void begin(State& root)
```

First version: Begins execution of the state machine by entering the current state. Should be called before any other execution of the state machine. Invalid immediately after a call to reset as none of the information is preserved; use the second or third versions instead.

Second version: Same as the first, but begins by entering the given state. If the given state is not a root state, the navigator will enter it as though it was, but will still be able to navigate the entire state machine.

Third version: Same as the first, but begins by entering the given state.

```cpp
bool transition()
bool transition(std::string trigger)
int transition(std::list<std::string>& triggers)
```

First version: Executes the transition referred to by the public string trigger. Returns true if a transition was performed, false otherwise. trigger is not affected by the call. If the transition was unsuccessful, the current state remains the same.

Second version: Executes the transition referred to by the string in the argument. Returns true if the transition was successful, false otherwise. If the transition fails, the current state remains the same.
Third version: Executes, in order, the transitions provided by the list. Returns an int representing the number of successful transitions. Immediately stops when a transition fails, but does not undo the previous transitions if there is a failure. Doesn't affect the contents of triggers.

#### State Information

These methods give access about the current state. They all reflect calls to their equivalent state information method of the current state (see State Information in section 4.1), so only the equivalent methods are listed here.

```cpp
std::string state() const
```

Equivalent to a call to getName().

```cpp
std::vector<std::string> hierarchy() const
std::string hierarchy(int n) const
```

First version: Equivalent to a call to `getHierarchy()`.

Second version: equivalent to a call to `getHierarchy(n)`.

```cpp
std::list<std::string> transitions() const
```

Equivalent to a call to `getTransitions()`.

```cpp
State* getState() const
```

Returns a pointer to the current state.

### `StateMachine.h`

The StateMachine class is designed to act as a simple interface for users who don't want to learn or use the advanced features of the normal `State` and `Navigator` classes. It maintains internally a `State` object as the State Machine and a `Navigator` object to handle navigating through it. The main loss of functionality is the loss of child states; all states in the state machine handled by the StateMachine class are children of the root state.

#### Constructor/Destructor

```cpp
StateMachine()
StateMachine(std::fstream& file)
```

First version: Creates an empty state machine.

Second version: Loads a state machine from a file. The file does not have conform to State Machines that can be created in the state machine class; any state machine will be loaded normally. Execution of a State machine with child states will proceed normally, but it is impossible to modify construction of states that aren't direct children of the root state. This does not initialize the navigator.

```cpp
~StateMachine()
```

Destroys the state machine.

#### Construction

These methods allow for construction and modification of the state machine. None of them affect the Navigator, with a minor exception in `deleteState`.

```cpp
bool addState( std::string name)
```

Creates a new state with the given name. Returns `true` if a state was created, `false` if a state with that name already existed.

```cpp
bool deleteState(std::string name)
```

Deletes the state with the given name. Returns `true` if a state was deleted, `false` otherwise. If the Navigator object is in the deleted state, it will be reset and must be restarted with `setState`.

```cpp
bool addTransition(std::string from, std::string to, std::string trigger)
```

Adds a transition from the state from to the state to with the name trigger. Returns `true` if the trigger was created, `false` otherwise. This operation will fail if either of the sates do not exist or if the state from already has a transition trigger.

```cpp
bool deleteTransition(std::string name, std::string trigger)
```

Deletes a transition from name with the name trigger. Returns true if a trigger was deleted, false otherwise.

#### Execution

These methods control the `Navigator` object's navigation of the state machine and the info relating to the current state. None of these methods affect the state machine's construction.

```cpp
bool setState(std::string name)
```

Begins the `Navigator` at the state referred to by name. Returns `true` if the `Navigator` was successfully set to the target state, `false` otherwise

```cpp
bool transition(std::string trigger)
int transition(std::list<std::string> triggers)
```

First version: attempts to transition using the given trigger. Returns true if the transition was successful, false otherwise. If the transition fails the Navigator remains in the previous state.

Second version: attempts to perform the transitions in the list, in the order provided. Stops transitioning as soon as a transition is unsuccessful. Returns the number of successful transitions.

```cpp
std::string currentState()
```

Returns the name of the current state.

#### Save/Load

These allow you to save to a file. The formatting is identical to the normal save file.

```cpp
void save(fstream& file)
```

Saves the state machine to the file. File must have the ios::out flag.

“I do not, however, for the complexity of this code”
