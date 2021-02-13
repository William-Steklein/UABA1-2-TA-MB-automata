#include <iostream>
#include <unordered_set>
#include <fstream>
#include <iomanip>
#include "DFA.h"
#include "json.hpp"

using json = nlohmann::json;

int main(){
//	DFA dfa;
//	std::cout << std::boolalpha << dfa.accepts("0010110100")
//	<< std::endl << dfa.accepts("0001") << std::endl;


//	// inlezen uit file
//	std::ifstream input("DFA.json");
//
//	json j;
//	input >> j;
//
//	// tonen op het scherm
//	std::cout << std::setw(4) << j << std::endl;
//
//	// manueel aanmaken
//	json j2;
//	j2["type"] = "DFA";
//	j2["alphabet"] = {"0", "1"};
//	std::cout << std::setw(4) << j2 << std::endl;


	DFA dfa;
//	dfa.load("DFA2.json");

	std::cout << std::boolalpha << dfa.accepts("0010110100")
	<< std::endl << dfa.accepts("0001") << std::endl;

	return 0;
}
