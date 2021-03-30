#include "Automaton.h"

const std::string DOT_IMAGES_PATH = "../DOT_images/";
const std::string DOT_FILES_PATH = "../DOT_files/";

Automaton::~Automaton()
{
	for (const auto& state : states)
		delete state.second;
}

Automaton& Automaton::operator=(const Automaton& _automaton)
{
	if (&_automaton != this)
	{
		this->Alphabet::operator=(_automaton);

		for (const auto& state : _automaton.states)
			addState(state.first, state.second->accepting);

		for (const auto& state : _automaton.states)
		{
			for (const auto& transition : state.second->transitions)
			{
				for (const auto& output_state : transition.second)
					addTransition(state.first, output_state->name, transition.first);
			}
		}

		setStartState(_automaton.getStartState());
	}

	return *this;
}

json Automaton::loadJsonFile(const std::string& filename)
{
	// file input
	std::ifstream input(filename);

	json automaton_json;

	if (!input.good())
	{
		std::cerr << "Error: couldn't open \"" << filename << "\"" << std::endl;
		return automaton_json;
	}

	input >> automaton_json;
	input.close();

	return automaton_json;
}

bool Automaton::loadBasicComponents(const std::string& filename, const json& automaton_json)
{
	if (!(automaton_json.contains("type") && automaton_json.contains("alphabet")
		&& automaton_json.contains("states") && automaton_json.contains("transitions")))
	{
		*getOutputStream() << "Error: \"" << filename << "\" has an invalid format" << std::endl;
		return false;
	}

	clear();

	for (const auto& symbol : automaton_json["alphabet"])
	{
		addSymbol(symbol.get<std::string>()[0]);
	}

	if (automaton_json.contains("eps"))
    {
        setEpsilon(automaton_json["eps"].get<std::string>()[0]);
        addSymbol(automaton_json["eps"].get<std::string>()[0]);
    }

	for (const auto& state : automaton_json["states"])
	{
		addState(state["name"].get<std::string>(), state["accepting"].get<bool>());
		if (state["starting"].get<bool>())
			setStartState(state["name"].get<std::string>());
	}

	for (const auto& transition : automaton_json["transitions"])
	{
		addTransition(transition["from"].get<std::string>(), transition["to"].get<std::string>(),
			transition["input"].get<std::string>()[0]);
	}

	isLegal();

	return true;
}

json Automaton::saveBasicComponents() const
{
	json automaton_json;

	automaton_json["alphabet"] = json::array();
	for (auto symbol : getAlphabet())
		automaton_json["alphabet"].push_back(std::string(1, symbol));

	for (const auto& state : states)
	{
		bool is_start = false;
		if (state.second == start_state)
			is_start = true;

		automaton_json["states"].push_back({
			{ "name", state.first },
			{ "starting", is_start },
			{ "accepting", state.second->accepting }
		});

		for (const auto& transition : state.second->transitions)
		{
			for (const auto& state2 : transition.second)
			{
				automaton_json["transitions"].push_back({
					{ "from", state.first },
					{ "to", state2->name },
					{ "input", std::string(1, transition.first) }
				});
			}
		}
	}

	return automaton_json;
}

void Automaton::print(std::ostream& output_stream) const
{
	output_stream << std::setw(4) << save() << std::endl;
}

void Automaton::addState(const std::string& name, bool is_accepting)
{
	State* new_state = new State;
	new_state->name = name;
	new_state->accepting = is_accepting;
	states[name] = new_state;
}

bool Automaton::removeState(const std::string& s_str)
{
	State* state_to_delete = getState(s_str);
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

	states.erase(s_str);
	delete state_to_delete;

	return true;
}

std::string Automaton::getStartState() const
{
	if (start_state)
		return start_state->name;
	else
	{
		*getOutputStream() << "Error: automaton " << getID() << " has no start state" << std::endl;
		return "";
	}
}

bool Automaton::setStartState(const std::string& start_state_str)
{
	State* new_start_state = getState(start_state_str);
	if (!new_start_state)
		return false;

	start_state = new_start_state;
	return true;
}

