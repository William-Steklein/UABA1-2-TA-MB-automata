#ifndef TA_AUTOMATA_AUTOMATON_H
#define TA_AUTOMATA_AUTOMATON_H

#include <string>
#include <vector>
#include <map>
#include <set>

struct State;

class Automaton {
	std::map<std::string, State*> states;
	State* start_state = nullptr;
	std::set<char> alphabet;

	int id;
	static int nextID;
};

#endif //TA_AUTOMATA_AUTOMATON_H
