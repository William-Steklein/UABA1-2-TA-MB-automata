#include "DFA.h"

//using json = nlohmann::ordered_json;
using json = nlohmann::json;

const std::string DOT_IMAGES_PATH = "../DOT_images/";
int DFA::nextID = 0;

DFA::DFA() {
	id = nextID++;
}

DFA::DFA(const std::string& json_filename)
{
	id = nextID++;
	load(json_filename);
}

DFA::~DFA()
{
	for (const auto& state : states)
		delete state.second;
}

bool DFA::load(const std::string& filename)
{
	// file input
	std::ifstream input(filename);

	if (!input.good())
	{
		std::cerr << "Error: couldn't open " << filename << std::endl;
		return false;
	}

	json DFA_json;
	input >> DFA_json;

	if (!(DFA_json.contains("type") && DFA_json.contains("alphabet")
		  && DFA_json.contains("states") && DFA_json.contains("transitions")))
	{
		std::cerr << "Error: " << filename << " has an invalid format" << std::endl;
		return false;
	}

	if (DFA_json["type"] != "DFA")
	{
		std::cerr << "Error: " << filename << " is of the type " << DFA_json["type"]
				  << " and not DFA" << std::endl;
		return false;
	}

	for (const auto& symbol : DFA_json["alphabet"])
	{
		addSymbol(symbol.get<std::string>()[0]);
	}

	for (const auto& state : DFA_json["states"])
	{
		addState(state["name"].get<std::string>(), state["accepting"].get<bool>());
		if (state["starting"].get<bool>())
			setStartState(state["name"].get<std::string>());
	}

	for (const auto& transition : DFA_json["transitions"])
	{
		addTransition(transition["from"].get<std::string>(), transition["to"].get<std::string>()
		    , transition["input"].get<std::string>()[0]);
	}

	return true;
}

json DFA::save() const
{
	json json_DFA;

	json_DFA["type"] = "DFA";

	json_DFA["alphabet"] = json::array();
	for (auto symbol : alphabet)
		json_DFA["alphabet"].push_back(std::string(1, symbol));

	for (const auto& state : states)
	{
		bool is_start = false;
		if (state.second == start_state) is_start = true;

		json_DFA["states"].push_back({
			{"name", state.first},
			{"starting", is_start},
			{"accepting", state.second->accepting}
		});

		for (const auto& transition : state.second->transitions)
		{
			json_DFA["transitions"].push_back({
				{"from", state.first},
				{"to", transition.second->name},
				{"input", std::string(1, transition.first)}
			});
		}
	}

	return json_DFA;
}

void DFA::print() const
{
	std::cout << std::setw(4) << save() << std::endl;
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

void DFA::addSymbol(char new_symbol)
{
	alphabet.insert(new_symbol);
}

void DFA::removeSymbol(char symbol)
{
	alphabet.erase(symbol);
}

void DFA::setAlphabet(const std::string& new_alphabet)
{
	clearAlphabet();
	for (char symbol : new_alphabet)
	{
		addSymbol(symbol);
	}
}

void DFA::clearAlphabet()
{
	alphabet.clear();
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
	State* state = getState(name);
	if (!state)
		return false;

	if (state == start_state)
		start_state = nullptr;

	states.erase(name);
	delete state;

	return true;
}

bool DFA::setStartState(const std::string& new_start_state_name)
{
	State* new_start_state = getState(new_start_state_name);
	if (!new_start_state)
		return false;

	start_state = new_start_state;
	return true;
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
	{
		std::cout << "Overwriting transition δ(" << s1->name << ", " << a << ")" << std::endl;
	}

	s1->transitions[a] = s2;

	return true;
}

bool DFA::accepts(const std::string& string_w) const
{
	if (!start_state)
	{
		std::cerr << "Error: no start state" << std::endl;
		return false;
	}

	State* current_state = start_state;

	for (char a : string_w)
	{
		if (!isSymbolInAlphabet(a))
			return false;

		auto it = current_state->transitions.find(a);

		// check if there is a transition
		if (it == current_state->transitions.end())
		{
			std::cerr << "Error: δ(" << current_state->name << ", " << a << ") has no output" << std::endl;
			return false;
		}

		current_state = it->second;
	}

	if (current_state->accepting)
		return true;
	else
		return false;
}

bool DFA::isSymbolInAlphabet(char a) const
{
	if (alphabet.find(a) != alphabet.end())
		return true;
	std::cerr << "Error: symbol " << a << " is not in alphabet" << std::endl;
	return false;
}

DFA::State* DFA::getState(const std::string& name) const
{
	auto state = states.find(name);
	if (state != states.end())
		return state->second;

	std::cerr << "Error: couldn't find state with name \"" << name << "\"" << std::endl;
	return nullptr;
}

int DFA::getID() const
{
	return id;
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
			dot += " " + state.first;
	}

	dot += "\n\tnode [shape = circle];\n";

	dot += "\n\tstart -> " + start_state->name;

	for (const auto& state : states)
	{
		for (const auto& transition : state.second->transitions)
		{
			dot += "\n\t" + state.first + " -> " + transition.second->name;
			dot += " [ label = \"" + std::string(1, transition.first) + "\" ];";
		}
	}
	dot += "\n}";

	return dot;
}

bool DFA::genImage() const
{
	std::string path = DOT_IMAGES_PATH + std::to_string(id) + ".dot";

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
	gvRender(gvc, g, "png", fopen((DOT_IMAGES_PATH + std::to_string(id) + "_image.png").c_str(),
			"w"));
	gvFreeLayout(gvc, g);
	agclose(g);

	return (gvFreeContext(gvc));
}
