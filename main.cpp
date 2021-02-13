#include <iostream>
#include <unordered_set>
#include "DFA.h"

int main()
{
    DFA dfa;
    std::cout << std::boolalpha << dfa.accepts("0010110100") << std::endl << dfa.accepts("0001") << std::endl;
    return 0;
}

