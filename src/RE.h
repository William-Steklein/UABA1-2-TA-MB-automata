#ifndef UABA1_TA_AUTOMATA2__RE_H
#define UABA1_TA_AUTOMATA2__RE_H

class DFA;
class NFA;
class ENFA;

class RE : public Alphabet
{
	enum NodeType
	{
		var, plus, dot, star
	};

	struct RENode
	{
		RENode* parent = nullptr;
		std::vector<RENode*> children;
		int node_id = 0;
		NodeType nodetype = var;
		char symbol = ' ';

		RENode& operator=(RENode _renode);
	};
	RENode* start_node = nullptr;
	int nr_of_nodes = 0;

public:
	RE();
	RE(const std::string& regex_string, char epsilon);
	~RE();

	bool load(const std::string& regex_string, char epsilon);
	std::string save() const;
	void print() const;
	bool empty() const;

	void varRE(char symbol);
	void unionRE(const std::vector<RE*>& regexes);
	void concatenateRE(const std::vector<RE*>& regexes);
	void kleeneStar();

	DFA toDFA() const; // finished
	NFA toNFA() const; // finished
	/* regex -> ENFA */
	ENFA toENFA() const;

	bool isLegal(RENode* current_node = nullptr, bool start = true) const;
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
	void unionOrConcatenation(const std::vector<RE*>& regexes, NodeType node_type);
};

#endif
