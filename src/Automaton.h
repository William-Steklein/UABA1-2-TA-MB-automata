#ifndef TA_AUTOMATA_AUTOMATON_H
#define TA_AUTOMATA_AUTOMATON_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <graphviz/gvc.h>
#include "json.hpp"

//using json = nlohmann::ordered_json;
using json = nlohmann::json;

class Automaton
{
	int ID;
	static int nextID;

protected:
	Automaton();
	std::set<char> alphabet;

	bool isSymbolInAlphabet(char a) const;

	/* help function of load(), opens the json document and checks if
	 * it contains "type", "alphabet", "states" and "transitions"*/
	json loadStart(const std::string& filename) const;
	/* help function of load(), loads the alphabet, states and transitions */
	bool loadBaseComponents(const json& automaton_json);

public:
	bool addSymbol(char new_symbol);
	bool removeSymbol(char symbol);
	/* Sets the alphabet to the characters in the string */
	void setAlphabet(const std::string& new_alphabet);
	void clearAlphabet();

	int getID() const;

	/* Loads an automaton from a .json file */
	virtual bool load(const std::string& filename) = 0;
	/* returns a json object containing the automaton */
	virtual json save() const = 0;
	/* prints the automaton in json format */
	void print() const;


	virtual void addState(const std::string& name, bool is_accepting) = 0;
	virtual bool removeState(const std::string& name) = 0;
	/* Sets a state as the start of the automaton, can only have one startstate */
	virtual bool setStartState(const std::string& new_start_state_name) = 0;
	virtual bool addTransition(const std::string& s1_name, const std::string& s2_name, char a) = 0;
	virtual bool removeTransition(const std::string& s1_name, char a) = 0;
	/* Checks if the given sequence of symbols ends at an accepting state */
	virtual bool accepts(const std::string& string_w) const = 0;
	/* Clears the alphabet, states and transitions */
	virtual void clear() = 0;

	/* Gives information about the automaton
	 * no_of_states
	 * no_of_transitions[symbol]
	 * degree[i] = gives number of states that has i transitions
	 * */
	virtual void printStats() const = 0;
	/* Checks if the automaton is legal */
	virtual bool checkLegality() const = 0;
	virtual std::string genDOT() const = 0;
	bool genImage() const;
};

#endif //TA_AUTOMATA_AUTOMATON_H
