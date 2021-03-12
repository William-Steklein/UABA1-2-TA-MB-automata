#include "Automaton.h"

const std::string DOT_IMAGES_PATH = "../DOT_images/";
int Automaton::nextID = 0;

Automaton::Automaton()
{
	ID = nextID++;
}

bool Automaton::isSymbolInAlphabet(char a) const
{
	if (alphabet.find(a) != alphabet.end())
		return true;
	std::cerr << "Error: symbol " << a << " is not in alphabet" << std::endl;
	return false;
}

void Automaton::addSymbol(char new_symbol)
{
	alphabet.insert(new_symbol);
}

void Automaton::removeSymbol(char symbol)
{
	alphabet.erase(symbol);
}

void Automaton::setAlphabet(const std::string& new_alphabet)
{
	clearAlphabet();
	for (char symbol : new_alphabet)
	{
		addSymbol(symbol);
	}
}

void Automaton::clearAlphabet()
{
	alphabet.clear();
}

int Automaton::getID() const
{
	return ID;
}

bool Automaton::genImage() const
{
	std::string path = DOT_IMAGES_PATH + std::to_string(getID()) + ".dot";

	std::ofstream file(path);
	std::string my_string = genDOT();
	file << my_string;
	file.close();

	GVC_t* gvc;
	Agraph_t* g;
	FILE* fp;
	gvc = gvContext();
	fp = fopen((path).c_str(), "r");
	g = agread(fp, nullptr);
	gvLayout(gvc, g, "dot");
	gvRender(gvc, g, "png", fopen((DOT_IMAGES_PATH + std::to_string(getID()) + "_image.png").c_str(),
		"w"));
	gvFreeLayout(gvc, g);
	agclose(g);

	return (gvFreeContext(gvc));
}
