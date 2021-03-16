#include "DFA.h"
#include "NFA.h"
#include "ENFA.h"
#include "RE.h"

DFA::DFA() : Automaton()
{
}

DFA::DFA(const std::string& json_filename) : Automaton()
{
	DFA::load(json_filename);
}

DFA::DFA(const DFA& dfa1, const DFA& dfa2, bool intersection)
{
	product(dfa1, dfa2, intersection);
}

DFA::~DFA()
{
	for (const auto& state : states)
		delete state.second;
}

bool DFA::load(const std::string& filename)
{
	json DFA_json = loadStart(filename);

	if (DFA_json["type"] != "DFA")
	{
		std::cerr << "Error: " << filename << " is of the type " << DFA_json["type"]
				  << " and not DFA" << std::endl;
		return false;
	}

	loadBaseComponents(DFA_json);

	isLegal();

	return true;
}

json DFA::save() const
{
	json DFA_json;

	DFA_json["type"] = "DFA";

	DFA_json["alphabet"] = json::array();
	for (auto symbol : alphabet)
		DFA_json["alphabet"].push_back(std::string(1, symbol));

	for (const auto& state : states)
	{
		bool is_start = false;
		if (state.second == start_state)
			is_start = true;

		DFA_json["states"].push_back({
			{ "name", state.first },
			{ "starting", is_start },
			{ "accepting", state.second->accepting }
		});

		for (const auto& transition : state.second->transitions)
		{
			DFA_json["transitions"].push_back({
				{ "from", state.first },
				{ "to", transition.second->name },
				{ "input", std::string(1, transition.first) }
			});
		}
	}

	return DFA_json;
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
	State* state_to_delete = getState(name);
	if (!state_to_delete)
		return false;

	if (state_to_delete == start_state)
		start_state = nullptr;

	// removes state pointer from other transitions
	for (const auto& state : states)
	{
		for (auto& transition : state.second->transitions)
		{
			if (transition.second == state_to_delete)
				transition.second = nullptr;
		}
	}

	states.erase(name);
	delete state_to_delete;

	return true;
}

std::string DFA::getStartState() const
{
	return start_state->name;
}

bool DFA::setStartState(const std::string& new_start_state_name)
{
	State* new_start_state = getState(new_start_state_name);
	if (!new_start_state)
		return false;

	start_state = new_start_state;
	return true;
}

bool DFA::isStateAccepting(const std::string& s_name) const
{
	if (stateExists(s_name))
		return getState(s_name)->accepting;
	else
		std::cerr << "Error: DFA " << getID() << " state \"" + s_name + "\" doesn't exists" << std::endl;
	return false;
}

