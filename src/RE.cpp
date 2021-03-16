#include "DFA.h"
#include "NFA.h"
#include "ENFA.h"
#include "RE.h"

const std::string DOT_IMAGES_PATH = "../DOT_images/";
int RE::nextID = 0;

RE::RE()
{
	ID = nextID++;
}

RE::RE(const std::string& regex_string, char epsilon)
{
	ID = nextID++;
	load(regex_string, epsilon);
}

RE::~RE()
{
	deleteNode(start_node);
}

void RE::deleteNode(RE::RENode* current_node)
{
	if (!current_node->children.empty())
	{
		for (const auto& child : current_node->children)
			deleteNode(child);
	}
	delete current_node;
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
			var,
			regex_string[0],
			nullptr,
			{},
			nr_of_nodes};

		nr_of_nodes++;

		return node;
	}

	// var*
	if (regex_string.size() == 2 && regex_string.back() == '*')
	{
		RENode* node = new RENode{
			star,
			' ',
			nullptr,
			{ parse(regex_string.substr(0, 1), eps) },
			nr_of_nodes};

		nr_of_nodes++;

		node->children[0]->parent = node;
		return node;
	}

	// (regex)*
	if (regex_string.back() == '*' && regex_string[0] == '(' && regex_string[regex_string.size() - 2] == ')'
		&& isValidRE(regex_string.substr(1, regex_string.size() - 3)))
	{
		RENode* node = new RENode{
			star,
			' ',
			nullptr,
			{ parse(regex_string.substr(1, regex_string.size() - 3), eps) },
			nr_of_nodes};

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
					is_plus = true; is_dot = false;
				}
				else if (next_char != ')' && !is_plus)
				{
					if (!is_dot)
					{
						current_dotstring.clear();
						current_dotstring = current_string;
						is_dot = true;
					} else
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
				} else
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
			if (indent == 1  && i < regex_string.size() - 1)
			{
				indent--;
				is_in_substring = false;
				char next_char = regex_string[i + 1];

				if (next_char == '+')
				{
					is_plus = true; is_dot = false;
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
					if (!is_plus) {
						if (!is_dot)
						{
							current_dotstring.clear();
							current_dotstring = current_string;
							is_dot = true;
						} else
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
				} else
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
					is_plus = true; is_dot = false;
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
					} else
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
				children.push_back(parse(subtree, epsilon));
		}

		RENode* node = new RENode{
			plus,
			' ',
			nullptr,
			children,
			nr_of_nodes};

		nr_of_nodes++;

		// link children with parent
		for (const auto& child : node->children)
		{
			child->parent = node;
		}

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
				children.push_back(parse(dot_subtree, epsilon));
		}

		RENode* node = new RENode{
			dot,
			' ',
			nullptr,
			children,
			nr_of_nodes};

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

bool RE::load(const std::string& regex_string, char _epsilon)
{
	start_node = parse(regex_string, _epsilon);

	epsilon = _epsilon;

	if (start_node)
		return true;
	return false;
}

std::string RE::save()
{
	return printRec(start_node);
}

void RE::print()
{
	std::cout << printRec(start_node) << std::endl;
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

DFA RE::toDFA()
{
	return toENFA().toDFA();
}

NFA RE::toNFA()
{
	return toENFA().toNFA();
}

ENFA RE::toENFA()
{
	return ENFA();
}

int RE::getID() const
{
	return ID;
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
	std::string path = DOT_IMAGES_PATH + "RE_" + std::to_string(getID()) + ".dot";

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
	+ std::to_string(getID()) + "_image.png").c_str(),"w"));
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
