#include "DFA.h"
#include "NFA.h"
#include "ENFA.h"
#include "RE.h"

ENFA::ENFA() : Automaton()
{
}

ENFA::ENFA(const std::string& json_filename) : Automaton()
{

}

ENFA::~ENFA()
{

}

bool ENFA::load(const std::string& filename)
{
	return false;
}

json ENFA::save() const
{
	return json();
}

void ENFA::addState(const std::string& name, bool is_accepting)
{

}

bool ENFA::removeState(const std::string& name)
{
	return false;
}

bool ENFA::setStartState(const std::string& new_start_state_name)
{
	return false;
}

bool ENFA::addTransition(const std::string& s1_name, const std::string& s2_name, char a)
{
	return false;
}

bool ENFA::removeTransition(const std::string& s1_name, char a)
{
	return false;
}

bool ENFA::removeSpecificTransition(const std::string& s1_name, const std::string& s2_name, char a)
{
	return false;
}

bool ENFA::accepts(const std::string& string_w) const
{
	return false;
}

void ENFA::clear()
{

}

DFA ENFA::toDFA()
{
	return DFA();
}

NFA ENFA::toNFA()
{
	return NFA();
}

RE ENFA::toRE()
{
	return toDFA().toRE();
}

bool ENFA::checkLegality() const
{
	return false;
}

std::string ENFA::genDOT() const
{
	return std::string();
}

ENFA::State* ENFA::getState(const std::string& name) const
{
	return nullptr;
}
