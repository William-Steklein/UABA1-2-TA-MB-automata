#include "DFA.h"
#include "NFA.h"
#include "ENFA.h"
#include "RE.h"

const std::string DOT_IMAGES_PATH = "../DOT_images/";
const std::string DOT_FILES_PATH = "../DOT_files/";

RE::RENode& RE::RENode::operator=(RE::RENode _renode)
{
	if (&_renode != this)
	{
		nodetype = _renode.nodetype;
		symbol = _renode.symbol;
		parent = _renode.parent;

		for (const auto& child : _renode.children)
		{
			RENode* new_child = new RENode;
			*new_child = *child;
			children.push_back(new_child);
		}

		node_id = _renode.node_id;
	}

	return *this;
}

RE::RE()
{
}

RE::RE(const std::string& regex_string, char epsilon)
{
	load(regex_string, epsilon);
}

RE::~RE()
{
//	deleteNode(start_node);
}

void RE::deleteNode(RE::RENode* current_node)
{
	if (!current_node)
		return;
	if (!current_node->children.empty())
	{
		for (const auto& child : current_node->children)
			deleteNode(child);
	}
	delete current_node;
}

bool RE::load(const std::string& regex_string, char _epsilon)
{
	setEpsilon(_epsilon);
	addSymbol(_epsilon);
	start_node = parse(regex_string, _epsilon);

	if (start_node)
		return true;
	return false;
}

RE::RENode* RE::parse(const std::string& regex_string2, char eps)
{
	std::vector<std::string> string_list;
	std::vector<std::string> dotstring_list;
	std::string current_string;
	std::string current_dotstring;
	int indent = 0;
	bool is_in_substring = false;
	bool is_plus = false;
	bool is_dot = false;

	std::string regex_string = removeOuterParentheses(regex_string2);

	// var
	if (regex_string.size() == 1)
	{
		RENode* node = new RENode{
			nullptr,
			{},
			nr_of_nodes,
			var,
			regex_string[0] };

		nr_of_nodes++;

		addSymbol(regex_string[0]);

		return node;
	}

	// var*
	if (regex_string.size() == 2 && regex_string.back() == '*')
	{
		RENode* node = new RENode{
			nullptr,
			{ parse(regex_string.substr(0, 1), eps) },
			nr_of_nodes,
			star,
			' ' };

		nr_of_nodes++;

		node->children[0]->parent = node;
		return node;
	}

	// (regex)*
	if (regex_string.back() == '*' && regex_string[0] == '(' && regex_string[regex_string.size() - 2] == ')'
		&& isValidRE(regex_string.substr(1, regex_string.size() - 3)))
	{
		RENode* node = new RENode{
			nullptr,
			{ parse(regex_string.substr(1, regex_string.size() - 3), eps) },
			nr_of_nodes,
			star,
			' ' };

		nr_of_nodes++;
		node->children[0]->parent = node;
		return node;
	}

	for (int i = 0; i < regex_string.size(); i++)
	{
		char current_char = regex_string[i];

		if (current_char == '+')
		{
			if (!is_in_substring)
			{
				if (!current_string.empty())
				{
					string_list.push_back(current_string);
					current_string.clear();
				}
				continue;
			}
		}
		else if (current_char == '*')
		{
			if (!is_in_substring && i < regex_string.size() - 1)
			{
				char next_char = regex_string[i + 1];
				if (next_char == '+')
				{
					is_plus = true;
					is_dot = false;
				}
				else if (next_char != ')' && !is_plus)
				{
					if (!is_dot)
					{
						current_dotstring.clear();
						current_dotstring = current_string;
						is_dot = true;
					}
					else
					{
						dotstring_list.push_back(current_dotstring + '*');
						current_dotstring.clear();
						current_string += '*';
						continue;
					}
				}
					// var
				else
				{
					current_string += '*';
					continue;
				}
				if (is_dot)
				{
					current_dotstring += '*';
					current_string += '*';
				}
				else
				{
					string_list.push_back(current_string + '*');
					current_string.clear();
				}
				continue;
			}
		}
		else if (current_char == '(')
		{
			if (indent == 0)
			{
				is_in_substring = true;
				indent++;

				if (is_dot)
				{
					dotstring_list.push_back(current_dotstring);
					current_dotstring.clear();
				}
			}
			else
				indent++;
		}
		else if (current_char == ')')
		{
			if (indent == 1 && i < regex_string.size() - 1)
			{
				indent--;
				is_in_substring = false;
				char next_char = regex_string[i + 1];

				if (next_char == '+')
				{
					is_plus = true;
					is_dot = false;
				}
				else if (next_char == '(' && !is_plus)
				{
					if (!is_dot)
					{
						current_dotstring = current_string;
						is_dot = true;
					}
				}
				else if (next_char == '*')
				{
					if (is_dot)
					{
						current_dotstring += ')';
					}
					current_string += ')';

					continue;
				}
				else if (next_char != ')')
				{
					if (!is_plus)
					{
						if (!is_dot)
						{
							current_dotstring.clear();
							current_dotstring = current_string;
							is_dot = true;
						}
						else
						{
							dotstring_list.push_back(current_dotstring + ')');
							current_dotstring.clear();
							current_string += ')';
							continue;
						}
					}
					else
					{
						current_string += ')';
						continue;
					}
				}
				if (is_dot)
				{
					current_dotstring += ')';
					current_string += ')';
				}
				else
				{
					string_list.push_back(current_string + ')');
					current_string.clear();
				}

				continue;
			}
			else
			{
				indent--;
			}
		}
			// var
		else
		{
			if (!is_in_substring && i < regex_string.size() - 1)
			{
				char next_char = regex_string[i + 1];
				if (next_char == '+')
				{
					is_plus = true;
					is_dot = false;
				}
				else if (next_char == '*')
				{
					if (is_dot)
					{
						dotstring_list.push_back(current_dotstring);
						current_dotstring.clear();
					}
				}
				else if (next_char != ')' && !is_plus)
				{
					if (!is_dot)
					{
						current_dotstring.clear();
						is_dot = true;
					}
					else
					{
						dotstring_list.push_back(current_dotstring);
						current_dotstring.clear();
					}
				}
			}
			else if (!is_in_substring)
			{
				if (is_dot)
				{
					dotstring_list.push_back(current_dotstring);
					current_dotstring.clear();
				}
			}
		}
		current_string += current_char;

		if (is_dot)
			current_dotstring += current_char;
	}

	if (is_plus)
	{
		if (!current_string.empty())
			string_list.push_back(current_string);
		std::vector<RENode*> children;

		children.reserve(string_list.size() + 1);
		for (const auto& subtree : string_list)
		{
			if (!subtree.empty())
				children.push_back(parse(subtree, getEpsilon()));
		}

		RENode* node = new RENode{
			nullptr,
			children,
			nr_of_nodes,
			plus,
			' ' };

		nr_of_nodes++;

		// link children with parent
		for (const auto& child : node->children)
			child->parent = node;

		return node;
	}

	if (is_dot)
	{
		if (!current_dotstring.empty())
			dotstring_list.push_back(current_dotstring);
		std::vector<RENode*> children;

		children.reserve(dotstring_list.size() + 1);
		for (const auto& dot_subtree : dotstring_list)
		{
			if (!dot_subtree.empty())
				children.push_back(parse(dot_subtree, getEpsilon()));
		}

		RENode* node = new RENode{
			nullptr,
			children,
			nr_of_nodes,
			dot,
			' ' };

		nr_of_nodes++;

		// link children with parent
		for (const auto& child : node->children)
		{
			child->parent = node;
		}

		return node;
	}

	return nullptr;
}

