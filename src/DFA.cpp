#include "DFA.h"

bool DFA::load(const std::string& filename)
{
	json DFA_json = loadJsonFile(filename);

	if (DFA_json.empty())
		return false;

	if (DFA_json["type"] != "DFA")
	{
		*getErrorOutputStream() << "Error: " << filename << " is of the type " << DFA_json["type"] << " and not DFA"
								<< std::endl;
		return false;
	}

	if (DFA_json.contains("eps"))
	{
		*getErrorOutputStream() << "Error: \"" << filename << "\" has an invalid format" << std::endl;
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

DFA::DFA(int no_id)
{
	if (no_id)
		decrementID();
}

DFA::DFA(const std::string& json_filename)
{
	DFA::load(json_filename);
}

DFA::DFA(const DFA& dfa1, const DFA& dfa2, bool intersection)
{
	product(dfa1, dfa2, intersection);
}

bool operator==(const DFA& dfa1, const DFA& dfa2)
{
	if (dfa1.getAlphabet() != dfa2.getAlphabet())
		return false;

	std::map<std::string, std::map<std::string, bool>> table = dfa1.getTable(dfa2, false);
	dfa1.printTable(table);

	if (table[dfa1.getStartState()][dfa2.getStartState()] || table[dfa2.getStartState()][dfa1.getStartState()])
		return false;

	return true;
}

bool DFA::addTransition(const std::string& s1_str, const std::string& s2_str, char a)
{
	if (!transition(s1_str, a).empty())
	{
		if (!isSymbolInAlphabet(a))
			return false;

		if (!stateExists(s1_str) || !stateExists(s2_str))
			return false;

		*getErrorOutputStream() << "Error: DFA " << getID() << ", overwriting transition δ(" << s1_str << ", " << a
								<< ")"
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

	*getErrorOutputStream() << "Error: DFA " << getID()
							<< ", addTransitions: output_states contains more than one state"
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
	std::map<std::string, std::map<std::string, RE*>> current_transitions;

	for (const auto& state : states_to_eliminate)
	{
		if (isStateAccepting(state))
			accepting_states.insert(state);
	}

	std::vector<RE*> final_res;

	while (!accepting_states.empty())
	{
		current_transitions = transitionsToRE();
		std::string accepting_state = *accepting_states.begin();
		accepting_states.erase(accepting_states.begin());

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

					if (temp_RE && !temp_RE->empty())
					{
						temp_RE->kleeneStar();
						REs.push_back(temp_RE);
					}

					// get transition from elistate to outputtransition
					REs.push_back(outputtransition.second);

					// concatenate
					RE* new_RE2 = new RE;
					new_RE2->concatenateRE(REs, false);

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
				current_transitions[inputtransition.first].erase(
						current_transitions[inputtransition.first].find(elistate));
			}
			for (const auto& transition : current_transitions[elistate])
				delete transition.second;
			current_transitions.erase((current_transitions.find(elistate)));
		}

		std::ostream bitBucket(0);

		if (getStartState() == accepting_state)
		{
			final_res.push_back(current_transitions[accepting_state][accepting_state]);
			continue;
		}

		// ((R+S(U*)T)*)S(U*)
		RE* RE_R = getLoopsRE(current_transitions, getStartState());
		RE* RE_S = current_transitions[getStartState()][accepting_state];
		RE* RE_T = current_transitions[accepting_state][getStartState()];
		RE* RE_U = getLoopsRE(current_transitions, accepting_state);

		delete current_transitions[getStartState()][getStartState()];
		delete current_transitions[accepting_state][accepting_state];

		RE_U->setOutputStream(bitBucket);
		RE_U->kleeneStar();

		RE* result_RE = new RE;
		result_RE->setOutputStream(bitBucket);

		RE* temp1 = new RE;
		temp1->setOutputStream(bitBucket);
		temp1->concatenateRE({ RE_S, RE_U, RE_T }, false);
		result_RE->unionRE({ RE_R, temp1 });
		delete temp1;

		result_RE->kleeneStar();
		result_RE->concatenateRE({ RE_S, RE_U });

		delete RE_R;
		delete RE_S;
		delete RE_T;
		delete RE_U;

		final_res.push_back(result_RE);
	}
	RE final_RE;
	final_RE.unionRE(final_res);

	for (const auto& result_RE : final_res)
		delete result_RE;

	final_RE.setAlphabet(getAlphabet());
	final_RE.setEpsilon('e');
	final_RE.addSymbol('e');
	final_RE.setOutputStream(std::cerr);

	final_RE.isLegal();

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

DFA DFA::minimize() const
{
	if (!isLegal())
		*getErrorOutputStream() << "Error: Could not minimize DFA " << getID() << ", because it's not legal"
								<< std::endl;

	DFA dfa;
	dfa.setAlphabet(getAlphabet());

	std::map<std::string, std::map<std::string, bool>> table = getTable(DFA(), true);
	std::set<std::set<std::string>> combined_states;

	for (const auto& table2 : table)
	{
		for (const auto& state2 : table2.second)
		{
			if (!state2.second)
			{
				bool added_state = false;
				for (const auto& combined_state : combined_states)
				{
					if ((combined_state.find(table2.first) != combined_state.end()) ||
						(combined_state.find(state2.first) != combined_state.end()))
					{
						std::set<std::string> new_combined_state = combined_state;
						new_combined_state.insert(table2.first);
						new_combined_state.insert(state2.first);

						combined_states.erase(combined_state);
						combined_states.insert(new_combined_state);
						added_state = true;
						break;
					}
				}

				if (!added_state)
				{
					std::set<std::string> new_combined_state;
					new_combined_state.insert(table2.first);
					new_combined_state.insert(state2.first);

					combined_states.insert(new_combined_state);
				}
			}
		}
	}

	std::set<std::string> single_states = getAllStates();

	for (const auto& combined_state : combined_states)
	{
		bool is_start_state = false;
		for (const auto& state : combined_state)
		{
			single_states.erase(state);
			if (state == getStartState())
				is_start_state = true;
		}

		dfa.addState(getSetOfStatesString2(combined_state), isSetOfStatesAccepting(combined_state));
		if (is_start_state)
			dfa.setStartState(getSetOfStatesString2(combined_state));
	}

	for (const auto& state : single_states)
	{
		dfa.addState(getSetOfStatesString2({ state }), isStateAccepting(state));
		if (state == getStartState())
			dfa.setStartState(getSetOfStatesString2({ state }));
	}

	for (const auto& combined_state : combined_states)
	{
		for (char a : getAlphabet())
		{
			std::set<std::string> output_states = transitionSetOfStates(combined_state, a);
			bool transition_added = false;

			for (const auto& _combined_state : combined_states)
			{
				if (_combined_state.find(*output_states.begin()) != _combined_state.end())
				{
					dfa.addTransition(getSetOfStatesString2(combined_state), getSetOfStatesString2(_combined_state),
							a);
					transition_added = true;
				}
			}

			if (!transition_added && output_states.size() == 1)
				dfa.addTransition(getSetOfStatesString2(combined_state), getSetOfStatesString2(output_states), a);
			else if (!transition_added)
				*getErrorOutputStream() << "Error: Could not minimize DFA " << getID() << std::endl;
		}

	}

	for (const auto& state : single_states)
	{
		for (char a : getAlphabet())
		{
			std::string output_state = *transition(state, a).begin();
			bool state_added = false;

			// checks if output_state is in a combined state
			for (const auto& combined_state : combined_states)
			{
				if (combined_state.find(output_state) != combined_state.end())
				{
					dfa.addTransition(getSetOfStatesString2({ state }), getSetOfStatesString2(combined_state), a);
					state_added = true;
					break;
				}
			}

			if (!state_added)
				dfa.addTransition(getSetOfStatesString2({ state }), getSetOfStatesString2({ output_state }), a);
		}
	}

	dfa.isLegal();

	return dfa;
}

void
DFA::printTable(const std::map<std::string, std::map<std::string, bool>>& _table, std::ostream& output_stream) const
{
	std::map<std::string, std::map<std::string, bool>> table;
	if (!_table.empty())
		table = _table;
	else
		table = getTable();

	std::string previous_state;

	for (const auto& table2 : table)
	{
		if (table2 == *(table.begin()))
		{
			previous_state = table2.first;
			continue;
		}

		output_stream << table2.first;

		for (const auto& state2 : table2.second)
		{
			output_stream << "   ";

			if (table[table2.first][state2.first])
				output_stream << "X";
			else
				output_stream << "-";

			if (state2.first == previous_state)
				break;
		}

		previous_state = table2.first;
		output_stream << std::endl;
	}

	for (const auto& table2 : table)
	{
		if (table2 == *(table.begin()))
			output_stream << "    " << table2.first;
		else if (table2 == *(table.rbegin()))
			continue;
		else
			output_stream << "   " << table2.first;
	}

	output_stream << std::endl;
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
				*getErrorOutputStream() << "Error: DFA " << getID() << " has no transition from state " << state
										<< " with symbol "
										<< c << std::endl;
				is_legal = false;
			}

			if (output_state.size() > 1)
			{
				*getErrorOutputStream() << "Error: DFA " << getID() << " has multiple transitions from state " << state
										<< " with symbol " << c << std::endl;
				is_legal = false;
			}
		}
	}

	if (getEpsilon() != ' ')
	{
		*getErrorOutputStream() << "Error: DFA " << getID() << " has an epsilon symbol '" << getEpsilon() << "'"
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

			new_RE->load(std::string(1, symbol), getStandardEpsilon());
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

	return new_RE;
}

