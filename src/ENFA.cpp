#include "DFA.h"
#include "NFA.h"
#include "ENFA.h"
#include "RE.h"

ENFA::ENFA() : Automaton()
{
}

ENFA::ENFA(const std::string& json_filename) : Automaton()
{
	ENFA::load(json_filename);
}

ENFA::~ENFA()
{
	for (const auto& state : states)
		delete state.second;
}

bool ENFA::load(const std::string& filename)
{
	json ENFA_json = loadStart(filename);

	if (ENFA_json["type"] != "ENFA")
	{
		std::cerr << "Error: " << filename << " is of the type " << ENFA_json["type"]
				  << " and not ENFA" << std::endl;
		return false;
	}

	if (!(ENFA_json.contains("type")))
	{
		std::cerr << "Error: " << filename << " has an invalid format" << std::endl;
		return false;
	}

	if (!setEpsilon(ENFA_json["eps"].get<std::string>()[0]))
		return false;

	loadBaseComponents(ENFA_json);

	isLegal();

	return true;
}

json ENFA::save() const
{
	json NFA_json;

	NFA_json["type"] = "NFA";

	NFA_json["eps"] = getEpsilon();

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

bool ENFA::setEpsilon(char new_epsilon)
{
	if (alphabet.find(new_epsilon) != alphabet.end())
	{
		std::cerr << "Error: Epsilon '" << new_epsilon << "' is already in the alphabet" << std::endl;
		return false;
	}
	epsilon = new_epsilon;
	return true;
}

char ENFA::getEpsilon() const
{
	return epsilon;
}

void ENFA::addState(const std::string& name, bool is_accepting)
{
	State* new_state = new State;
	new_state->name = name;
	new_state->accepting = is_accepting;
	states[name] = new_state;
}

bool ENFA::removeState(const std::string& name)
{
	State* state_to_delete = getState(name);
	if (!state_to_delete)
		return false;

	if (state_to_delete == start_state)
		start_state = nullptr;

	// deletes the state pointer from the transitions
	for (const auto& state : states)
	{
		for (auto& transitions : state.second->transitions)
			transitions.second.erase(state_to_delete);
	}

	states.erase(name);
	delete state_to_delete;

	return true;
}

std::string ENFA::getStartState() const
{
	return start_state->name;
}

bool ENFA::setStartState(const std::string& new_start_state_name)
{
	State* new_start_state = getState(new_start_state_name);
	if (!new_start_state)
		return false;

	start_state = new_start_state;
	return true;
}

std::set<std::string> ENFA::getAllStates() const
{
	std::set<std::string> all_states;
	for (const auto& state : states)
		all_states.insert(state.first);
	return all_states;
}

bool ENFA::isStateAccepting(const std::string& s_name) const
{
	return getState(s_name)->accepting;
}

void ENFA::setStateAccepting(const std::string& s_name, bool is_accepting) const
{
	State* s = getState(s_name);
	if (s != nullptr)
		s->accepting = is_accepting;
}

bool ENFA::stateExists(const std::string& s_name) const
{
	if (getState(s_name))
		return true;
	return false;
}

std::set<std::string> ENFA::getEClosure(const std::set<std::string>& state_set) const
{
	bool new_states_added = true;
	std::set<std::string> current_set = state_set;

	while (new_states_added)
	{
		new_states_added = false;
		// adds all states from the epsilon transition to the current set

		for (const auto& s : current_set)
		{
			for (const auto& new_state : transitionFunction(s, getEpsilon()))
			{
				if (current_set.find(new_state) == current_set.end())
				{
					current_set.insert(new_state);
					new_states_added = true;
				}
			}
		}
	}
	return current_set;
}

bool ENFA::addTransition(const std::string& s1_name, const std::string& s2_name, char a)
{
	if (a != getEpsilon() && !isSymbolInAlphabet(a))
		return false;

	State* s1 = getState(s1_name);
	State* s2 = getState(s2_name);
	if (!s1 || !s2)
		return false;

	s1->transitions[a].insert(s2);

	return true;
}

bool ENFA::removeTransition(const std::string& s_name, char a)
{
	if (a != getEpsilon() && !isSymbolInAlphabet(a))
		return false;

	State* s = getState(s_name);

	if (!s)
		return false;

	s->transitions[a].clear();

	return true;
}

bool ENFA::removeSingleTransition(const std::string& s1_name, const std::string& s2_name, char a)
{
	if (a != getEpsilon() && !isSymbolInAlphabet(a))
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

std::set<std::string> ENFA::transitionFunction(const std::string& s_name, char a) const
{
	std::set<std::string> result;
	for (const auto& state : getState(s_name)->transitions[a])
		result.insert(state->name);
	return result;
}

bool ENFA::accepts(const std::string& string_w) const
{
	if (!start_state)
	{
		std::cerr << "Error: no start state" << std::endl;
		return false;
	}

	std::set<std::string> set_of_input_states;
	std::set<std::string> set_of_output_states;
	set_of_input_states.insert(start_state->name);

	if (string_w.empty())
		return isSetOfStatesAccepting(getEClosure(set_of_input_states));

	for (char a : string_w)
	{
		if (a != getEpsilon() && !isSymbolInAlphabet(a))
			return false;

		set_of_input_states = getEClosure(set_of_input_states);
		set_of_output_states = transitionFunctionSetOfStates(set_of_input_states, a);

		if (set_of_output_states.empty())
			return false;

		set_of_input_states = getEClosure(set_of_output_states);
		set_of_output_states.clear();
	}

	return isSetOfStatesAccepting(set_of_input_states);
}

void ENFA::clear()
{
	// alphabet
	clearAlphabet();

	// states
	for (const auto& state : states)
		delete state.second;

	states.clear();
	start_state = nullptr;
}

DFA ENFA::toDFA() const
{
	DFA dfa;
	std::vector<std::set<std::string>> sets_of_input_states;
	std::vector<std::set<std::string>> new_sets_of_input_states;
	std::set<std::string> set_of_output_states;
	std::set<std::string> current_state;
	bool contains_empty_set = false;

	dfa.setAlphabet(getAlphabet());
	current_state = getEClosure({ start_state->name });
	std::string input_start_state_name = getSetOfStatesString(current_state);

	dfa.addState(input_start_state_name, isSetOfStatesAccepting(current_state));
	dfa.setStartState(input_start_state_name);
	sets_of_input_states.push_back(current_state);

	while (!sets_of_input_states.empty())
	{
		// takes a set of input states that hasn't been processed
		current_state = sets_of_input_states.front();
		sets_of_input_states.erase(sets_of_input_states.begin());

		current_state = getEClosure(current_state);

		for (char a : getAlphabet())
		{
			// transitions
			set_of_output_states = transitionFunctionSetOfStates(current_state, a);
			// e-closure
			set_of_output_states = getEClosure(set_of_output_states);

			std::string new_state_name;

			// empty set
			if (set_of_output_states.empty())
			{
				if (!contains_empty_set)
				{
					dfa.addState("{}", false);
					contains_empty_set = true;
					for (char symbol : getAlphabet())
						dfa.addTransition("{}", "{}", symbol);
				}
				new_state_name = "{}";
			}
			else
			{
				new_state_name = getSetOfStatesString(set_of_output_states);

				// adds the state if it doesn't exists in the dfa
				if (!dfa.stateExists(new_state_name))
				{
					dfa.addState(new_state_name, isSetOfStatesAccepting(set_of_output_states));
					new_sets_of_input_states.push_back(set_of_output_states);
				}
			}
			dfa.addTransition(getSetOfStatesString(current_state), new_state_name, a);
			set_of_output_states.clear();
		}
		sets_of_input_states
			.insert(sets_of_input_states.end(), new_sets_of_input_states.begin(), new_sets_of_input_states.end());
		new_sets_of_input_states.clear();

	}
	dfa.isLegal();
	return dfa;
}

NFA ENFA::toNFA() const
{
	return NFA();
}

RE ENFA::toRE() const
{
	return toDFA().toRE();
}

void ENFA::printStats() const
{
	std::cout << "no_of_states=" << states.size() << std::endl;

	std::set<char> full_alphabet = alphabet;
	full_alphabet.insert(getEpsilon());

	for (const auto& symbol : full_alphabet)
	{
		int no_of_transitions = 0;
		for (const auto& state : states)
			no_of_transitions += state.second->transitions[symbol].size();
		std::cout << "no_of_transitions[" << symbol << "]=" << no_of_transitions << std::endl;
	}

	std::map<int, int> degrees;

	for (const auto& state : states)
	{
		int degree = 0;
		for (const auto& symbol : full_alphabet)
			degree += state.second->transitions[symbol].size();
		if (!degrees[degree])
			degrees[degree] = 1;
		else
			degrees[degree]++;
	}

	for (const auto& degree : degrees)
		std::cout << "degree[" << degree.first << "]=" << degree.second << std::endl;
}

bool ENFA::isLegal() const
{
	bool legal = true;

	// check if it has a start state
	if (!start_state)
	{
		std::cerr << "Error: DFA " << getID() << " has no start state" << std::endl;
		legal = false;
	}

	// check if epsilon is not in the alphabet set
	if (alphabet.find(getEpsilon()) != alphabet.end())
		legal = false;

	return legal;
}

std::string ENFA::genDOT() const
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

ENFA::State* ENFA::getState(const std::string& name, bool error_output) const
{
	auto state = states.find(name);
	if (state != states.end())
		return state->second;

	if (error_output)
		std::cerr << "Error: couldn't find state with name \"" << name << "\"" << std::endl;
	return nullptr;
}
