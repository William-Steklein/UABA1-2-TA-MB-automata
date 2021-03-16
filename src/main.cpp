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

/*
 * todo implementation
 * - regex -> ENFA
 * - state elimination DFA -> regex
 * - table filling algorithm
 *
 *
 * todo cleaning
 * - "Overwriting transition δ(..." => overwriting ... to ...
 * - automaton id in all errors
 * - clean or rewrite RE parser
 *
 *
 * todo changes/other
 * - put library source code in project
 * - check algorithms with testinput from tuyaux
 * - change toDot(), so that if there are two arrows with the same origin and destination, it will make one arrow and
 * add the symbols with a comma.
 * - unit testing?
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
//
//	std::cout << dfa.isLegal() << std::endl;
//	std::cout << dfa2.isLegal() << std::endl;
//	std::cout << dfa3.isLegal() << std::endl;
//	std::cout << dfa4.isLegal() << std::endl;
//
//	dfa.genImage();
//	dfa2.genImage();
//	dfa3.genImage();
//	dfa4.genImage();
//
//
//
//	NFA nfa("../automata_json/NFA.json");
//	NFA nfa2("../automata_json/NFA2.json");
//	NFA nfa3("../automata_json/NFA3.json");
//
//	std::cout << nfa.isLegal() << std::endl;
//	std::cout << nfa2.isLegal() << std::endl;
//	std::cout << nfa3.isLegal() << std::endl;
//
//	nfa.genImage();
//	nfa2.genImage();
//	nfa3.genImage();
//
//	DFA nfa_dfa = nfa.toDFA();
//	nfa_dfa.genImage();
//	DFA nfa2_dfa = nfa.toDFA();
//	nfa2_dfa.genImage();
//	DFA nfa3_dfa = nfa3.toDFA();
//	nfa3_dfa.genImage();

	DFA dfa5("../automata_json/DFA5.json");
	dfa5.genImage();
	DFA dfa6("../automata_json/DFA6.json");
	dfa6.genImage();
	DFA product_dfa(dfa5, dfa6, true);
	product_dfa.genImage();


//	ENFA enfa("../automata_json/ENFA.json");
//	ENFA enfa2("../automata_json/ENFA2.json");

//	std::cout << enfa.isLegal() << std::endl;
//	std::cout << enfa2.isLegal() << std::endl;

//	enfa.genImage();
//	enfa2.genImage();

//	DFA enfa_dfa = enfa.toDFA();
//	enfa_dfa.genImage();
//	DFA enfa2_dfa = enfa2.toDFA();
//	enfa2_dfa.genImage();




//	RE re;
//	re.load("(kd+e)(k+i)*(a+b)*", 'e');
//	re.genImage();
//	re.print();
//
//	RE re2;
//	re2.load("(kd+e)(k+i)*(a+b)*",'e');
//	re2.genImage();

//	 (0+1)*1(0+1)*
//	 0+1((0+1)*1(0+1)*)1
//	 e+0(0+1)*+(0+1)*0+1(0+1)*1(0+1)*1
//	 e+0(0+1)*+(0+1)*0+1
//	 a(ab)*ba
//	 (kd+e+k)i*(k+i)
//	 (kd+e)(k+i)*(a+b)*



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