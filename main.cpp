#include <iostream>
#include <unordered_set>
#include <fstream>
#include <iomanip>
#include <graphviz/gvc.h>
#include "DFA.h"
#include "json.hpp"

using json = nlohmann::ordered_json;
//using json = nlohmann::json;

bool saveImageGV(const std::string& dot_file_path, const std::string& image_file_path)
{
	GVC_t* gvc;
	Agraph_t* g;
	FILE* fp;
	gvc = gvContext();
	fp = fopen((dot_file_path).c_str(), "r");
	g = agread(fp, nullptr);
	gvLayout(gvc, g, "dot");
	gvRender(gvc, g, "png", fopen((image_file_path + "image.png").c_str(), "w"));
	gvFreeLayout(gvc, g);
	agclose(g);
	return (gvFreeContext(gvc));
}

int main()
{
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


//	DFA dfa{"DFA2.json"};
//
//	std::cout << std::boolalpha << dfa.accepts("0010110100")
//	<< std::endl << dfa.accepts("0001") << std::endl;
//
//	dfa.print();

	DFA dfa("../DFA_json/DFA3.json");
	std::cout << dfa.genImage();

	return 0;
}