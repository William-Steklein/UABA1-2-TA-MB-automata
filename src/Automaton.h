#ifndef UABA1_TA_AUTOMATA2__AUTOMATON_H
#define UABA1_TA_AUTOMATA2__AUTOMATON_H

#include "Alphabet.h"

using json = nlohmann::json;

class DFA;
class NFA;
class ENFA;
class RE;

class Automaton : public Alphabet
{
	struct State
	{
		std::string name;
		std::map<char, std::set<State*>> transitions;
		bool accepting = false;
	};
	std::map<std::string, State*> states;
	State* start_state = nullptr;

protected:
	Automaton() = default;
	~Automaton();
	Automaton& operator=(const Automaton& _automaton);

protected:
	static json loadJsonFile(const std::string& filename);
	/* Loads an automaton from a .json file */
	virtual bool loadBasicComponents(const std::string& filename, const json& automaton_json);
	/* returns a json object containing the automaton */
	virtual json saveBasicComponents() const;
	virtual json save() const = 0;

public:
	/* prints the automaton in json format */
	void print(std::ostream& output_stream = std::cout) const;

	void addState(const std::string& name, bool is_accepting);
	bool removeState(const std::string& s_str);
	std::string getStartState() const;
	/* Sets a state as the start of the automaton, can only have one startstate */
	bool setStartState(const std::string& start_state_str);
	bool isStateAccepting(const std::string& s_str) const;
	void setStateAccepting(const std::string& s_str, bool is_accepting) const;
	std::set<std::string> getAllStates() const;
	bool isSetOfStatesAccepting(const std::set<std::string>& set_of_states) const;
	static std::string getSetOfStatesString(const std::set<std::string>& set_of_states);
	bool stateExists(const std::string& s_str) const;

	virtual bool addTransition(const std::string& s1_str, const std::string& s2_str, char a);
	virtual bool addTransitions(const std::string& s_str, const std::set<std::string>& output_states, char a);
	bool removeTransition(const std::string& s1_str, const std::string& s2_str, char a);
	bool removeTransitions(const std::string& s_str, char a);
	std::set<std::string> transition(const std::string& s_str, char a) const;
	std::set<std::string> transitionSetOfStates(const std::set<std::string>& set_of_states, char a) const;
	std::set<std::string> reverseTransition(const std::string& s_str, char a) const;
	std::set<std::string> reverseTransitionSetOfStates(const std::set<std::string>& set_of_states, char a) const;
	std::set<std::string> eClosure(const std::set<std::string>& set_of_states) const;
	/* Checks if the given sequence of symbols ends at an accepting state */
	bool accepts(const std::string& string_w) const;

	/* Clears the alphabet, states and transitions */
	void clear();
	/**
	 * Gives information about the automaton
	 * - no_of_states
	 * - no_of_transitions[symbol]
	 * - degree[i] = gives number of states that has i transitions
	 */
	virtual void printStats() const;
	/* Checks if the automaton is legal */
	virtual bool isLegal() const = 0;
	void renameStates(bool letters = true);
	std::string genDOT() const;
	bool genImage() const;

private:
	State* getState(const std::string& name, bool error_output = false) const;
};

#endif
