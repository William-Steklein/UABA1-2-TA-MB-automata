#ifndef TA_AUTOMATA_NFA_H
#define TA_AUTOMATA_NFA_H

#include "Automaton.h"

class DFA;
class ENFA;
class RE;

class NFA : public Automaton
{
	struct State
	{
		std::string name;
		std::map<char, std::set<State*>> transitions;
		bool accepting = false;
	};
	std::map<std::string, State*> states;
	State* start_state = nullptr;

public:
	NFA();
	NFA(const std::string& json_filename);
	~NFA();

	bool load(const std::string& filename) override;
	json save() const override;

	void addState(const std::string& name, bool is_accepting) override;
	bool removeState(const std::string& name) override;
	std::string getStartState() const override;
	bool setStartState(const std::string& new_start_state_name) override;
	bool isStateAccepting(const std::string& s_name) const override;
	std::set<std::string> getAllStates() const override;
	void setStateAccepting(const std::string& s_name, bool is_accepting) const override;
	bool stateExists(const std::string& s_name) const override;
	bool addTransition(const std::string& s1_name, const std::string& s2_name, char a) override;
	/* removes all transitions from state s_name with symbol a */
	bool removeTransition(const std::string& s_name, char a) override;
	/* removes a transition */
	bool removeSingleTransition(const std::string& s1_name, const std::string& s2_name, char a);
	std::set<std::string> transitionFunction(const std::string& s_name, char a) const override;
	bool accepts(const std::string& string_w) const override;
	void clear() override;

	/* Subset construction */
	DFA toDFA() const;
	ENFA toENFA() const; // simple conversion
	RE toRE() const; // finished

	void printStats() const override;
	bool isLegal() const override;
	std::string genDOT() const override;

private:
	State* getState(const std::string& name) const;
};

#endif //TA_AUTOMATA_NFA_H
