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
	void print() const override;
	void clear() override;

	void addState(const std::string& name, bool is_accepting) override;
	bool removeState(const std::string& name) override;
	bool setStartState(const std::string& new_start_state_name) override;
	bool addTransition(const std::string& s1_name, const std::string& s2_name, char a) override;
	bool removeTransition(const std::string& s1_name, char a) override;
	bool accepts(const std::string& string_w) const override;

	/* Subset construction */
	DFA toDFA();
	ENFA toENFA(); // simple conversion
	RE toRE(); // finished

	bool checkLegality() const override;
	std::string genDOT() const override;

private:
	State* getState(const std::string& name) const;
};

#endif //TA_AUTOMATA_NFA_H
