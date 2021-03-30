#include "DFA.h"
#include "NFA.h"
#include "ENFA.h"
#include "RE.h"

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
	std::set<std::string> current_state;
	bool contains_empty_set = false;

	dfa.setAlphabet(getAlphabet());

	std::string start_state_name = getSetOfStatesString({getStartState()});

	dfa.addState(start_state_name, isStateAccepting(getStartState()));
	dfa.setStartState(start_state_name);
	sets_of_input_states.push_back({ getStartState() });

	while (!sets_of_input_states.empty())
	{
		// takes a set of input states that hasn't been processed
		current_state = sets_of_input_states.front();
		sets_of_input_states.erase(sets_of_input_states.begin());

		for (char a : getAlphabet())
		{
			// transitions
			set_of_output_states = transitionSetOfStates(current_state, a);

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

ENFA NFA::toENFA() const
{
	return ENFA();
}

RE NFA::toRE() const
{
	return RE();
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
