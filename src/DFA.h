#ifndef UABA1_TA_AUTOMATA2__DFA_H
#define UABA1_TA_AUTOMATA2__DFA_H

#include "Automaton.h"

class DFA : public Automaton
{
	bool load(const std::string& filename);
	json save() const override;

public:
	DFA() = default;
	DFA(const std::string& json_filename);
	DFA(const DFA& dfa1, const DFA& dfa2, bool intersection);


	bool addTransition(const std::string& s1_str, const std::string& s2_str, char a) override;
	bool addTransitions(const std::string& s_str, const std::set<std::string>& output_states, char a);

	NFA toNFA() const;
	ENFA toENFA() const;
	RE toRE() const;

	void product(const DFA& dfa1, const DFA& dfa2, bool intersection);

	bool isLegal() const override;

 private:
	std::map<std::string, std::map<std::string, RE*>> transitionsToRE() const;
	static std::map<std::string, RE*> getInputTransitionsRE(const std::map<std::string,
		std::map<std::string, RE*>>& transitionsRE,
		const std::string& target_state);
	static RE* getLoopsRE(std::map<std::string,
		std::map<std::string, RE*>>& transitionsRE,
		const std::string& target_state);
};

#endif