std::string RE::save() const
{
	return printRec(start_node);
}

void RE::print() const
{
	std::cout << printRec(start_node) << std::endl;
}

bool RE::empty() const
{
	if (!start_node)
		return true;
	return false;
}

void RE::varRE(char symbol)
{
	deleteNode(start_node);

	start_node = new RENode{ nullptr,
							 {},
							 0,
							 var,
							 symbol };
}

void RE::unionRE(const std::vector<RE*>& regexes)
{
	unionOrConcatenation(regexes, plus);
}

void RE::concatenateRE(const std::vector<RE*>& regexes)
{
	unionOrConcatenation(regexes, dot);
}

void RE::unionOrConcatenation(const std::vector<RE*>& regexes, NodeType node_type)
{
	std::vector<RENode*> start_nodes;
	if (!empty())
		start_nodes.push_back(start_node);

	for (const auto& regex : regexes)
	{
		if (regex && !regex->empty())
			start_nodes.push_back(regex->start_node);
	}
	if (start_nodes.empty())
	{
		std::string node_type_string = node_type==dot ? "union" : "concatenation";
		std::cerr << "Error: empty " + node_type_string << std::endl;
		return;
	}
	else if (start_nodes.size() == 1)
	{
		start_node = new RENode;
		*start_node = *start_nodes[0];
		return;
	}

	start_node = new RENode{ nullptr,
							 {},
							 0,
							 node_type,
							 ' '};

	for (const auto& node : start_nodes)
	{
		if (node->nodetype != node_type)
		{
			RENode* new_child = new RENode;
			*new_child = *node;

			new_child->parent = start_node;
			start_node->children.push_back(new_child);
		}
		else
		{
			for (const auto& child_regex : node->children)
			{
				RENode* new_child = new RENode;
				*new_child = *child_regex;

				child_regex->parent = start_node;
				start_node->children.push_back(new_child);
			}
		}
	}

	resetRENodeIDs(start_node);
}

