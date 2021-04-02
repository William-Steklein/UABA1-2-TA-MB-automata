#include "DFA.h"

bool DFA::load(const std::string& filename)
{
	json DFA_json = loadJsonFile(filename);

	if (DFA_json.empty())
		return false;

	if (DFA_json["type"] != "DFA")
	{
		*getOutputStream() << "Error: " << filename << " is of the type " << DFA_json["type"] << " and not DFA"
						   << std::endl;
		return false;
	}

	if (DFA_json.contains("eps"))
	{
		*getOutputStream() << "Error: \"" << filename << "\" has an invalid format" << std::endl;
		return false;
	}

	return Automaton::loadBasicComponents(filename, DFA_json);
}

json DFA::save() const
{
	json automaton_json = Automaton::saveBasicComponents();
	automaton_json["type"] = "DFA";

	return automaton_json;
}

DFA::DFA(const std::string& json_filename)
{
	DFA::load(json_filename);
}

DFA::DFA(const DFA& dfa1, const DFA& dfa2, bool intersection)
{
	product(dfa1, dfa2, intersection);
}

bool DFA::addTransition(const std::string& s1_str, const std::string& s2_str, char a)
{
	if (!transition(s1_str, a).empty())
	{
		if (!isSymbolInAlphabet(a))
			return false;

		if (!stateExists(s1_str) || !stateExists(s2_str))
			return false;

		*getOutputStream() << "Error: DFA " << getID() << ", overwriting transition δ(" << s1_str << ", " << a << ")"
						   << std::endl;
		removeTransitions(s1_str, a);
	}

	return Automaton::addTransition(s1_str, s2_str, a);
}

bool DFA::addTransitions(const std::string& s_str, const std::set<std::string>& output_states, char a)
{
	if (output_states.size() == 1)
	{
		return addTransition(s_str, *output_states.begin(), a);
	}
	else if (output_states.empty())
	{
		return true;
	}

	*getOutputStream() << "Error: DFA " << getID() << ", addTransitions: output_states contains more than one state"
					   << std::endl;
	return false;
}

NFA DFA::toNFA() const
{
	NFA nfa;

	nfa.setAlphabet(getAlphabet());

	for (const auto& state : getAllStates())
		nfa.addState(state, isStateAccepting(state));

	for (const auto& state : getAllStates())
	{
		for (char a : getAlphabet())
			nfa.addTransition(state, *transition(state, a).begin(), a);
	}

	nfa.setStartState(getStartState());

	return nfa;
}

ENFA DFA::toENFA() const
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
			enfa.addTransition(state, *transition(state, a).begin(), a);
	}

	enfa.setStartState(getStartState());

	return enfa;
}

