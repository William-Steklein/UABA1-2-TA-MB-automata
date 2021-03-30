#ifndef UABA1_TA_AUTOMATA2_SRC_ALPHABET_H
#define UABA1_TA_AUTOMATA2_SRC_ALPHABET_H

#include <iostream>
#include <string>
#include <set>

class Alphabet
{
	std::set<char> alphabet;
	char epsilon;

	std::ostream* output_stream = &std::cerr;
	int ID;
	static int nextID;

protected:
	Alphabet();
	Alphabet& operator=(const Alphabet& _alphabet);

	void incrementID();

public:
	bool addSymbol(char symbol);
	bool removeSymbol(char symbol);
	bool isSymbolInAlphabet(char a) const;
	std::set<char> getAlphabet() const;
	/* Sets the alphabet to the characters in the string */
	void setAlphabet(const std::string& _alphabet);
	void setAlphabet(const std::set<char>& _alphabet);
	void clearAlphabet();
	char getEpsilon() const;
	void setEpsilon(char _epsilon);

	std::ostream* getOutputStream() const;
	void setOutputStream(std::ostream& _output_stream);
	int getID() const;
};

#endif
