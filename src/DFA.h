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
	~DFA();

	bool load(const std::string& filename) override;
	json save() const override;
	void print() const override;
	void clear() override;

	void addState(const std::string& name, bool is_accepting) override;
	bool removeState(const std::string& name) override;
	bool setStartState(const std::string& new_start_state_name) override;
	bool addTransition(const std::string& s1_name, const std::string& s2_name, char a) override;
	bool removeTransition(const std::string& s1_name, char a) override;
	bool accepts(const std::string& string_w) const override;

	NFA toNFA(); // simple conversion
	ENFA toENFA(); // simple conversion
	/* state elimination */
	RE toRE();

	bool checkLegality() const override;
	std::string genDOT() const override;

private:
	State* getState(const std::string& name) const;
};

#endif //TA_AUTOMATA_DFA_H