#ifndef UABA1_TA_AUTOMATA2__NFA_H
#define UABA1_TA_AUTOMATA2__NFA_H

#include "Automaton.h"
#include "DFA.h"
#include "ENFA.h"
#include "RE.h"

class NFA : public Automaton
{
	bool load(const std::string& filename);
	json save() const override;

public:
	NFA() = default;
	NFA(const std::string& json_filename);

	DFA toDFA() const;
	ENFA toENFA() const;
	RE toRE() const;

	bool isLegal() const override;
};

#endif
