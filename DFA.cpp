

#include "DFA.h"

DFA::DFA()
{
	setAlphabet("01");
	addState("S0", true);
	addState("S1", false);
	addState("S2", false);

	setStartState("S0");

	addTransition("S0", "S0", '0');
	addTransition("S0", "S1", '1');

	addTransition("S1", "S2", '0');
	addTransition("S1", "S0", '1');

	addTransition("S2", "S1", '0');
	addTransition("S2", "S2", '1');
}

DFA::~DFA()
{
	for (const auto& state : states)
		delete state.second;
}

void DFA::addSymbol(char new_symbol)
{
	alphabet.insert(new_symbol);
}

void DFA::removeSymbol(char symbol)
{
	alphabet.erase(symbol);
}

void DFA::setAlphabet(std::string new_alphabet)
{
	clearAlphabet();
	for (char symbol : new_alphabet)
	{
		addSymbol(symbol);
	}
}

void DFA::clearAlphabet()
{
	alphabet.clear();
}

void DFA::addState(const std::string& name, bool is_accepting)
{
	State* new_state = new State;
	new_state->name = name;
	new_state->accepting = is_accepting;
	states[name] = new_state;
}

bool DFA::setStartState(const std::string& new_start_state_name)
{
	State* new_start_state = getState(new_start_state_name);
	if (!new_start_state)
		return false;

	start_state = new_start_state;
	return true;
}

bool DFA::addTransition(const std::string& s1_name, const std::string& s2_name, char a)
{
	if (!isSymbolInAlphabet(a))
		return false;

	State* s1 = getState(s1_name);
	State* s2 = getState(s2_name);
	if (!s1 || !s2)
		return false;

	if (s1->transitions.find(a) != s1->transitions.end())
	{
		std::cout << "Info: changing transition δ(" << s1->name << ", " << a << ")" << std::endl;
	}

	s1->transitions[a] = s2;

	return true;
}

bool DFA::accepts(const std::string& string_w) const
{
	if (!start_state)
	{
		std::cerr << "Error: no start state" << std::endl;
		return false;
	}

	State* current_state = start_state;

	for (char a : string_w)
	{
		if (!isSymbolInAlphabet(a))
			return false;

		auto it = current_state->transitions.find(a);

		// check if there is a transition
		if (it == current_state->transitions.end())
		{
			std::cerr << "Error: δ(" << current_state->name << ", " << a << ") has no output" << std::endl;
			return false;
		}

		current_state = it->second;
	}

	if (current_state->accepting)
		return true;
	else
		return false;
}

bool DFA::isSymbolInAlphabet(char a) const
{
	if (alphabet.find(a) != alphabet.end())
		return true;
	std::cerr << "Error: given symbol is not in alphabet" << std::endl;
	return false;
}

DFA::State* DFA::getState(const std::string& name) const
{
	auto state = states.find(name);
	if (state != states.end())
		return state->second;

	std::cerr << "Error: couldn't find state with name \"" << name << "\"" << std::endl;
	return nullptr;
}
