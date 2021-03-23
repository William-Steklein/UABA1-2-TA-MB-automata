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
 * - state elimination DFA -> regex
 * - table filling algorithm
 * - copy assignment operator overloader for every class
 *
 *
 * todo cleaning
 * - change or remove epsilon in (DFA) transitionstoRE
 * - RE parser remove eps parameter and make a get epsilon function
 * - RE alphabet getters and setters
 * - "Overwriting transition δ(..." => overwriting ... to ...
 * - automaton id in all errors
 * - clean or rewrite RE parser
 * - make everything that can be made const const
 * - MEMORY LEAKS!!!!!!!!!!!!!! AAAAAAAAAAAAAAAAAAAAAAAAAAAAAaaaaaaaa
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

void deleteFiles(const std::string& folderpath)
{
	DIR* theFolder = opendir(folderpath.c_str());
	struct dirent* next_file;
	char filepath[512];

	while ((next_file = readdir(theFolder))!=nullptr) {
		sprintf(filepath, "%s/%s", folderpath.c_str(), next_file->d_name);
		remove(filepath);
	}
	closedir(theFolder);
}

int main()
{
	deleteFiles("../DOT_images");
	deleteFiles("../DOT_files");


//	DFA dfa("../automata_json/DFA.json");
//	std::cout << dfa.isLegal() << std::endl;
//	dfa.genImage();
//
//	DFA dfa2("../automata_json/DFA2.json");
//	std::cout << dfa2.isLegal() << std::endl;
//	dfa2.genImage();
//
//	DFA dfa3("../automata_json/DFA3.json");
//	std::cout << dfa3.isLegal() << std::endl;
//	dfa3.genImage();
//
//	DFA dfa4("../automata_json/DFA4.json");
//	std::cout << dfa4.isLegal() << std::endl;
//	dfa4.genImage();
//
//	DFA dfa6("../automata_json/DFA6.json");
//	std::cout << dfa6.isLegal() << std::endl;
//	dfa6.genImage();
//
//	DFA dfa7("../automata_json/DFA7.json");
//	std::cout << dfa7.isLegal() << std::endl;
//	dfa7.genImage();



//	NFA nfa("../automata_json/NFA.json");
	NFA nfa2("../automata_json/NFA2.json");
	NFA nfa3("../automata_json/NFA3.json");
//
//	std::cout << nfa.isLegal() << std::endl;
//	std::cout << nfa2.isLegal() << std::endl;
	std::cout << nfa3.isLegal() << std::endl;

//	nfa.genImage();
//	nfa2.genImage();
	nfa3.genImage();

//	DFA nfa_dfa = nfa.toDFA();
//	nfa_dfa.genImage();
//	DFA nfa2_dfa = nfa.toDFA();
//	nfa2_dfa.genImage();
	DFA nfa3_dfa = nfa3.toDFA();
	nfa3_dfa.genImage();

	RE nfa3_dfa_RE = nfa3_dfa.toRE();
	nfa3_dfa_RE.genImage();

//	DFA dfa5("../automata_json/DFA5.json");
//	dfa5.genImage();
////	DFA dfa6("../automata_json/DFA6.json");
////	dfa6.genImage();
//	DFA product_dfa(dfa5, dfa6, true);
//	product_dfa.genImage();


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



//	// (0+1+2)*1(0+1)
//	RE re;
//	re.load("a+b", 'e');
//	re.genImage();
//	re.print();
//
//	// (kd+e)(k+i)*(a+b)*
//	RE re2;
//	re2.load("c+d",'e');
//	re2.genImage();
//	re2.print();
//
//	RE re3;
////	re3.unionRE({ &re, &re2 });
//	re3.kleeneStarRE(&re);
//
////	re.start_node->children[2]->children[1]->symbol = 'x';
////	re.genImage();
//
//
//	re3.genImage();
//	re3.print();
//
//	std::cout << "Hello World!" << std::endl;

//	 (0+1)*1(0+1)*
//	 0+1((0+1)*1(0+1)*)1
//	 e+0(0+1)*+(0+1)*0+1(0+1)*1(0+1)*1
//	 e+0(0+1)*+(0+1)*0+1
//	 a(ab)*ba
//	 (kd+e+k)i*(k+i)
//	 (kd+e)(k+i)*(a+b)*


//	// (m+y)*+(e+y+m+i)s
//	RE re("(m+y)*+(e+y+m+i)s",'e');
//	re.genImage();
//	ENFA enfa = re.toENFA();
//	enfa.genImage();
//	enfa.printStats();
//	DFA enfa_dfa = enfa.toDFA();
//	enfa_dfa.genImage();
//	// geven true
//	std::cout << std::boolalpha << enfa.accepts("ys") << std::endl;
//	std::cout << std::boolalpha << enfa.accepts("mmyyymmmym") << std::endl;
//	std::cout << std::boolalpha << enfa.accepts("s") << std::endl;
//
//	// geven false
//	std::cout << std::boolalpha << enfa.accepts("ss") << std::endl;
//	std::cout << std::boolalpha << enfa.accepts("ims") << std::endl;
//	std::cout << std::boolalpha << enfa.accepts("mimis") << std::endl;

//	RE testRE = dfa7.toRE();
//	testRE.print();
//	testRE.genImage();
//	ENFA testENFA = testRE.toENFA();
//
//	std::cout << dfa7.accepts("dff") << std::endl;
////	testENFA.genImage();
//	std::cout << testENFA.accepts("dff") << std::endl;
//	DFA testDFA = testENFA.toDFA();
////	testDFA.genImage();
//	std::cout << testDFA.accepts("dff") << std::endl;


	return 0;
}