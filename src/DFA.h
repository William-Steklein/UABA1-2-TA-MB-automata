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
	std::set<std::string> getAcceptingStates() const;
	bool setStartState(const std::string& new_start_state_name) override;
	std::set<std::string> getAllStates() const override;
	bool isStateAccepting(const std::string& s_name) const override;
	void setStateAccepting(const std::string& s_name, bool is_accepting) const override;
	bool stateExists(const std::string& s_name) const override;
	bool addTransition(const std::string& s1_name, const std::string& s2_name, char a) override;
	bool removeTransition(const std::string& s1_name, char a) override;
	std::set<std::string> transitionFunction(const std::string& s_name, char a) const override;
	bool accepts(const std::string& string_w) const override;
	void clear() override;

	/* product construction */
	void product(const DFA& dfa1, const DFA& dfa2, bool intersection);
	NFA toNFA() const; // simple conversion
	ENFA toENFA() const; // simple conversion
	/* state elimination */
	RE toRE() const;

	void printStats() const override;
	bool isLegal() const override;
	std::string genDOT() const override;

private:
	State* getState(const std::string& name, bool error_output = true) const;
	std::set<std::string> getInputStates(const std::string& s_name) const;
	std::set<std::string> getOutputStates(const std::string& s_name) const;
	std::map<std::string, std::map<std::string, RE*>> transitionsToRE() const;
	static std::map<std::string, RE*> getInputTransitionsRE(const std::map<std::string,
																		  std::map<std::string, RE*>>& transitionsRE,
		const std::string& target_state);
	static RE* getLoopsRE(std::map<std::string,
											std::map<std::string, RE*>>& transitionsRE,
		const std::string& target_state);

//	RE toRERec(const std::string& current_state, const std::string& end_state, std::set<std::string>& visited_states);
//	RE toREHelp(const std::string& end_state)
};

#endif //TA_AUTOMATA_DFA_H
