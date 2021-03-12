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

public:
	void addSymbol(char new_symbol);
	void removeSymbol(char symbol);
	/* Sets the alphabet to the characters in the string */
	void setAlphabet(const std::string& new_alphabet);
	void clearAlphabet();

	int getID() const;

	/* Loads a DFA from a .json file */
	virtual bool load(const std::string& filename) = 0;
	/* returns a json object containing the DFA */
	virtual json save() const = 0;
	/* prints the DFA in json format */
	virtual void print() const = 0;
	/* Clears the alphabet, states and transitions of the DFA */
	virtual void clear() = 0;

	virtual void addState(const std::string& name, bool is_accepting) = 0;
	virtual bool removeState(const std::string& name) = 0;
	/* Sets a state as the start of the automaton, can only have one startstate */
	virtual bool setStartState(const std::string& new_start_state_name) = 0;
	virtual bool addTransition(const std::string& s1_name, const std::string& s2_name, char a) = 0;
	virtual bool removeTransition(const std::string& s1_name, char a) = 0;
	/* Checks if the given sequence of symbols ends at an accepting state */
	virtual bool accepts(const std::string& string_w) const = 0;

	/* Checks if the automaton is legal */
	virtual bool checkLegality() const = 0;
	virtual std::string genDOT() const = 0;
	bool genImage() const;
};

#endif //TA_AUTOMATA_AUTOMATON_H