void RE::kleeneStar()
{
	if (empty())
	{
		std::cerr << "Error: kleene star on empty regex" << std::endl;
		return;
	}

	if (start_node->nodetype == star)
		return;

	RENode* child_node = start_node;

	start_node = new RENode{ nullptr,
							 { child_node },
							 0,
							 star,
							 ' '};

	resetRENodeIDs(start_node);
}

std::string RE::printRec(RE::RENode* current_node, bool is_base) const
{
	std::string current_regex_string;
	std::string child_regex_string;

	if (current_node->nodetype == plus)
	{
		if (!is_base)
			current_regex_string += "(";
		for (auto it = current_node->children.begin(); it != current_node->children.end(); it++)
		{
			if (it != current_node->children.begin())
				current_regex_string += "+";
			current_regex_string += printRec(*it, false);
		}
		if (!is_base)
			current_regex_string += ")";
	}
	else if (current_node->nodetype == dot)
	{
		for (const auto& child_node : current_node->children)
			current_regex_string += printRec(child_node, false);
	}
	else if (current_node->nodetype == star)
	{
		if (current_node->children[0]->nodetype == dot && current_node->children[0]->children.size() > 1)
			current_regex_string += "(" + printRec(current_node->children[0], false) + ")*";
		else
			current_regex_string += printRec(current_node->children[0], false) + "*";
	}
	else
		current_regex_string = current_node->symbol;

	return current_regex_string;
}

DFA RE::toDFA() const
{
	return toENFA().toDFA();
}

NFA RE::toNFA() const
{
	return toENFA().toNFA();
}

ENFA RE::toENFA() const
{
	ENFA enfa;
	enfa.setAlphabet(getAlphabet());
	enfa.setEpsilon(getEpsilon());
	std::tuple<std::string, std::string, int> return_tuple = toENFARec(enfa, start_node);
	enfa.setStartState(std::get<0>(return_tuple));
	enfa.setStateAccepting(std::get<1>(return_tuple), true);

	return enfa;
}

std::tuple<std::string, std::string, int> RE::toENFARec(ENFA& enfa,
	RENode* current_node,
	int nr_of_enfanodes) const
{
	std::tuple<std::string, std::string, int> child_return_tuple;
	std::string start_state;
	std::string end_state;
	int current_nr_of_enfanodes = nr_of_enfanodes;

	if (current_node->nodetype == plus)
	{
		current_nr_of_enfanodes++;
		start_state = std::to_string(current_nr_of_enfanodes);
		enfa.addState(start_state, false);

		current_nr_of_enfanodes++;
		end_state = std::to_string(current_nr_of_enfanodes);
		enfa.addState(end_state, false);

		for (const auto& child_node : current_node->children)
		{
			child_return_tuple = toENFARec(enfa, child_node, current_nr_of_enfanodes);
			current_nr_of_enfanodes = std::get<2>(child_return_tuple);

			enfa.addTransition(start_state, std::get<0>(child_return_tuple), enfa.getEpsilon());
			enfa.addTransition(std::get<1>(child_return_tuple), end_state, enfa.getEpsilon());
		}
	}
	else if (current_node->nodetype == dot)
	{
		child_return_tuple = toENFARec(enfa, current_node->children.front(), current_nr_of_enfanodes);
		current_nr_of_enfanodes = std::get<2>(child_return_tuple);

		start_state = std::get<0>(child_return_tuple);
		std::string start_state_chain = std::get<1>(child_return_tuple);
		std::string end_state_chain;

		for (int i = 1; i < current_node->children.size(); i++)
		{
			child_return_tuple = toENFARec(enfa, current_node->children[i], current_nr_of_enfanodes);
			current_nr_of_enfanodes = std::get<2>(child_return_tuple);

			end_state_chain = std::get<0>(child_return_tuple);

			enfa.addTransition(start_state_chain, end_state_chain, enfa.getEpsilon());

			start_state_chain = std::get<1>(child_return_tuple);
		}

		end_state = std::get<1>(child_return_tuple);
	}
	else if (current_node->nodetype == star)
	{
		current_nr_of_enfanodes++;
		start_state = std::to_string(current_nr_of_enfanodes);
		enfa.addState(start_state, false);

		current_nr_of_enfanodes++;
		end_state = std::to_string(current_nr_of_enfanodes);
		enfa.addState(end_state, false);

		child_return_tuple = toENFARec(enfa, current_node->children[0], current_nr_of_enfanodes);
		current_nr_of_enfanodes = std::get<2>(child_return_tuple);

		enfa.addTransition(start_state, std::get<0>(child_return_tuple), enfa.getEpsilon());
		enfa.addTransition(start_state, end_state, enfa.getEpsilon());
		enfa.addTransition(std::get<1>(child_return_tuple), end_state, enfa.getEpsilon());
		enfa.addTransition(std::get<1>(child_return_tuple), std::get<0>(child_return_tuple), enfa.getEpsilon());
	}
	else if (current_node->nodetype == var)
	{
		current_nr_of_enfanodes++;
		start_state = std::to_string(current_nr_of_enfanodes);
		enfa.addState(start_state, false);

		current_nr_of_enfanodes++;
		end_state = std::to_string(current_nr_of_enfanodes);
		enfa.addState(end_state, false);

		enfa.addTransition(start_state, end_state, current_node->symbol);
	}

	return { start_state, end_state, current_nr_of_enfanodes };
}

