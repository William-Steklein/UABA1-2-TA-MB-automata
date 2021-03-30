#include "DFA.h"
#include "NFA.h"
#include "ENFA.h"
#include "RE.h"

bool ENFA::load(const std::string& filename)
{
	json ENFA_json = loadJsonFile(filename);

	if (ENFA_json.empty())
		return false;

	if (ENFA_json["type"] != "ENFA")
	{
		*getOutputStream() << "Error: " << filename << " is of the type " << ENFA_json["type"] << " and not ENFA"
				  << std::endl;
		return false;
	}

	if (!ENFA_json.contains("eps"))
	{
		*getOutputStream() << "Error: \"" << filename << "\" has an invalid format" << std::endl;
		return false;
	}

	return loadBasicComponents(filename, ENFA_json);
}

json ENFA::save() const
{
	json automaton_json = saveBasicComponents();
	automaton_json["type"] = "ENFA";

	automaton_json["eps"] = getEpsilon();

	return automaton_json;
}

ENFA::ENFA(const std::string& json_filename)
{
	load(json_filename);
}

DFA ENFA::toDFA() const
{
	DFA dfa;
	std::vector<std::set<std::string>> sets_of_input_states;
	std::vector<std::set<std::string>> new_sets_of_input_states;
	std::set<std::string> set_of_output_states;
	std::set<std::string> current_state;
	bool contains_empty_set = false;

	std::set<char> new_alphabet = getAlphabet();
	new_alphabet.erase(getEpsilon());
	dfa.setAlphabet(new_alphabet);
	current_state = eClosure({ getStartState() });
	std::string input_start_state_name = getSetOfStatesString(current_state);

	dfa.addState(input_start_state_name, isSetOfStatesAccepting(current_state));
	dfa.setStartState(input_start_state_name);
	sets_of_input_states.push_back(current_state);

	while (!sets_of_input_states.empty())
	{
		// takes a set of input states that hasn't been processed
		current_state = sets_of_input_states.front();
		sets_of_input_states.erase(sets_of_input_states.begin());

		current_state = eClosure(current_state);

		for (char a : new_alphabet)
		{
			// transitions
			set_of_output_states = transitionSetOfStates(current_state, a);
			// e-closure
			set_of_output_states = eClosure(set_of_output_states);

			std::string new_state_name;

			// empty set
			if (set_of_output_states.empty())
			{
				if (!contains_empty_set)
				{
					dfa.addState("{}", false);
					contains_empty_set = true;
					for (char symbol : new_alphabet)
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
	return RE();
}

bool ENFA::isLegal() const
{
	bool is_legal = true;

	if (getStartState().empty())
		is_legal = false;

	if (getEpsilon() == ' ')
	{
		*getOutputStream() << "Error: ENFA " << getID() << " has no epsilon symbol" << std::endl;
		is_legal = false;
	}

	if (!isSymbolInAlphabet(getEpsilon()))
	{
		*getOutputStream() << "Error: ENFA " << getID() << ", epsilon is not in the alphabet" << std::endl;
		is_legal = false;
	}

	return is_legal;
}
