#ifndef UABA1_TA_AUTOMATA2__RE_H
#define UABA1_TA_AUTOMATA2__RE_H

#include "Alphabet.h"

class DFA;
class NFA;
class ENFA;

struct checkmemRE
{
	std::set<int> reids;

	void addreid(int _id)
	{
		// 0, 103, 142, 178, 188, 227, 258, 261, 280, 288, 301, 303, 305, 327, 350, 353, 368, 371, 379,
//		if (_id == 103)
//		{
//			std::cout << "OOOOHHH, it's here!!!!!!!!!!1111" << std::endl;
//		}
		reids.insert(_id);
	}
	void removeid(int _id)
	{
		reids.erase(_id);
	}

	void printNotFreed()
	{
//		for (const auto reid : reids)
//		{
//			std::cout << reid << ", ";
//		}
//
//		std::cout << std::endl;
		;
	}
};

static checkmemRE s_checkmemRE;



class RE : public Alphabet
{
public:
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

		int nodeID;
		static int nextnodeID;

		RENode();
		RENode(RENode* _parent, const std::vector<RENode*>& _children, int _node_id, NodeType _nodetype, char _symbol);
		RENode& operator=(const RENode& _renode);
	};
	RENode* start_node = nullptr;
	int nr_of_nodes = 0;
	bool is_empty_string = false;

public:
	RE();
	RE(const std::string& regex_string, char epsilon);
	~RE();
	RE& operator=(const RE& _re);

	bool load(const std::string& regex_string, char epsilon);
	std::string save() const;
	void print() const;
	bool empty() const;

	void varRE(char symbol);
	void unionRE(const std::vector<RE*>& regexes);
	void concatenateRE(const std::vector<RE*>& regexes, bool check_if_this_empty = true);
	void kleeneStar();

	DFA toDFA() const; // finished
	NFA toNFA() const; // finished
	/* regex -> ENFA */
	ENFA toENFA() const;

	bool isLegal(RENode* current_node = nullptr, bool start = true) const;
	std::string genDOT() const;
	bool genImage() const;

public:
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
	void unionOrConcatenation(const std::vector<RE*>& regexes, NodeType node_type, bool check_if_this_empty = true);
};

#endif
