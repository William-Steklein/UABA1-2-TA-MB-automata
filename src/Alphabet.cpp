#include "Alphabet.h"

int Alphabet::nextID = 0;

Alphabet::Alphabet()
{
	ID = nextID++;
	epsilon = ' ';
}

Alphabet & Alphabet::operator=(const Alphabet& _alphabet)
{
	if (&_alphabet != this)
	{
		alphabet = _alphabet.alphabet;
		epsilon = _alphabet.epsilon;
	}

	return *this;
}

void Alphabet::incrementID()
{
	ID = nextID++;
}

bool Alphabet::addSymbol(char symbol)
{
	alphabet.insert(symbol);
	return true;
}

bool Alphabet::removeSymbol(char symbol)
{
	if (alphabet.find(symbol) == alphabet.end())
	{
		*getOutputStream() << "Error: alphabet " << getID() << ", couldn't remove symbol '" << symbol << "' from the alphabet"
				  << std::endl;
		return false;
	}
	alphabet.erase(symbol);
	return true;
}

bool Alphabet::isSymbolInAlphabet(char a) const
{
	if (alphabet.find(a) != alphabet.end())
		return true;
	*getOutputStream() << "Error: alphabet " << getID() << ", symbol " << a << " is not in alphabet" << std::endl;
	return false;
}

std::set<char> Alphabet::getAlphabet() const
{
	return alphabet;
}

void Alphabet::setAlphabet(const std::string& _alphabet)
{
	clearAlphabet();
	for (char symbol : _alphabet)
	{
		addSymbol(symbol);
	}
}

void Alphabet::setAlphabet(const std::set<char>& _alphabet)
{
	alphabet = _alphabet;
}

void Alphabet::clearAlphabet()
{
	alphabet.clear();
	epsilon = ' ';
}

char Alphabet::getEpsilon() const
{
	return epsilon;
}

void Alphabet::setEpsilon(char _epsilon)
{
	epsilon = _epsilon;
}

char Alphabet::getStandardEpsilon()
{
	return standard_epsilon;
}

std::ostream* Alphabet::getOutputStream() const
{
	return output_stream;
}

void Alphabet::setOutputStream(std::ostream& _output_stream)
{
	output_stream = &_output_stream;
}

int Alphabet::getID() const
{
	return ID;
}