bool Automaton::isStateAccepting(const std::string& s_str) const
{
	State* s = getState(s_str);
	if (s)
		return s->accepting;
	else
	{
		*getOutputStream() << "Error: automaton " << getID() << " has no state with name \"" + s_str + "\"";
		return false;
	}
}

void Automaton::setStateAccepting(const std::string& s_str, bool is_accepting) const
{
	State* s = getState(s_str);
	if (s)
		s->accepting = is_accepting;
	else
	{
		*getOutputStream() << "Error: automaton " << getID() << " has no state with name \"" + s_str + "\"";
		return;
	}
}

std::set<std::string> Automaton::getAllStates() const
{
	std::set<std::string> all_states;
	for (const auto& state : states)
		all_states.insert(state.first);
	return all_states;
}

bool Automaton::isSetOfStatesAccepting(const std::set<std::string>& set_of_states) const
{
	for (const auto& s : set_of_states)
	{
		if (isStateAccepting(s))
			return true;
	}
	return false;
}

std::string Automaton::getSetOfStatesString(const std::set<std::string>& set_of_states)
{
	std::string set_of_states_string;

	set_of_states_string = "{";
	for (auto it = set_of_states.begin(); it != set_of_states.end(); it++)
	{
		if (it != set_of_states.begin())
			set_of_states_string += ",";

		set_of_states_string += *it;
	}
	set_of_states_string += "}";

	return set_of_states_string;
}

bool Automaton::stateExists(const std::string& s_str) const
{
	if (getState(s_str, false))
		return true;
	return false;
}

bool Automaton::addTransition(const std::string& s1_str, const std::string& s2_str, char a)
{
	if (!isSymbolInAlphabet(a))
		return false;

	State* s1 = getState(s1_str);
	State* s2 = getState(s2_str);

	if (!s1 || !s2)
		return false;

	s1->transitions[a].insert(s2);

	return true;
}

bool Automaton::removeTransitions(const std::string& s_str, char a)
{
	if (!isSymbolInAlphabet(a))
		return false;

	State* s = getState(s_str);

	if (!s)
		return false;

	s->transitions[a].clear();

	return true;
}

bool Automaton::removeTransition(const std::string& s1_str, const std::string& s2_str, char a)
{
	if (!isSymbolInAlphabet(a))
		return false;

	State* s1 = getState(s1_str);
	State* s2 = getState(s2_str);

	if (!s1 || !s2)
		return false;

	if (s1->transitions[a].find(s2) == s1->transitions[a].end())
	{
		*getOutputStream() << "Error: automaton " << getID() << ", couldn't remove transition δ(" << s1->name << ", " << a
				  << ") = " << s2->name << std::endl;
		return false;
	}

	s1->transitions[a].erase(s2);

	return true;
}

std::set<std::string> Automaton::transition(const std::string& s_str, char a) const
{
	std::set<std::string> result;
	std::set<std::string> current_states = { s_str };

	for (const auto& current_state : current_states)
	{
		for (const auto& state : getState(current_state)->transitions[a])
			result.insert(state->name);
	}

	return result;
}

std::set<std::string> Automaton::transitionSetOfStates(const std::set<std::string>& set_of_states, char a) const
{
	std::set<std::string> set_of_input_states = set_of_states;
	std::set<std::string> set_of_output_states;

	for (const auto& input_state : set_of_input_states)
	{
		for (const auto& output_state : transition(input_state, a))
			set_of_output_states.insert(output_state);
	}

	return set_of_output_states;
}

