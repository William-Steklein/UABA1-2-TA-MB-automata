#include <iostream>
#include <unordered_set>
#include <fstream>
#include <iomanip>
#include <graphviz/gvc.h>
#include <stdio.h>
#include <dirent.h>
#include "DFA.h"
#include "NFA.h"
#include "ENFA.h"
#include "RE.h"
#include "json.hpp"


//using json = nlohmann::ordered_json;
using json = nlohmann::json;

// todo: put library source code in project
// todo: check algorithms with testinput from tuyaux
/*
 * todo
 * - implement RE
 * - test new ENFA accept() out
 * - implement modified subset construction
 * - implement regex -> ENFA
 * - implement state elimination DFA -> regex
 * - implement table filling algorithm
 * - unit testing?
 *
 * - put library source code in project
 * - check algorithms with testinput from tuyaux
 * - change toDot(), so that if there are two arrows with the same origin and destination, it will make one arrow and
 * add the symbols with a comma.
 * - maybe change everything with this std::map<std::string, std::map<char, std::set<State*>>> transitions2;
 *
 */

void deleteImages(const std::string& folderpath)
{
	DIR* theFolder = opendir(folderpath.c_str());
	struct dirent* next_file;
	char filepath[256];

	while ((next_file = readdir(theFolder))!=nullptr) {
		sprintf(filepath, "%s/%s", folderpath.c_str(), next_file->d_name);
		remove(filepath);
	}
	closedir(theFolder);
}

int main()
{
	deleteImages("../DOT_images");

//	DFA dfa("../automata_json/DFA.json");
//	DFA dfa2("../automata_json/DFA2.json");
//	DFA dfa3("../automata_json/DFA3.json");
//	DFA dfa4("../automata_json/DFA4.json");
	NFA nfa("../automata_json/NFA.json");
//	NFA nfa2("../automata_json/NFA2.json");
	ENFA enfa("../automata_json/ENFA.json");

//	std::cout << dfa.checkLegality() << std::endl;
//	std::cout << dfa2.checkLegality() << std::endl;
//	std::cout << dfa3.checkLegality() << std::endl;
//	std::cout << dfa4.checkLegality() << std::endl;
//	std::cout << nfa.checkLegality() << std::endl;
//	std::cout << enfa.checkLegality() << std::endl;


//	dfa.genImage();
//	dfa2.genImage();
//	dfa3.genImage();
//	dfa4.genImage();
//	nfa.genImage();
//	nfa2.genImage();
//	DFA nfadfa = nfa.toDFA();
//	nfadfa.print();
//	nfadfa.genImage();
//	nfa.printStats();
//	nfa2.printStats();
	enfa.genImage();
//	enfa.toDFA();
//	enfa.printStats();

//	RE re;
//	re.load("a*b+c", 'e');
//	re.genImage();

//	RE re;
//	std::cout << re.removeOuterParentheses("(kd+e+kk+i)");
//	re.load("(kd+e)(k+i)*(a+b)*",'e');
	// (0+1)*1(0+1)*
	// 0+1((0+1)*1(0+1)*)1
	// e+0(0+1)*+(0+1)*0+1((0+1)*1(0+1)*)1
	// e+0(0+1)*+(0+1)*0+1
	// a(ab)*ba
	// (kd+e+k)i*(k+i)
	// (kd+e)(k+i)*(a+b)*

//	re.genImage();

//	ENFA enfa = re.toENFA();
//	enfa.printStats();
//	// geven true
//	std::cout << std::boolalpha << enfa.accepts("ys") << std::endl;
//	std::cout << std::boolalpha << enfa.accepts("mmyyymmmym") << std::endl;
//	std::cout << std::boolalpha << enfa.accepts("s") << std::endl;
//
//	// geven false
//	std::cout << std::boolalpha << enfa.accepts("ss") << std::endl;
//	std::cout << std::boolalpha << enfa.accepts("ims") << std::endl;
//	std::cout << std::boolalpha << enfa.accepts("mimis") << std::endl;

	return 0;
}