bool DFA::stateExists(const std::string& s_name) const
{
	if (getState(s_name, false))
		return true;
	return false;
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
		std::cout << "Overwriting transition δ(" << s1->name << ", " << a << ")" << std::endl;

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

std::set<std::string> DFA::transitionFunction(const std::string& s_name, char a) const
{
	if (getState(s_name)->transitions[a] != nullptr)
		return {getState(s_name)->transitions[a]->name};
	else
		return {};
}

bool DFA::accepts(const std::string& string_w) const
{
	if (!start_state)
	{
		std::cerr << "Error: no start state" << std::endl;
		return false;
	}

	std::string current_state = start_state->name;

	for (char a : string_w)
	{
		if (!isSymbolInAlphabet(a))
			return false;

		// checks if there is a transition
		if (transitionFunction(current_state, a).empty())
		{
			std::cerr << "Error: δ(" << current_state << ", " << a << ") has no output" << std::endl;
			return false;
		}

		current_state = *transitionFunction(current_state, a).begin();
	}

	if (isStateAccepting(current_state))
		return true;
	else
		return false;
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

void DFA::product(const DFA& dfa1, const DFA& dfa2, bool intersection)
{
	std::pair<std::string, std::string> pair_of_input_states = {dfa1.getStartState(), dfa2.getStartState()};
	std::pair<std::string, std::string> pair_of_output_states;
	std::vector<std::pair<std::string, std::string>> pairs_of_input_states;
	std::vector<std::pair<std::string, std::string>> new_pairs_of_input_states;
	std::string new_state_name;

	setAlphabet(dfa1.getAlphabet());

	new_state_name = "(" + pair_of_input_states.first + "," + pair_of_input_states.second + ")";

	if (intersection)
		addState(new_state_name,
			dfa1.isStateAccepting(pair_of_input_states.first) && dfa2.isStateAccepting(pair_of_input_states.second));
	else
		addState(new_state_name,
			dfa1.isStateAccepting(pair_of_input_states.first) || dfa2.isStateAccepting(pair_of_input_states.second));

	setStartState("(" + pair_of_input_states.first + "," + pair_of_input_states.second + ")");
	pairs_of_input_states.push_back(pair_of_input_states);

	while (!pairs_of_input_states.empty())
	{
		pair_of_input_states = pairs_of_input_states.front();
		pairs_of_input_states.erase(pairs_of_input_states.begin());

		for (char a : getAlphabet())
		{
			// transitions
			pair_of_output_states.first = *dfa1.transitionFunction(pair_of_input_states.first, a).begin();
			pair_of_output_states.second = *dfa2.transitionFunction(pair_of_input_states.second, a).begin();

			new_state_name = "(" + pair_of_output_states.first + "," + pair_of_output_states.second + ")";

			// adds the state if it doesn't exists in the product dfa
			if (!stateExists(new_state_name))
			{
				if (intersection)
					addState(new_state_name,
						dfa1.isStateAccepting(pair_of_output_states.first) && dfa2.isStateAccepting(pair_of_output_states.second));
				else
					addState(new_state_name,
						dfa1.isStateAccepting(pair_of_output_states.first) || dfa2.isStateAccepting(pair_of_output_states.second));

				new_pairs_of_input_states.push_back(pair_of_output_states);
			}
			addTransition("(" + pair_of_input_states.first + "," + pair_of_input_states.second + ")",
				new_state_name, a);
		}
		pairs_of_input_states
			.insert(pairs_of_input_states.end(), new_pairs_of_input_states.begin(), new_pairs_of_input_states.end());
		new_pairs_of_input_states.clear();
	}
	isLegal();
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

void DFA::printStats() const
{
	std::cout << "no_of_states=" << states.size() << std::endl;

	for (const auto& symbol : alphabet)
	{
		int no_of_transitions = 0;
		for (const auto& state : states)
		{
			if (state.second->transitions[symbol])
				no_of_transitions++;
		}
		std::cout << "no_of_transitions[" << symbol << "]=" << no_of_transitions << std::endl;
	}

	std::map<int, int> degrees;

	for (const auto& state : states)
	{
		int degree = state.second->transitions.size();
		if (!degrees[degree])
			degrees[degree] = 1;
		else
			degrees[degree]++;
	}

	for (const auto& degree : degrees)
	{
		std::cout << "degree[" << degree.first << "]=" << degree.second << std::endl;
	}
}

bool DFA::isLegal() const
{
	bool legal = true;

	// checks if it has a start state
	if (!start_state)
	{
		std::cerr << "Error: DFA " << getID() << " has no start state" << std::endl;
		legal = false;
	}

	// checks if every state has a transition with every symbol
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
			dot += " \"" + state.first + "\"";
	}

	dot += "\n\tnode [shape = circle];\n";

	dot += "\n\tstart -> \"" + start_state->name + "\"";

	for (const auto& state : states)
	{
		for (const auto& transition : state.second->transitions)
		{
			if (transition.second)
			{
				dot += "\n\t\"" + state.first + "\"" + " -> \"" + transition.second->name + "\"";
				dot += " [ label = \"" + std::string(1, transition.first) + "\" ];";
			}
		}
	}
	dot += "\n}";

	return dot;
}

DFA::State* DFA::getState(const std::string& name, bool error_output) const
{
	auto state = states.find(name);
	if (state != states.end())
		return state->second;

	if (error_output)
		std::cerr << "Error: couldn't find state with name \"" << name << "\"" << std::endl;
	return nullptr;
}
