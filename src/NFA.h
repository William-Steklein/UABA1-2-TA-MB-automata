#ifndef TA_AUTOMATA_NFA_H
#define TA_AUTOMATA_NFA_H

#include <string>
#include <set>
#include <map>

class NFA {
private:
	struct State {
	  std::string name;
	  std::map<char, std::set<State*>> transitions;
	  bool accepting = false;
	};
	std::map<std::string, State*> states;
	State* start_state = nullptr;
	std::set<char> alphabet;

	int id;
	static int nextID;
};

#endif //TA_AUTOMATA_NFA_H
