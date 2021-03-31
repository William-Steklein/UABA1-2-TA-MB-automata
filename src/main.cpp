#include <iostream>
#include "DFA.h"
#include "NFA.h"
#include "ENFA.h"
#include "RE.h"

/*
 * todo implementation
 * - state elimination DFA -> regex
 * - table filling algorithm
 *
 *
 * todo cleaning
 * - rewrite the whole project with better polymorphism
 * 		- finish all conversions
 * 		- relook all algoritms and make tests
 *
 * - RE fix concatenation child problem => user error handling
 *
 * - change or remove epsilon in (DFA) transitionstoRE
 * - RE parser remove eps parameter and make a get epsilon function
 * - clean or rewrite RE parser
 * - make everything that can be made const const
 * - MEMORY LEAKS AAAAAAAAAAAAAAAAAAAAAAAAAAAAA!!!!!!!!!!!!!!
 *
 *
 * todo changes/other
 * - make directory if it doesn't exists
 * - fix deletefiles function so that it doesn't crash when the directory doesn't exists
 *
 */

void deleteFiles(const std::string& folderpath);

int main()
{
	deleteFiles("../DOT_images");
	deleteFiles("../DOT_files");

	std::cout << "Hello World" << std::endl;

//	DFA dfa("../automata_json/DFA.json");
	DFA dfa("../src/testing/testInput/basic_functionality/DFA1.json");
	dfa.genImage();

	std::cout << dfa.accepts("") << std::endl;
	std::cout << dfa.accepts("1") << std::endl;
	std::cout << dfa.accepts("111") << std::endl;
	std::cout << dfa.accepts("00000000000") << std::endl;
	std::cout << dfa.accepts("00000100000") << std::endl;
	std::cout << dfa.accepts("00100100000") << std::endl;

	dfa.print();

//	DFA dfa2("../automata_json/DFA2.json");
//	dfa2.genImage();
//
//	DFA dfa3("../automata_json/DFA3.json");
//	dfa3.genImage();
//
//	DFA dfa4("../automata_json/DFA4.json");
//	dfa4.genImage();
//
//	DFA dfa5("../automata_json/DFA5.json");
//	dfa5.genImage();
//
//	DFA dfa6("../automata_json/DFA6.json");
//	dfa6.genImage();
//
//	DFA dfa7("../automata_json/DFA7.json");
//	dfa7.genImage();


	return 0;
}
