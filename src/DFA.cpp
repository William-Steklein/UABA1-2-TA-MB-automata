#include "DFA.h"
#include "NFA.h"
#include "ENFA.h"
#include "RE.h"

DFA::DFA() : Automaton()
{
}

DFA::DFA(const std::string& json_filename) : Automaton()
{
	load(json_filename);
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

	json DFA_json;
	input >> DFA_json;

	if (!(DFA_json.contains("type") && DFA_json.contains("alphabet")
		&& DFA_json.contains("states") && DFA_json.contains("transitions")))
	{
		std::cerr << "Error: " << filename << " has an invalid format" << std::endl;
		return false;
	}

	if (DFA_json["type"] != "DFA")
	{
		std::cerr << "Error: " << filename << " is of the type " << DFA_json["type"]
				  << " and not DFA" << std::endl;
		return false;
	}

	for (const auto& symbol : DFA_json["alphabet"])
	{
		addSymbol(symbol.get<std::string>()[0]);
	}

	for (const auto& state : DFA_json["states"])
	{
		addState(state["name"].get<std::string>(), state["accepting"].get<bool>());
		if (state["starting"].get<bool>())
			setStartState(state["name"].get<std::string>());
	}

	for (const auto& transition : DFA_json["transitions"])
	{
		addTransition(transition["from"].get<std::string>(), transition["to"].get<std::string>(),
			transition["input"].get<std::string>()[0]);
	}

	checkLegality();

	return true;
}

json DFA::save() const
{
	json json_DFA;

	json_DFA["type"] = "DFA";

	json_DFA["alphabet"] = json::array();
	for (auto symbol : alphabet)
		json_DFA["alphabet"].push_back(std::string(1, symbol));

	for (const auto& state : states)
	{
		bool is_start = false;
		if (state.second == start_state)
			is_start = true;

		json_DFA["states"].push_back({
			{ "name", state.first },
			{ "starting", is_start },
			{ "accepting", state.second->accepting }
		});

		for (const auto& transition : state.second->transitions)
		{
			json_DFA["transitions"].push_back({
				{ "from", state.first },
				{ "to", transition.second->name },
				{ "input", std::string(1, transition.first) }
			});
		}
	}

	return json_DFA;
}

void DFA::print() const
{
	std::cout << std::setw(4) << save() << std::endl;
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

void DFA::addState(const std::string& name, bool is_accepting = false)
{
	State* new_state = new State;
	new_state->name = name;
	new_state->accepting = is_accepting;
	states[name] = new_state;
}

bool DFA::removeState(const std::string& name)
{
	State* state_to_delete = getState(name);
	if (!state_to_delete)
		return false;

	if (state_to_delete == start_state)
		start_state = nullptr;

	// remove state pointer from other transitions
	for (const auto& state : states)
	{
		for (auto& transition : state.second->transitions)
		{
			if (transition.second == state_to_delete)
			{
				transition.second = nullptr;
			}
		}
	}

	states.erase(name);
	delete state_to_delete;

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
		std::cout << "Overwriting transition δ(" << s1->name << ", " << a << ")" << std::endl;
	}

	s1->transitions[a] = s2;

	return true;
}

bool DFA::removeTransition(const std::string& s_name, char a)
{
	if (!isSymbolInAlphabet(a))
		return false;

	State* s = getState(s_name);

	if (!s)
		return false;

	s->transitions[a] = nullptr;

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
		{
			std::cerr << "Error: symbol " << a << " is not in the alphabet" << std::endl;
			return false;
		}

		// check if there is a transition
		if (current_state->transitions[a] == nullptr)
		{
			std::cerr << "Error: δ(" << current_state->name << ", " << a << ") has no output" << std::endl;
			return false;
		}

		current_state = current_state->transitions[a];
	}

	if (current_state->accepting)
		return true;
	else
		return false;
}

NFA DFA::toNFA()
{
	return NFA();
}

ENFA DFA::toENFA()
{
	return ENFA();
}

RE DFA::toRE()
{
	return RE();
}

bool DFA::checkLegality() const
{
	bool legal = true;
	bool has_accepting_state = false;

	// check if it has a start state
	if (!start_state)
	{
		std::cerr << "Error: DFA " << getID() << " has no start state" << std::endl;
		legal = false;
	}

	// check if every state has a transition with every symbol
	for (auto state : states)
	{
		for (auto c : alphabet)
		{
			if (state.second->transitions[c] == nullptr)
			{
				std::cerr << "Error: DFA " << getID() << " has no transition from state " << state.first <<
						  " with symbol " << c << std::endl;
				legal = false;
			}
		}
	}

	return legal;
}

DFA::State* DFA::getState(const std::string& name) const
{
	auto state = states.find(name);
	if (state != states.end())
		return state->second;

	std::cerr << "Error: couldn't find state with name \"" << name << "\"" << std::endl;
	return nullptr;
}

std::string DFA::genDOT() const
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
		for (const auto& transition : state.second->transitions)
		{
			if (transition.second)
			{
				dot += "\n\t" + state.first + " -> " + transition.second->name;
				dot += " [ label = \"" + std::string(1, transition.first) + "\" ];";
			}
		}
	}
	dot += "\n}";

	return dot;
}