std::set<std::string> Automaton::eClosure(const std::set<std::string>& set_of_states) const
{
	bool new_states_added = true;
	std::set<std::string> current_set = set_of_states;

	if (getEpsilon() == ' ')
		return current_set;

	while (new_states_added)
	{
		new_states_added = false;

		// adds all set_of_states from the epsilon transition to the current set
		for (const auto& s : current_set)
		{
			for (const auto& new_state : transition(s, getEpsilon()))
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

bool Automaton::accepts(const std::string& string_w) const
{
	if (!isLegal())
		return false;

	std::set<std::string> set_of_input_states;
	std::set<std::string> set_of_output_states;
	set_of_input_states.insert(getStartState());

	for (char a : string_w)
	{
		if (!isSymbolInAlphabet(a))
			return false;

		if (getEpsilon() != ' ')
			set_of_input_states = eClosure(set_of_input_states);

		set_of_output_states = transitionSetOfStates(set_of_input_states, a);

		if (set_of_output_states.empty())
			return false;

		set_of_input_states = set_of_output_states;
		set_of_output_states.clear();
	}

	if (getEpsilon() != ' ')
		set_of_input_states = eClosure(set_of_input_states);

	return isSetOfStatesAccepting(set_of_input_states);
}

void Automaton::clear()
{
	// alphabet
	clearAlphabet();

	// states
	for (const auto& state : states)
		delete state.second;

	states.clear();
	start_state = nullptr;
}

void Automaton::printStats() const
{
	std::cout << "no_of_states=" << states.size() << std::endl;

	for (const auto& symbol : getAlphabet())
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
		for (const auto& symbol : getAlphabet())
			degree += state.second->transitions[symbol].size();
		if (!degrees[degree])
			degrees[degree] = 1;
		else
			degrees[degree]++;
	}

	for (const auto& degree : degrees)
		std::cout << "degree[" << degree.first << "]=" << degree.second << std::endl;
}

void Automaton::renameStates(bool letters)
{
	std::map<std::string, State*> new_states;

	if (letters)
	{
		std::string alphaberta = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		int nr_of_letters = 1;
		int current_letter = 0;

		for (auto& state : states)
		{
			const std::string new_name = std::string(nr_of_letters, alphaberta.at(current_letter));

			state.second->name = new_name;
			new_states[new_name] = state.second;

			current_letter++;
			if (current_letter == 26)
			{
				current_letter = 0;
				nr_of_letters++;
			}
		}
	}
	else
	{
		int new_name = 0;
		for (auto& state : states)
		{
			state.second->name = std::to_string(new_name);
			new_states[std::to_string(new_name)] = state.second;

			new_name++;
		}
	}

	states = new_states;
}

std::string Automaton::genDOT() const
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
		std::map<State*, std::set<char>> _transitions;

		for (const auto& transitions : state.second->transitions)
		{
			for (const auto& _state : transitions.second)
			{
				_transitions[_state].insert(transitions.first);
			}
		}

		for (const auto& state2 : _transitions)
		{
			dot += "\n\t\"" + state.first + "\"" + " -> \"" + state2.first->name + "\"";

			std::string result;
			for (auto it = state2.second.begin(); it != state2.second.end(); it++)
			{
				if (it != state2.second.begin())
					result += ",";

				result += std::string(1, *it);
			}

			dot += " [ label = \"" + result + "\" ];";
		}
	}
	dot += "\n}";

	return dot;
}

bool Automaton::genImage() const
{
	if (!isLegal())
	{
		*getOutputStream() << "Error: Automaton " << getID() << " is illegal" << std::endl;
		return false;
	}

	std::string path = DOT_FILES_PATH + std::to_string(getID()) + ".dot";

	std::ofstream file(path);
	std::string my_string = genDOT();
	file << my_string;
	file.close();

	GVC_t* gvc;
	Agraph_t* g;
	FILE* fp;
	gvc = gvContext();
	fp = fopen((path).c_str(), "r");
	g = agread(fp, nullptr);
	gvLayout(gvc, g, "dot");
	gvRender(gvc, g, "png", fopen((DOT_IMAGES_PATH + std::to_string(getID()) + "_image.png").c_str(),
		"w"));
	gvFreeLayout(gvc, g);
	agclose(g);

	return (gvFreeContext(gvc));
}

Automaton::State* Automaton::getState(const std::string& name, bool error_output) const
{
	auto state = states.find(name);
	if (state != states.end())
		return state->second;

	if (error_output)
		*getOutputStream() << "Error: automaton " << getID() << ", couldn't find state with name \"" << name << "\"" << std::endl;
	return nullptr;
}