std::map<std::string, std::map<std::string, bool>> DFA::getTable(const DFA& dfa2, bool is_minimize) const
{
	std::set<std::string> dfa1_states = getAllStates();
	std::set<std::string> dfa2_states;
	std::set<std::string> all_states = dfa1_states;

	if (is_minimize)
//		decrementID();
		;
	else
	{
		dfa2_states = dfa2.getAllStates();
		all_states.insert(dfa2_states.begin(), dfa2_states.end());
	}

	std::map<std::string, std::map<std::string, bool>> table;

	if (all_states.empty() || all_states.size() == 1)
		return table;

	for (const auto& state : all_states)
	{
		for (const auto& state2 : all_states)
		{
			if (state2 == state)
				continue;

			bool is_state1_accepting;
			bool is_state2_accepting;

			if (dfa1_states.find(state) != dfa1_states.end())
				is_state1_accepting = isStateAccepting(state);
			else
				is_state1_accepting = dfa2.isStateAccepting(state);

			if (dfa1_states.find(state2) != dfa1_states.end())
				is_state2_accepting = isStateAccepting(state2);
			else
				is_state2_accepting = dfa2.isStateAccepting(state2);

			if ((is_state1_accepting && !is_state2_accepting) || (!is_state1_accepting && is_state2_accepting))
			{
				table[state][state2] = true;
				table[state2][state] = true;
			}
			else
			{
				table[state][state2] = false;
				table[state2][state] = false;
			}
		}
	}

	bool new_marks_placed = true;
	std::map<std::string, std::set<std::string>> checked;

	while (new_marks_placed)
	{
		new_marks_placed = false;
		for (const auto& table2 : table)
		{
			for (const auto& state2 : table2.second)
			{
				// if there is a mark
				// maybe put this in a helpfunction
				bool is_checked = state2.second;
				bool is_checked2 = checked[table2.first].find(state2.first) == checked[table2.first].end();
				if (is_checked && is_checked2)
				{
					if (placeTableMark(table2.first, state2.first, table, dfa2))
						new_marks_placed = true;

					checked[table2.first].insert(state2.first);
				}
			}
		}
	}

	return table;
}

