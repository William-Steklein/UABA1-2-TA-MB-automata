//#include <iostream>
//#include "DFA.h"
//#include "NFA.h"
//#include "ENFA.h"
//#include "RE.h"

/*
 * todo cleaning
 * 		- remove hardcoded stuff from inginious output
 * 		- relook all algoritms and make tests
 * 		- RE fix concatenation child problem => user error handling
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

	deleteFiles("../DOT_files");
	deleteFiles("../DOT_images");

//	DFA dfa("../automata_json/DFA11.json");
////	dfa.genImage();
//
//	DFA dfa2("../automata_json/DFA7.json");
//
////	dfa.printTable();
//	DFA mindfa = dfa.minimize();
//	mindfa.printTable();
//
////	mindfa.genImage();
//
////	mindfa.print();
//
//	cout << boolalpha << (dfa == mindfa) << endl;    // zijn ze equivalent? Zou hier zeker moeten. Dit wordt getest in de volgende vraag, maar hiermee kan je al eens proberen
////	cout << boolalpha << (dfa == dfa2) << endl;    // false

//	RE re0("(kk)*(q+kq)(k)*q(q)*k(q+k(q)*k)*+(kk)*k", 'e');
//
//	DFA dfa0 = re0.toDFA();
//	dfa0.minimize();
//	dfa0.renameStates();
////	dfa0.genImage();
//
//
//	RE re00 = dfa0.toRE();
//	re00.print();
//
//	DFA dfa1 = re00.toDFA();
//	dfa1.minimize();
//	dfa1.renameStates();
////	dfa1.genImage();
//
//	RE re000 = dfa1.toRE();
//	re000.print();


//	DFA dfa7("../automata_json/DFA11.json");
////	dfa7.genImage();
//
//	RE re = dfa7.toRE();
//
//	re.print();


//	RE re2("a+b", 'e');
//	RE re3("cd", 'e');
//	RE re5;
//
//	re3.kleeneStar();
//
//	RE re4;
//
//	re4.kleeneStar();
//
//	re4.concatenateRE({ &re2, &re3 }, false);
//
//	re4.unionRE({ &re5 });
//
//	re4.print();

	DFA dfa("../automata_json/DFA4.json");
	DFA mindfa = dfa.minimize();
	dfa.printTable();
	mindfa.print();
	cout << boolalpha << (dfa == mindfa) << endl;    // zijn ze equivalent? Zou hier zeker moeten. Dit wordt getest in de volgende vraag, maar hiermee kan je al eens proberen
	return 0;

	return 0;
}