#include <fstream>
#include <iostream>
#include "DFA.h"
#include "json.hpp"

using json = nlohmann::json;

DFA::DFA()
{
	load("DFA.json");
}

DFA::~DFA()
{
	for (const auto& state : states)
		delete state.second;
}

bool DFA::load(const std::string& filename)
{
	// file input
	std::ifstream input(filename);

	if (!input.good())
	{
		std::cerr << "Error: couldn't open " << filename << std::endl;
		return false;
	}

	json DFSjson;
	input >> DFSjson;

	if (!(DFSjson.contains("type") && DFSjson.contains("alphabet")
		  && DFSjson.contains("states") && DFSjson.contains("transitions")))
	{
		std::cerr << "Error: " << filename << " has an invalid format" << std::endl;
		return false;
	}

	if (DFSjson["type"] != "DFA")
	{
		std::cerr << "Error: " << filename << " is of the type " << DFSjson["type"]
				  << " and not DFA" << std::endl;
		return false;
	}

	for (const auto& symbol : DFSjson["alphabet"])
	{
		addSymbol(symbol.get<std::string>()[0]);
	}

	for (const auto& state : DFSjson["states"])
	{
		addState(state["name"].get<std::string>(), state["accepting"].get<bool>());
		if (state["starting"].get<bool>())
			setStartState(state["name"].get<std::string>());
	}

	for (const auto& transition : DFSjson["transitions"])
	{
		addTransition(transition["from"].get<std::string>(), transition["to"].get<std::string>()
		    , transition["input"].get<std::string>()[0]);
	}

	return true;
}

void DFA::clear()
{
	// alphabet
	clearAlphabet();

	// states
	for (const auto& state : states)
		delete state.second;

	states.clear();
	start_state = nullptr;
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

bool DFA::removeState(const std::string& name)
{
	State* state = getState(name);
	if (!state)
		return false;

	if (state == start_state)
		start_state = nullptr;

	states.erase(name);
	delete state;

	return true;
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
		std::cout << "Changing transition δ(" << s1->name << ", " << a << ")" << std::endl;
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
	std::cerr << "Error: symbol " << a << " is not in alphabet" << std::endl;
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
