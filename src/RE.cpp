#include "DFA.h"
#include "NFA.h"
#include "ENFA.h"
#include "RE.h"

RE::RE()
{

}

RE::~RE()
{

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
