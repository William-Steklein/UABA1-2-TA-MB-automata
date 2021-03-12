#include "DFA.h"
#include "NFA.h"
#include "ENFA.h"
#include "RE.h"

NFA::NFA() : Automaton()
{
}

NFA::NFA(const std::string& json_filename) : Automaton()
{
	load(json_filename);
}

NFA::~NFA()
{
	for (const auto& state : states)
		delete state.second;
}

bool NFA::load(const std::string& filename)
{
	// file input
	std::ifstream input(filename);

	if (!input.good())
	{
		std::cerr << "Error: couldn't open " << filename << std::endl;
		return false;
	}

	json NFA_json;
	input >> NFA_json;

	if (!(NFA_json.contains("type") && NFA_json.contains("alphabet")
		&& NFA_json.contains("states") && NFA_json.contains("transitions")))
	{
		std::cerr << "Error: " << filename << " has an invalid format" << std::endl;
		return false;
	}

	if (NFA_json["type"] != "NFA")
	{
		std::cerr << "Error: " << filename << " is of the type " << NFA_json["type"]
				  << " and not NFA" << std::endl;
		return false;
	}

	for (const auto& symbol : NFA_json["alphabet"])
	{
		addSymbol(symbol.get<std::string>()[0]);
	}

	for (const auto& state : NFA_json["states"])
	{
		addState(state["name"].get<std::string>(), state["accepting"].get<bool>());
		if (state["starting"].get<bool>())
			setStartState(state["name"].get<std::string>());
	}

	for (const auto& transition : NFA_json["transitions"])
	{
		addTransition(transition["from"].get<std::string>(), transition["to"].get<std::string>(),
			transition["input"].get<std::string>()[0]);
	}

	checkLegality();

	return true;
}

json NFA::save() const
{
	return json();
}

void NFA::print() const
{

}

void NFA::clear()
{

}

void NFA::addState(const std::string& name, bool is_accepting)
{

}

bool NFA::removeState(const std::string& name)
{
	return false;
}

bool NFA::setStartState(const std::string& new_start_state_name)
{
	return false;
}

bool NFA::addTransition(const std::string& s1_name, const std::string& s2_name, char a)
{
	return false;
}

bool NFA::removeTransition(const std::string& s1_name, char a)
{
	return false;
}

bool NFA::accepts(const std::string& string_w) const
{
	return false;
}

bool NFA::checkLegality() const
{
	return false;
}

std::string NFA::genDOT() const
{
	return std::string();
}

NFA::State* NFA::getState(const std::string& name) const
{
	return nullptr;
}

DFA NFA::toDFA()
{
	return DFA();
}

ENFA NFA::toENFA()
{
	return ENFA();
}

RE NFA::toRE()
{
	return toDFA().toRE();
}