RE DFA::toRE() const
{
	std::set<std::string> states_to_eliminate = getAllStates();
	std::set<std::string> accepting_states;
	std::map<std::string, std::map<std::string, RE*>> current_transitions = transitionsToRE();

	for (const auto& state : states_to_eliminate)
	{
		if (isStateAccepting(state))
			accepting_states.insert(state);
	}

	std::string accepting_state = *accepting_states.begin();

	// remove startstate and acceptingstate from states_to_eliminate
	states_to_eliminate.erase(getStartState());
	states_to_eliminate.erase(accepting_state);

	for (const auto& elistate : states_to_eliminate)
	{
		std::map<std::string, RE*> inputtransitions = getInputTransitionsRE(current_transitions, elistate);
		std::map<std::string, RE*> outputtransitions = current_transitions[elistate];
		for (auto& inputtransition : inputtransitions)
		{
			for (auto& outputtransition : outputtransitions)
			{
				// if outputtransition goes to elistate
				if (outputtransition.first == elistate || inputtransition.first == elistate)
					continue;

				std::vector<RE*> REs;

				// get transition from inputstate to elistate
				REs.push_back(inputtransition.second);

				// get the loop transition on elistate if it exists
				// if multiple loops then do union
				RE* temp_RE = getLoopsRE(current_transitions, elistate);

				if (!temp_RE->empty())
				{
					temp_RE->kleeneStar();
					REs.push_back(temp_RE);
				}

				// get transition from elistate to outputtransition
				REs.push_back(outputtransition.second);

				// concatenate
				RE* new_RE2 = new RE;
				new_RE2->concatenateRE(REs);

				delete temp_RE;

				// put in current_transitions
				if (current_transitions[inputtransition.first].find(outputtransition.first)
					!= current_transitions[inputtransition.first].end())
				{
					RE* new_new_RE = new RE;
					new_new_RE->unionRE({ current_transitions[inputtransition.first][outputtransition.first],
										  new_RE2 });
					delete new_RE2;
					delete current_transitions[inputtransition.first][outputtransition.first];
					current_transitions[inputtransition.first][outputtransition.first] = new_new_RE;
				}
				else
					current_transitions[inputtransition.first][outputtransition.first] = new_RE2;
			}
		}
		// remove elistate
		for (const auto& inputtransition : inputtransitions)
		{
			delete current_transitions[inputtransition.first][elistate];
			current_transitions[inputtransition.first].erase(current_transitions[inputtransition.first].find(elistate));
		}
		for (const auto& transition : current_transitions[elistate])
			delete transition.second;
		current_transitions.erase((current_transitions.find(elistate)));
	}

	std::ostream bitBucket(0);

	// ((R+S(U*)T)*)S(U*)
	RE* RE_R = getLoopsRE(current_transitions, getStartState());
	RE* RE_S = current_transitions[getStartState()][accepting_state];
	RE* RE_T = current_transitions[accepting_state][getStartState()];
	RE* RE_U = getLoopsRE(current_transitions, accepting_state);

	delete current_transitions[getStartState()][getStartState()];
	delete current_transitions[accepting_state][accepting_state];

	RE_U->setOutputStream(bitBucket);
	RE_U->kleeneStar();

	RE final_RE;
	final_RE.setOutputStream(bitBucket);
//	if (RE_R)
//		final_RE = *RE_R;
	RE* temp1 = new RE;
	temp1->setOutputStream(bitBucket);
	temp1->concatenateRE({ RE_S, RE_U, RE_T });
	final_RE.unionRE({ RE_R, temp1 });
	delete temp1;

	final_RE.kleeneStar();
	final_RE.concatenateRE({ RE_S, RE_U });

	delete RE_R;
	delete RE_S;
	delete RE_T;
	delete RE_U;

	final_RE.setAlphabet(getAlphabet());
	final_RE.setEpsilon('e');
	final_RE.addSymbol('e');
	final_RE.setOutputStream(std::cerr);

	return final_RE;
}

void DFA::product(const DFA& dfa1, const DFA& dfa2, bool intersection)
{
	std::pair<std::string, std::string> pair_of_input_states = { dfa1.getStartState(), dfa2.getStartState() };
	std::pair<std::string, std::string> pair_of_output_states;
	std::vector<std::pair<std::string, std::string>> pairs_of_input_states;
	std::vector<std::pair<std::string, std::string>> new_pairs_of_input_states;
	std::string new_state_name;

	setAlphabet(dfa1.getAlphabet());

	new_state_name = "(" + pair_of_input_states.first + "," + pair_of_input_states.second + ")";

	if (intersection)
		addState(new_state_name,
				dfa1.isStateAccepting(pair_of_input_states.first) &&
				dfa2.isStateAccepting(pair_of_input_states.second));
	else
		addState(new_state_name,
				dfa1.isStateAccepting(pair_of_input_states.first) ||
				dfa2.isStateAccepting(pair_of_input_states.second));

	setStartState("(" + pair_of_input_states.first + "," + pair_of_input_states.second + ")");
	pairs_of_input_states.push_back(pair_of_input_states);

	while (!pairs_of_input_states.empty())
	{
		pair_of_input_states = pairs_of_input_states.front();
		pairs_of_input_states.erase(pairs_of_input_states.begin());

		for (char a : getAlphabet())
		{
			// transitions
			pair_of_output_states.first = *dfa1.transition(pair_of_input_states.first, a).begin();
			pair_of_output_states.second = *dfa2.transition(pair_of_input_states.second, a).begin();

			new_state_name = "(" + pair_of_output_states.first + "," + pair_of_output_states.second + ")";

			// adds the state if it doesn't exists in the product dfa
			if (!stateExists(new_state_name))
			{
				if (intersection)
					addState(new_state_name,
							dfa1.isStateAccepting(pair_of_output_states.first)
							&& dfa2.isStateAccepting(pair_of_output_states.second));
				else
					addState(new_state_name,
							dfa1.isStateAccepting(pair_of_output_states.first)
							|| dfa2.isStateAccepting(pair_of_output_states.second));

				new_pairs_of_input_states.push_back(pair_of_output_states);
			}
			addTransition("(" + pair_of_input_states.first + "," + pair_of_input_states.second + ")",
					new_state_name, a);
		}
		pairs_of_input_states
				.insert(pairs_of_input_states.end(), new_pairs_of_input_states.begin(),
						new_pairs_of_input_states.end());
		new_pairs_of_input_states.clear();
	}
	isLegal();
}

