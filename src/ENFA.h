#ifndef UABA1_TA_AUTOMATA2__ENFA_H
#define UABA1_TA_AUTOMATA2__ENFA_H

#include "Automaton.h"

class ENFA : public Automaton
{
	bool load(const std::string& filename);
	json save() const override;

public:
	ENFA() = default;
	ENFA(const std::string& json_filename);

	DFA toDFA() const;
	NFA toNFA() const;
	RE toRE() const;

	bool isLegal() const override;
};

#endif
