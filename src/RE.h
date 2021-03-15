#ifndef TA_AUTOMATA_SRC_RE_H
#define TA_AUTOMATA_SRC_RE_H

class DFA;
class NFA;
class ENFA;

class RE
{
	std::set<char> alphabet;
	char epsilon;

	enum NodeType { var, plus, dot, star };

	struct RENode
	{
		NodeType nodetype;
		char symbol = ' ';
		RENode* parent = nullptr;
		std::vector<RENode*> children;
		int node_id = 0;
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
	void save();
	void print();

	DFA toDFA(); // finished
	NFA toNFA(); // finished
	/* regex -> ENFA */
	ENFA toENFA();

	int getID() const;
	std::string genDOT() const;
	bool genImage() const;

private:
//	RENode* parse(const std::string& regex_string, char eps);
	RENode* parse2(const std::string& regex_string2, char eps);
	std::string genDOTRec(RE::RENode* current_node) const;
	std::string removeOuterParentheses(const std::string& regex_string) const;
	bool isValidRE(const std::string& regex_string) const;
	std::string printRec (RE::RENode* current_node) const;

};

#endif //TA_AUTOMATA_SRC_RE_H