bool DFA::isLegal() const
{
	bool is_legal = true;

	if (getStartState().empty())
		is_legal = false;

	for (const auto& state : getAllStates())
	{
		for (const auto& c : getAlphabet())
		{
			std::set<std::string> output_state = transition(state, c);

			if (output_state.empty())
			{
				*getOutputStream() << "Error: DFA " << getID() << " has no transition from state " << state
								   << " with symbol "
								   << c << std::endl;
				is_legal = false;
			}

			if (output_state.size() > 1)
			{
				*getOutputStream() << "Error: DFA " << getID() << " has multiple transitions from state " << state
								   << " with symbol " << c << std::endl;
				is_legal = false;
			}
		}
	}

	if (getEpsilon() != ' ')
	{
		*getOutputStream() << "Error: DFA " << getID() << " has an epsilon symbol '" << getEpsilon() << "'"
						   << std::endl;
		is_legal = false;
	}

	return is_legal;
}

std::map<std::string, std::map<std::string, RE*>> DFA::transitionsToRE() const
{
	std::map<std::string, std::map<std::string, RE*>> new_transitions;

	for (const auto& state : getAllStates())
	{
		for (char symbol : getAlphabet())
		{
			RE* new_RE = new RE;

			new_RE->load(std::string(1, symbol), 'e');
			if (new_transitions[state].find(*transition(state, symbol).begin()) != new_transitions[state].end())
			{
				// union
				RE* new_new_RE = new RE;
				new_new_RE->unionRE({ new_transitions[state][*transition(state, symbol).begin()], new_RE });
				delete new_transitions[state][*transition(state, symbol).begin()];
				delete new_RE;
				new_transitions[state][*transition(state, symbol).begin()] = new_new_RE;
			}
			else
			{
				new_transitions[state][*transition(state, symbol).begin()] = new_RE;
			}
		}
	}

	return new_transitions;
}

std::map<std::string, RE*> DFA::getInputTransitionsRE(const std::map<std::string,
		std::map<std::string, RE*>>& transitionsRE,
		const std::string& target_state)
{
	std::map<std::string, RE*> input_transitions;

	for (const auto& input_transition : transitionsRE)
	{
		for (const auto& output_transition : input_transition.second)
		{
			if (output_transition.first == target_state)
				input_transitions[input_transition.first] = output_transition.second;
		}
	}

	return input_transitions;
}

RE* DFA::getLoopsRE(std::map<std::string, std::map<std::string, RE*>>& transitionsRE,
		const std::string& target_state)
{
	std::vector<RE*> loops_regexes;

	for (auto& output_transition : transitionsRE[target_state])
	{
		if (output_transition.first == target_state)
			loops_regexes.push_back(output_transition.second);
	}
	std::ostream bitBucket(0);

	RE* new_RE = new RE;
	new_RE->setOutputStream(bitBucket);

	new_RE->unionRE(loops_regexes);

//	if (loops_regexes.size() > 1)
//		new_RE->unionRE(loops_regexes);
//	else if (loops_regexes.size() == 1)
//		new_RE = loops_regexes[0];

	return new_RE;
}