bool DFA::placeTableMark(const std::string& s1, const std::string& s2,
		std::map<std::string, std::map<std::string, bool>>& table, const DFA& dfa2, bool is_minimize) const
{
	std::set<std::string> dfa1_states = getAllStates();
	std::set<std::string> dfa2_states;

	if (!is_minimize)
		dfa2_states = dfa2.getAllStates();

	table[s1][s2] = true;
	bool new_marks_placed = false;

	std::set<std::string> rtrans1;
	std::set<std::string> rtrans2;

	for (char c : getAlphabet())
	{
		if (dfa1_states.find(s1) != dfa1_states.end())
			rtrans1 = reverseTransition(s1, c);
		else
			rtrans1 = dfa2.reverseTransition(s1, c);

		for (const auto& state : rtrans1)
		{
			if (dfa1_states.find(s2) != dfa1_states.end())
				rtrans2 = reverseTransition(s2, c);
			else
				rtrans2 = dfa2.reverseTransition(s2, c);

			for (const auto& state2 : rtrans2)
			{
				bool is_state1_accepting;
				bool is_state2_accepting;

				if (dfa1_states.find(state) != dfa1_states.end())
					is_state1_accepting = isStateAccepting(state);
				else
					is_state1_accepting = dfa2.isStateAccepting(state);

				if (dfa1_states.find(state2) != dfa1_states.end())
					is_state2_accepting = isStateAccepting(state2);
				else
					is_state2_accepting = dfa2.isStateAccepting(state2);

				if ((!is_state1_accepting && !is_state2_accepting) || (is_state1_accepting && is_state2_accepting))
				{
					table[state][state2] = true;
					table[state2][state] = true;
					new_marks_placed = true;
				}
			}
		}
	}

	return new_marks_placed;
}
