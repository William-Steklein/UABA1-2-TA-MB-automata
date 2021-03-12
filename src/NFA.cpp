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
	json NFA_json = loadStart(filename);

	if (NFA_json["type"] != "NFA")
	{
		std::cerr << "Error: " << filename << " is of the type " << NFA_json["type"]
				  << " and not NFA" << std::endl;
		return false;
	}

	loadBaseComponents(NFA_json);

	checkLegality();

	return true;
}

json NFA::save() const
{
	json NFA_json;

	NFA_json["type"] = "NFA";

	NFA_json["alphabet"] = json::array();
	for (auto symbol : alphabet)
		NFA_json["alphabet"].push_back(std::string(1, symbol));

	for (const auto& state : states)
	{
		bool is_start = false;
		if (state.second == start_state)
			is_start = true;

		NFA_json["states"].push_back({
			{ "name", state.first },
			{ "starting", is_start },
			{ "accepting", state.second->accepting }
		});

		for (const auto& transition : state.second->transitions)
		{
			for (const auto& state2 : transition.second)
			{
				NFA_json["transitions"].push_back({
					{ "from", state.first },
					{ "to", state2->name },
					{ "input", std::string(1, transition.first) }
				});
			}
		}
	}

	return NFA_json;
}

void NFA::addState(const std::string& name, bool is_accepting)
{
	State* new_state = new State;
	new_state->name = name;
	new_state->accepting = is_accepting;
	states[name] = new_state;
}

bool NFA::removeState(const std::string& name)
{
	State* state_to_delete = getState(name);
	if (!state_to_delete)
		return false;

	if (state_to_delete == start_state)
		start_state = nullptr;

	// todo: test if state2 is changed
	// remove state pointer from other transitions
	for (const auto& state : states)
	{
		for (const auto& transition : state.second->transitions)
		{
			for (auto state2 : transition.second)
			{
				if (state2 == state_to_delete)
				{
					state2 = nullptr;
				}
			}
		}
	}

	states.erase(name);
	delete state_to_delete;

	return true;
}

bool NFA::setStartState(const std::string& new_start_state_name)
{
	State* new_start_state = getState(new_start_state_name);
	if (!new_start_state)
		return false;

	start_state = new_start_state;
	return true;
}

bool NFA::addTransition(const std::string& s1_name, const std::string& s2_name, char a)
{
	if (!isSymbolInAlphabet(a))
		return false;

	State* s1 = getState(s1_name);
	State* s2 = getState(s2_name);
	if (!s1 || !s2)
		return false;

	if (s1->transitions.find(a) != s1->transitions.end())
	{
		std::cout << "Overwriting transition δ(" << s1->name << ", " << a << ")" << std::endl;
	}

	s1->transitions[a].insert(s2);

	return true;
}

bool NFA::removeTransition(const std::string& s_name, char a)
{
	if (!isSymbolInAlphabet(a))
		return false;

	State* s = getState(s_name);

	if (!s)
		return false;

	s->transitions[a].clear();

	return true;
}

bool NFA::removeSpecificTransition(const std::string& s1_name, const std::string& s2_name, char a)
{
	if (!isSymbolInAlphabet(a))
		return false;

	State* s1 = getState(s1_name);
	State* s2 = getState(s2_name);

	if (!s1 || !s2)
		return false;

	if (s1->transitions[a].find(s2) == s1->transitions[a].end())
	{
		std::cerr << "Error: Couldn't remove transition δ(" << s1->name << ", " << a << ") = " << s2->name << std::endl;
		return false;
	}

	s1->transitions[a].erase(s2);

	return true;
}

bool NFA::accepts(const std::string& string_w) const
{
	if (!start_state)
	{
		std::cerr << "Error: no start state" << std::endl;
		return false;
	}

	std::set<State*> current_states;
	std::set<State*> new_current_states;
	current_states.insert(start_state);

	for (char a : string_w)
	{
		if (!isSymbolInAlphabet(a))
			return false;

		// loop through all current states
		for (const auto& current_state : current_states)
		{
			// add all states from the transition to the new set
			for (const auto& new_state : current_state->transitions[a])
			{
				new_current_states.insert(new_state);
			}
		}

		current_states = new_current_states;
		new_current_states.clear();
	}

	// todo: check if this works XD
	// sad it doesn't work :(
//	return std::any_of(current_states.begin(), current_states.end(), [](const State& state) {return state.accepting;});

	for (const auto& current_state : current_states)
	{
		if (current_state->accepting)
			return true;
	}
	return false;
}

void NFA::clear()
{
	// alphabet
	clearAlphabet();

	// states
	for (const auto& state : states)
		delete state.second;

	states.clear();
	start_state = nullptr;
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


bool NFA::checkLegality() const
{
	// check if it has a start state
	if (!start_state)
	{
		std::cerr << "Error: DFA " << getID() << " has no start state" << std::endl;
		return false;
	}

	return true;
}

std::string NFA::genDOT() const
{
	std::string dot;

	// header
	dot += "digraph finite_state_machine {";
	dot += "\n\trankdir=LR;\n\tsize=\"8,5\";";

	// body
	dot += "\n\n\tnode [shape = doublecircle];";

	for (const auto& state : states)
	{
		if (state.second->accepting)
			dot += " " + state.first;
	}

	dot += "\n\tnode [shape = circle];\n";

	dot += "\n\tstart -> " + start_state->name;

	for (const auto& state : states)
	{
		for (const auto& transitions : state.second->transitions)
		{
			for (const auto& state2 : transitions.second)
			{
				dot += "\n\t" + state.first + " -> " + state2->name;
				dot += " [ label = \"" + std::string(1, transitions.first) + "\" ];";
			}
		}
	}
	dot += "\n}";

	return dot;
}

NFA::State* NFA::getState(const std::string& name) const
{
	auto state = states.find(name);
	if (state != states.end())
		return state->second;

	std::cerr << "Error: couldn't find state with name \"" << name << "\"" << std::endl;
	return nullptr;
}
