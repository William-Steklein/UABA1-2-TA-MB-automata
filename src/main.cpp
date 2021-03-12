#include <iostream>
#include <unordered_set>
#include <fstream>
#include <iomanip>
#include <graphviz/gvc.h>
#include <stdio.h>
#include <dirent.h>
#include "DFA.h"
#include "NFA.h"
#include "json.hpp"

//using json = nlohmann::ordered_json;
using json = nlohmann::json;

// todo: put library source code in project
// todo: check algorithms with testinput from tuyaux
/*
 * todo
 * - implement NFA
 * - implement ENFA
 * - implement RE
 * - implement subset construction
 * - implement modified subset construction
 * - implement regex -> ENFA
 * - implement state elimination DFA -> regex
 * - implement table filling algorithm
 *
 * - put library source code in project
 * - check algorithms with testinput from tuyaux
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

	DFA dfa("../automata_json/DFA.json");
	DFA dfa2("../automata_json/DFA2.json");
	DFA dfa3("../automata_json/DFA3.json");

	std::cout << dfa.checkLegality() << std::endl;
	std::cout << dfa2.checkLegality() << std::endl;
	std::cout << dfa3.checkLegality() << std::endl;

	dfa.genImage();
	dfa2.genImage();
	dfa3.genImage();

	return 0;
}