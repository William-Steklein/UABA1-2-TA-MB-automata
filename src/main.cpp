//#include <iostream>
//#include "DFA.h"
//#include "NFA.h"
//#include "ENFA.h"
//#include "RE.h"

/*
 * todo cleaning
 * 		- relook all algoritms and make tests
 * 		- RE fix concatenation child problem => user error handling
 * 		- RE parser remove eps parameter and make a get epsilon function
 * 		- clean or rewrite RE parser
 * 		- make everything that can be made const const
 *
 *
 * todo changes/other
 * 		- deleteFiles() : make directory if it doesn't exists
 * 		- fix deletefiles function so that it doesn't crash when the directory doesn't exists
 *
 */

#include "DFA.h"
#include "RE.h"

using namespace std;

void deleteFiles(const std::string& folderpath);

struct AllocationMetrics
{
	uint32_t TotalAllocated = 0;
	uint32_t TotalFreed = 0;

	uint32_t CurrentUsage() { return TotalAllocated - TotalFreed; }
};

static AllocationMetrics s_AllocationMetrics;

void* operator new(size_t size)
{
	s_AllocationMetrics.TotalAllocated += size;

	return malloc(size);
}

void operator delete(void* memory, size_t size)
{
	s_AllocationMetrics.TotalFreed += size;

	free(memory);
}

static void PrintMemoryUsage()
{
	std::cout << "Memory Usage: " << s_AllocationMetrics.CurrentUsage() << " bytes\n";
}

int main() {
	DFA dfa("../automata_json/DFA8.json");
	DFA dfa2("../automata_json/DFA7.json");


	dfa.printTable();
	DFA mindfa = dfa.minimize();

	dfa.genImage();
	mindfa.genImage();

	mindfa.print();

	cout << boolalpha << (dfa == mindfa) << endl;    // zijn ze equivalent? Zou hier zeker moeten. Dit wordt getest in de volgende vraag, maar hiermee kan je al eens proberen
	cout << boolalpha << (dfa == dfa2) << endl;    // zijn ze equivalent? Zou hier zeker moeten. Dit wordt getest in de volgende vraag, maar hiermee kan je al eens proberen
	return 0;
}