#include "Automaton.h"

const std::string DOT_IMAGES_PATH = "../DOT_images/";
int Automaton::nextID = 0;

Automaton::Automaton()
{
	ID = nextID++;
}

bool Automaton::isSymbolInAlphabet(char a) const
{
	if (alphabet.find(a) != alphabet.end())
		return true;
	std::cerr << "Error: symbol " << a << " is not in alphabet" << std::endl;
	return false;
}

json Automaton::loadStart(const std::string& filename) const
{
	// file input
	std::ifstream input(filename);

	if (!input.good())
	{
		std::cerr << "Error: couldn't open " << filename << std::endl;
		return false;
	}

	json automaton_json;
	input >> automaton_json;
	input.close();

	if (!(automaton_json.contains("type") && automaton_json.contains("alphabet")
		&& automaton_json.contains("states") && automaton_json.contains("transitions")))
	{
		std::cerr << "Error: " << filename << " has an invalid format" << std::endl;
		return false;
	}

	return automaton_json;
}

bool Automaton::loadBaseComponents(const json& automaton_json)
{
	for (const auto& symbol : automaton_json["alphabet"])
	{
		addSymbol(symbol.get<std::string>()[0]);
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

	return true;
}

bool Automaton::addSymbol(char new_symbol)
{
	if (alphabet.find(new_symbol) != alphabet.end())
	{
		std::cerr << "Error: Symbol '" << new_symbol << "' is already in the alphabet" << std::endl;
		return false;
	}
	alphabet.insert(new_symbol);
	return true;
}

bool Automaton::removeSymbol(char symbol)
{
	if (alphabet.find(symbol) == alphabet.end())
	{
		std::cerr << "Error: Couldn't remove symbol '" << symbol << "' from the alphabet" << std::endl;
		return false;
	}
	alphabet.erase(symbol);
	return true;
}

std::set<char> Automaton::getAlphabet() const
{
	return alphabet;
}

void Automaton::setAlphabet(const std::string& new_alphabet)
{
	clearAlphabet();
	for (char symbol : new_alphabet)
	{
		addSymbol(symbol);
	}
}

void Automaton::setAlphabet(const std::set<char>& new_alphabet)
{
	alphabet = new_alphabet;
}

void Automaton::clearAlphabet()
{
	alphabet.clear();
}

int Automaton::getID() const
{
	return ID;
}

void Automaton::print() const
{
	std::cout << std::setw(4) << save() << std::endl;
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

std::string Automaton::getSetOfStatesString(const std::set<std::string>& set_of_states) const
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

std::set<std::string> Automaton::transitionFunctionSetOfStates(const std::set<std::string>& set_of_inputstates, char a) const
{
	std::set<std::string> set_of_output_states;
	for (const auto& input_state : set_of_inputstates)
	{
		for (const auto& output_state : transitionFunction(input_state, a))
			set_of_output_states.insert(output_state);
	}
	return set_of_output_states;
}

bool Automaton::genImage() const
{
	std::string path = DOT_IMAGES_PATH + std::to_string(getID()) + ".dot";

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
