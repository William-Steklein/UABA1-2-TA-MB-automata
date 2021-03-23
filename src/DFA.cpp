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

std::set<std::string> DFA::getAcceptingStates() const
{
	return std::set<std::string>();
}

bool DFA::setStartState(const std::string& new_start_state_name)
{
	State* new_start_state = getState(new_start_state_name);
	if (!new_start_state)
		return false;

	start_state = new_start_state;
	return true;
}

std::set<std::string> DFA::getAllStates() const
{
	std::set<std::string> all_states;
	for (const auto& state : states)
		all_states.insert(state.first);
	return all_states;
}

bool DFA::isStateAccepting(const std::string& s_name) const
{
	if (stateExists(s_name))
		return getState(s_name)->accepting;
	else
		std::cerr << "Error: DFA " << getID() << " state \"" + s_name + "\" doesn't exists" << std::endl;
	return false;
}

void DFA::setStateAccepting(const std::string& s_name, bool is_accepting) const
{
	State* s = getState(s_name);
	if (s != nullptr)
		s->accepting = is_accepting;
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
		return { getState(s_name)->transitions[a]->name };
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
	std::pair<std::string, std::string> pair_of_input_states = { dfa1.getStartState(), dfa2.getStartState() };
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
			.insert(pairs_of_input_states.end(), new_pairs_of_input_states.begin(), new_pairs_of_input_states.end());
		new_pairs_of_input_states.clear();
	}
	isLegal();
}

NFA DFA::toNFA() const
{
	return NFA();
}

ENFA DFA::toENFA() const
{
	return ENFA();
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
		for (auto& inputtransition : getInputTransitionsRE(current_transitions, elistate))
		{
			for (auto& outputtransition : current_transitions[elistate])
			{
				// if outputtransition == elistate
				if (outputtransition.first == elistate)
					continue;

				std::vector<RE*> REs;

				// get transition from inputstate to elistate
				REs.push_back(inputtransition.second);

				// get the loop transition on elistate if it exists
				// if multiple loops then do union
				if (getLoopsRE(current_transitions, elistate))
				{
					RE* new_RE = new RE;
					new_RE->kleeneStarRE(getLoopsRE(current_transitions, elistate));
					REs.push_back(new_RE);
				}

				// get transition from elistate to outputtransition
				REs.push_back(outputtransition.second);

				// concatenate
				RE* new_RE2 = new RE;
				new_RE2->concatenateRE(REs);

				// put in current_transitions
				if (current_transitions[inputtransition.first].find(outputtransition.first) != current_transitions[inputtransition.first].end())
				{
					RE* new_new_RE = new RE;
					new_new_RE->unionRE( { current_transitions[inputtransition.first][outputtransition.first], new_RE2 } );
					current_transitions[inputtransition.first][outputtransition.first] = new_new_RE;
				}
				else
					current_transitions[inputtransition.first][outputtransition.first] = new_RE2;
			}
		}
		// remove elistate
		current_transitions.erase((current_transitions.find(elistate)));
	}

	// ((R+S(U*)T)*)S(U*)
	RE* RE_R = getLoopsRE(current_transitions, getStartState());

	RE* RE_S = current_transitions[getStartState()][accepting_state];
	RE* RE_T = current_transitions[accepting_state][getStartState()];

	RE* RE_U = getLoopsRE(current_transitions, accepting_state);

	RE* RE_U_star = new RE;
	RE_U_star->kleeneStarRE(RE_U);

	RE* RE_S_concat_U_star = new RE;
	RE_S_concat_U_star->concatenateRE({ RE_S, RE_U_star });

	RE* RE_S_concat_U_star_T = new RE;
	RE_S_concat_U_star_T->concatenateRE({ RE_S_concat_U_star, RE_T });

	RE* RE_R_plus_other = new RE;
	RE_R_plus_other->unionRE({ RE_R, RE_S_concat_U_star_T });

	RE* RE_R_plus_other_star = new RE;
	RE_R_plus_other_star->kleeneStarRE(RE_R_plus_other);

	RE final_RE;
	final_RE.concatenateRE({ RE_R_plus_other_star, RE_S_concat_U_star});

	final_RE.setAlphabet(getAlphabet());
	final_RE.setEpsilon('e');

	return final_RE;
}

//RE DFA::toRERec(const std::string& current_state, const std::string& end_state, std::set<std::string>& visited_states)
//{
//	RE regex;
//	RE result_regex;
//	RE child_regex;
//	std::vector<RE*> child_regexes;
//	std::string next_state;
//
//	if (visited_states.find(current_state) != visited_states.end())
//		return regex;
//	visited_states.insert(current_state);
//
//	// add
//
//	for (char a : getAlphabet())
//	{
//		next_state = *transitionFunction(current_state, a).begin();
//
//		if (next_state == end_state)
//		{
//			// no recursion
//
//			// make var f
//			child_regex.varRE(a);
//		}
//		else if (next_state == current_state)
//		{
//			RE temp_regex1;
//			temp_regex1.varRE(a);
//			child_regex.kleeneStarRE(&temp_regex1);
//		}
//		else
//		{
//			// yes recursion
//
//			RE temp_regex1;
//			temp_regex1.varRE(a);
//
//			RE temp_regex2 = toRERec(*transitionFunction(current_state, a).begin(), end_state, visited_states);
//			child_regex.concatenateRE({ &temp_regex1, &temp_regex2 });
//		}
//
//		if (!child_regex.empty())
//			child_regexes.push_back(&child_regex);
//	}
//
//	// if there are more than one child regex then do union of them
//	if (child_regexes.size() > 1)
//		regex.unionRE(child_regexes);
//
//	return regex;
//}

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

std::set<std::string> DFA::getInputStates(const std::string& s_name) const
{
	std::set<std::string> input_states;

	for (const auto& state : getAllStates())
	{
		for (char symbol : getAlphabet())
		{
			if (*transitionFunction(state, symbol).begin() == s_name)
				input_states.insert(*transitionFunction(state, symbol).begin());
		}
	}

	return input_states;
}

std::set<std::string> DFA::getOutputStates(const std::string& s_name) const
{
	std::set<std::string> output_states;

	for (char symbol : getAlphabet())
		output_states.insert(*transitionFunction(s_name, symbol).begin());

	return output_states;
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
			if (new_transitions[state].find(*transitionFunction(state, symbol).begin()) != new_transitions[state].end())
			{
				// union
				RE* new_new_RE = new RE;
				new_new_RE->unionRE({ new_transitions[state][*transitionFunction(state, symbol).begin()], new_RE });
				new_transitions[state][*transitionFunction(state, symbol).begin()] = new_new_RE;
			}
			else
			{
				new_transitions[state][*transitionFunction(state, symbol).begin()] = new_RE;
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

	if (loops_regexes.size() > 1)
	{
		RE* new_RE = new RE;
		new_RE->unionRE(loops_regexes);
	}
	else if(loops_regexes.size() == 1)
		return loops_regexes[0];
	else
		return nullptr;
}
