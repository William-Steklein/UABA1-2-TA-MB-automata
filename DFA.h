#ifndef TA_DFA_DFA_H
#define TA_DFA_DFA_H
#include <string>
#include <vector>
#include <map>
#include <set>
#include "json.hpp"

using json = nlohmann::ordered_json;

class DFA
{
 private:
	struct State
	{
		std::string name;
		std::map<char, State*> transitions;
		bool accepting = false;
	};
	std::map<std::string, State*> states;
	State* start_state = nullptr;
	std::set<char> alphabet;

 public:
	DFA() = default;
	DFA(const std::string& json_filename);
	~DFA();

	/* Loads a DFA from a .json file */
	bool load(const std::string& filename);
	/* returns a json object containing the DFA */
	json save() const;
	/* prints the DFA in json format */
	void print() const;
	/* Clears the alphabet, states and transitions of the DFA */
	void clear();

	/* Adds a symbol to the alphabet of the DFA */
	void addSymbol(char new_symbol);
	/* Removes a symbol from the alphabet of the DFA */
	void removeSymbol(char symbol);
	/* Sets the alphabet to the characters in the string */
	void setAlphabet(const std::string& new_alphabet);
	void clearAlphabet();

	void addState(const std::string& name, bool is_accepting);
	bool removeState(const std::string& name);
	/* Sets a state as the start of the DFA, can only have one startstate */
	bool setStartState(const std::string& new_start_state_name);
	bool addTransition(const std::string& s1_name, const std::string& s2_name, char a);

	/* Checks if the given sequence of symbols ends at an accepting state */
	bool accepts(const std::string& string_w) const;

 private:
	bool isSymbolInAlphabet(char a) const;
	State* getState(const std::string& name) const;
};

#endif //TA_DFA_DFA_H
