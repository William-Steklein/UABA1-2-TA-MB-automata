#include "NFA.h"

bool NFA::load(const std::string& filename)
{
	json NFA_json = loadJsonFile(filename);

	if (NFA_json.empty())
		return false;

	if (NFA_json["type"] != "NFA")
	{
		*getOutputStream() << "Error: " << filename << " is of the type " << NFA_json["type"] << " and not NFA"
				  << std::endl;
		return false;
	}

    if (NFA_json.contains("eps"))
    {
        *getOutputStream() << "Error: \"" << filename << "\" has an invalid format" << std::endl;
        return false;
    }

	return Automaton::loadBasicComponents(filename, NFA_json);
}

json NFA::save() const
{
	json automaton_json = Automaton::saveBasicComponents();
	automaton_json["type"] = "NFA";

	return automaton_json;
}

NFA::NFA(const std::string& json_filename)
{
	load(json_filename);
}

DFA NFA::toDFA() const
{
	DFA dfa;
	std::vector<std::set<std::string>> sets_of_input_states;
	std::vector<std::set<std::string>> new_sets_of_input_states;
	std::set<std::string> set_of_output_states;
	std::set<std::string> current_set_of_input_states;
	bool contains_empty_set = false;

	dfa.setAlphabet(getAlphabet());

	std::string start_state_name = getSetOfStatesString({getStartState()});

	dfa.addState(start_state_name, isStateAccepting(getStartState()));
	dfa.setStartState(start_state_name);
	sets_of_input_states.push_back({ getStartState() });

	while (!sets_of_input_states.empty())
	{
		// takes a set of input states that hasn't been processed
		current_set_of_input_states = sets_of_input_states.front();
		sets_of_input_states.erase(sets_of_input_states.begin());

		for (char a : getAlphabet())
		{
			// transitions
			set_of_output_states = transitionSetOfStates(current_set_of_input_states, a);

			std::string new_state_name;

			// empty set
			if (set_of_output_states.empty())
			{
				if (!contains_empty_set)
				{
					contains_empty_set = true;
					dfa.addState("{}", false);
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
			dfa.addTransition(getSetOfStatesString(current_set_of_input_states), new_state_name, a);
			set_of_output_states.clear();

		}
		sets_of_input_states
			.insert(sets_of_input_states.end(), new_sets_of_input_states.begin(), new_sets_of_input_states.end());
		new_sets_of_input_states.clear();

	}
	dfa.isLegal();
	return dfa;
}

ENFA NFA::toENFA() const
{
	ENFA enfa;

	enfa.setAlphabet(getAlphabet());
	enfa.setEpsilon(Alphabet::getStandardEpsilon());
	enfa.addSymbol(Alphabet::getStandardEpsilon());

	for (const auto& state : getAllStates())
		enfa.addState(state, isStateAccepting(state));

	for (const auto& state : getAllStates())
	{
		for (char a : getAlphabet())
			enfa.addTransitions(state, transition(state, a), a);
	}

	enfa.setStartState(getStartState());

	return enfa;
}

RE NFA::toRE() const
{
	return toDFA().toRE();
}

bool NFA::isLegal() const
{
	bool is_legal = true;

	if (getStartState().empty())
		is_legal = false;

	if (getEpsilon() != ' ')
	{
		*getOutputStream() << "Error: NFA " << getID() << " has an epsilon symbol '" << getEpsilon() << "'" << std::endl;
		is_legal = false;
	}

	return is_legal;
}
