#ifndef TA_AUTOMATA_SRC_RE_H
#define TA_AUTOMATA_SRC_RE_H

class DFA;
class NFA;
class ENFA;

class RE
{
public:
	RE();
	~RE();

	DFA toDFA(); // finished
	NFA toNFA(); // finished
	/* regex -> ENFA */
	ENFA toENFA();
};

#endif //TA_AUTOMATA_SRC_RE_H