std::string RE::genDOTRec(RE::RENode* current_node) const
{
	std::string dot_string;
	if (current_node->nodetype == plus)
		dot_string += "\t" + std::to_string(current_node->node_id) + " [label=\"+\"];\n";
	else if (current_node->nodetype == dot)
		dot_string += "\t" + std::to_string(current_node->node_id) + " [label=\".\"];\n";
	else if (current_node->nodetype == star)
		dot_string += "\t" + std::to_string(current_node->node_id) + " [label=\"*\"];\n";
	else if (current_node->nodetype == var)
		dot_string += "\t" + std::to_string(current_node->node_id) + " [label=\"" + current_node->symbol + "\"];\n";

	for (const auto& child_node : current_node->children)
	{
		dot_string += genDOTRec(child_node);
		dot_string += "\t" + std::to_string(current_node->node_id) + " -> " +
			std::to_string(child_node->node_id) + "\n";
	}

	return dot_string;
}

bool RE::isLegal(RENode* current_node, bool start) const
{
	bool is_legal = true;

	if (start)
	{
		current_node = start_node;
		if (!current_node)
		{
			*getOutputStream() << "Error: RE " << getID() << " has no start node" << std::endl;
			return false;
		}
	}

	if (current_node->nodetype == star && current_node->children.size() > 1)
	{
		*getOutputStream() << "Error: RE " << getID() << " has a star node with more than 1 children" << std::endl;
		is_legal = false;
	}

	for (const auto& child : current_node->children)
	{
		if (current_node->nodetype == star && child->nodetype == star)
		{
			*getOutputStream() << "Error: RE " << getID() << " has a star node under a star node" << std::endl;
			is_legal = false;
		}

		if (current_node->nodetype == dot && child->nodetype == dot)
		{
			*getOutputStream() << "Error: RE " << getID() << " has a dot node under a dot node" << std::endl;
			is_legal = false;
		}
	}

	return is_legal;
}

std::string RE::genDOT() const
{
	std::string dot;

	// header
	dot += "digraph RE {\n";

	// body
	dot += genDOTRec(start_node);

	dot += "}";

	return dot;
}

bool RE::genImage() const
{
	std::string path = DOT_FILES_PATH + "RE_" + std::to_string(getID()) + ".dot";

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
	gvRender(gvc, g, "png", fopen((DOT_IMAGES_PATH + "RE_"
		+ std::to_string(getID()) + "_image.png").c_str(), "w"));
	gvFreeLayout(gvc, g);
	agclose(g);

	return (gvFreeContext(gvc));
}

std::string RE::removeOuterParentheses(const std::string& regex_string) const
{
	std::string new_regex_string = regex_string;
	int indent = 0;
	bool first_parentheses = false;

	for (int i = 0; i < new_regex_string.size(); i++)
	{
		if (new_regex_string[i] == '(')
		{
			indent++;
			if (i == 0)
				first_parentheses = true;
		}
		else if (new_regex_string[i] == ')')
		{
			indent--;
			if (indent == 0 && first_parentheses)
			{
				if (i != new_regex_string.size() - 1)
					first_parentheses = false;
				else
					new_regex_string = new_regex_string.substr(1, new_regex_string.size() - 2);
			}
		}
	}

	return new_regex_string;
}

bool RE::isValidRE(const std::string& regex_string) const
{
	int indent = 0;
	for (char c : regex_string)
	{
		if (c == '(')
			indent++;
		else if (c == ')')
		{
			if (indent == 0)
				return false;
			indent--;
		}
	}

	if (indent == 0)
		return true;
	else
		return false;
}

int RE::resetRENodeIDs(RENode* current_node, bool is_start)
{
	if (is_start)
		nr_of_nodes = 0;

	current_node->node_id = nr_of_nodes;
	nr_of_nodes++;

	for (const auto& child_node : current_node->children)
		nr_of_nodes = resetRENodeIDs(child_node, false);

	return nr_of_nodes;
}
