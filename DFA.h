#ifndef TA_DFA_DFA_H
#define TA_DFA_DFA_H
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_set>

/* TODO
 *
 * thinking about how I am going to design this program
 *
 * Todot function/method for debugging automatons
 * */

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
	std::unordered_set<char> alphabet;

 public:
	DFA();
	~DFA();

	void addSymbol(char new_symbol);
	void removeSymbol(char symbol);
	void setAlphabet(std::string new_alphabet);
	void clearAlphabet();

	void addState(const std::string& name, bool is_accepting);
	bool setStartState(const std::string& new_start_state_name);
	bool addTransition(const std::string& s1_name, const std::string& s2_name, char a);

	bool accepts(const std::string& string_w) const;

 private:
	bool isSymbolInAlphabet(char a) const;
	State* getState(const std::string& name) const;
};

#endif //TA_DFA_DFA_H
