#ifndef TA_AUTOMATA_SRC_RE_H
#define TA_AUTOMATA_SRC_RE_H

class DFA;
class NFA;
class ENFA;

class RE
{
	std::set<char> alphabet;
	char epsilon;

	enum NodeType
	{
		var, plus, dot, star
	};

	struct RENode
	{
		NodeType nodetype = var;
		char symbol = ' ';
		RENode* parent = nullptr;
		std::vector<RENode*> children;
		int node_id = 0;

		// copy assignment
		RENode& operator=(RENode other)
		{
//			std::cout << "Copying RENode" << std::endl;
			nodetype = other.nodetype;
			symbol = other.symbol;
			parent = other.parent;

			for (const auto& child : other.children)
			{
				RENode* new_child = new RENode;
				*new_child = *child;
				children.push_back(new_child);
			}

			node_id = other.node_id;

			return *this;
		}
	};
	RENode* start_node = nullptr;
	int nr_of_nodes = 0;

	int ID;
	static int nextID;

public:
	RE();
	RE(const std::string& regex_string, char epsilon);
	~RE();

	bool load(const std::string& regex_string, char epsilon);
	std::string save() const;
	void print() const;
	bool empty() const;

	void setAlphabet(const std::set<char>& _alphabet);
	void setEpsilon(char _epsilon);

	void varRE(char symbol);
	void unionRE(const std::vector<RE*>& regexes);
	void concatenateRE(const std::vector<RE*>& regexes);
	void kleeneStarRE(RE* regex);

	DFA toDFA() const; // finished
	NFA toNFA() const; // finished
	/* regex -> ENFA */
	ENFA toENFA() const;

	int getID() const;
	std::string genDOT() const;
	bool genImage() const;

private:
	void deleteNode(RE::RENode* current_node);
	RENode* parse(const std::string& regex_string2, char eps);
	std::string genDOTRec(RE::RENode* current_node) const;
	std::string removeOuterParentheses(const std::string& regex_string) const;
	bool isValidRE(const std::string& regex_string) const;
	std::string printRec(RE::RENode* current_node, bool is_base = true) const;
	std::tuple<std::string, std::string, int> toENFARec(ENFA& enfa,
		RENode* current_node,
		int nr_of_enfanodes = 0) const;
	int resetRENodeIDs(RENode* current_node, bool is_start = true);

};

#endif //TA_AUTOMATA_SRC_RE_H
