#ifndef TA_AUTOMATA_DFA_H
#define TA_AUTOMATA_DFA_H

#include "Automaton.h"

class NFA;
class ENFA;
class RE;

class DFA : public Automaton
{
	struct State
	{
		std::string name;
		std::map<char, State*> transitions;
		bool accepting = false;
	};
	std::map<std::string, State*> states;
	State* start_state = nullptr;

public:
	DFA();
	DFA(const std::string& json_filename);
	DFA(const DFA& dfa1, const DFA& dfa2, bool intersection);
	~DFA();

	bool load(const std::string& filename) override;
	json save() const override;

	void addState(const std::string& name, bool is_accepting) override;
	bool removeState(const std::string& name) override;
	std::string getStartState() const override;
	bool setStartState(const std::string& new_start_state_name) override;
	bool isStateAccepting(const std::string& s_name) const override;
	bool stateExists(const std::string& s_name) const override;
	bool addTransition(const std::string& s1_name, const std::string& s2_name, char a) override;
	bool removeTransition(const std::string& s1_name, char a) override;
	std::set<std::string> transitionFunction(const std::string& s_name, char a) const override;
	bool accepts(const std::string& string_w) const override;
	void clear() override;

	/* product construction */
	void product(const DFA& dfa1, const DFA& dfa2, bool intersection);
	NFA toNFA(); // simple conversion
	ENFA toENFA(); // simple conversion
	/* state elimination */
	RE toRE();

	void printStats() const override;
	bool isLegal() const override;
	std::string genDOT() const override;

private:
	State* getState(const std::string& name, bool error_output = true) const;
};

#endif //TA_AUTOMATA_DFA_H
