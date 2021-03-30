#include <iostream>
#include <gtest/gtest.h>
#include "../DFA.h"
#include "../NFA.h"
#include "../ENFA.h"
#include "../RE.h"

class AutomataTests: public ::testing::Test {
 protected:
	// You should make the members protected s.t. they can be
	// accessed from sub-classes.

	// virtual void SetUp() will be called before each test is run.  You
	// should define it if you need to initialize the variables.
	// Otherwise, this can be skipped.
	virtual void SetUp() {
	}

	// virtual void TearDown() will be called after each test is run.
	// You should define it if there is cleanup work to do.  Otherwise,
	// you don't have to provide it.
	virtual void TearDown() {
	}

	// Declares the variables your tests want to use.

};

TEST_F(AutomataTests, basic_functionality)
{
	std::ostream bitBucket(0);

	EXPECT_TRUE(true);

	DFA dfa0("../src/testing/testInput/basic_functionality/DFA1.json");
	dfa0.setOutputStream(bitBucket);

	EXPECT_TRUE(dfa0.isLegal());
	EXPECT_EQ(' ', dfa0.getEpsilon());

	// Alphabet
	std::set<char> _alphabet = { '0', '1' };
	EXPECT_EQ(_alphabet, dfa0.getAlphabet());

	dfa0.addSymbol('2');
	EXPECT_FALSE(dfa0.isLegal());
	_alphabet = { '0', '1', '2' };
	EXPECT_EQ(_alphabet, dfa0.getAlphabet());

	dfa0.removeSymbol('2');
	EXPECT_TRUE(dfa0.isLegal());
	_alphabet = { '0', '1' };
	EXPECT_EQ(_alphabet, dfa0.getAlphabet());

	dfa0.setEpsilon('*');
	EXPECT_FALSE(dfa0.isLegal());

	dfa0.setEpsilon(' ');
	EXPECT_TRUE(dfa0.isLegal());

	EXPECT_EQ(0, dfa0.getID());

	// Automaton
	dfa0.addState("Q2", true);
	EXPECT_FALSE(dfa0.isLegal());

	dfa0.addTransition("Q0", "Q2", '0');

	EXPECT_FALSE(dfa0.isLegal());

	dfa0.addTransition("Q2", "Q2", '0');
	dfa0.addTransition("Q2", "Q2", '1');

	EXPECT_TRUE(dfa0.isLegal());
	EXPECT_TRUE(dfa0.accepts("1"));
	EXPECT_FALSE(dfa0.accepts("11"));

	dfa0.setStartState("Q1");

	EXPECT_TRUE(dfa0.isLegal());
	EXPECT_FALSE(dfa0.accepts("1"));
	EXPECT_TRUE(dfa0.accepts("11"));

	EXPECT_TRUE(dfa0.isStateAccepting(dfa0.getStartState()));
	EXPECT_FALSE(dfa0.isStateAccepting("Q0"));
	EXPECT_TRUE(dfa0.isSetOfStatesAccepting(dfa0.getAllStates()));

	EXPECT_EQ("{Q0,Q1,Q2}", DFA::getSetOfStatesString(dfa0.getAllStates()));

	EXPECT_EQ("{Q1,Q2}", DFA::getSetOfStatesString(dfa0.transitionSetOfStates({ "Q1", "Q0" }, '0')));

	EXPECT_TRUE(dfa0.removeTransition("Q1", "Q0", '1'));
	EXPECT_FALSE(dfa0.isLegal());

	EXPECT_TRUE(dfa0.removeState("Q0"));
	EXPECT_FALSE(dfa0.removeState("Q0"));
	EXPECT_FALSE(dfa0.isLegal());

	EXPECT_TRUE(dfa0.addTransition("Q1", "Q2", '1'));
	EXPECT_TRUE(dfa0.isLegal());

	EXPECT_FALSE(dfa0.addTransition("Q1", "Q0", '1'));
	EXPECT_TRUE(dfa0.isLegal());

	EXPECT_EQ("{Q2}", DFA::getSetOfStatesString(dfa0.transition(dfa0.getStartState(), '1')));

	// DFA
	DFA dfa1("../src/testing/testInput/basic_functionality/DFA1.json");
	dfa1.setOutputStream(bitBucket);
	EXPECT_TRUE(dfa1.isLegal());
	EXPECT_EQ(1, dfa1.getID());

	EXPECT_TRUE(dfa1.accepts("1"));
	EXPECT_TRUE(dfa1.accepts("111"));
	EXPECT_TRUE(dfa1.accepts("00000100000"));
	EXPECT_FALSE(dfa1.accepts(""));
	EXPECT_FALSE(dfa1.accepts("11"));
	EXPECT_FALSE(dfa1.accepts("00000000000"));
	EXPECT_FALSE(dfa1.accepts("00100100110"));

	EXPECT_EQ(' ', dfa1.getEpsilon());

	EXPECT_EQ("{Q0,Q1}", DFA::getSetOfStatesString(dfa1.getAllStates()));
	dfa1.renameStates(true);
	EXPECT_EQ("{A,B}", DFA::getSetOfStatesString(dfa1.getAllStates()));
	EXPECT_TRUE(dfa1.isLegal());
	dfa1.renameStates(false);
	EXPECT_EQ("{0,1}", DFA::getSetOfStatesString(dfa1.getAllStates()));
	EXPECT_TRUE(dfa1.isLegal());

	dfa1.genImage();

	dfa1.clear();

	EXPECT_FALSE(dfa1.isLegal());
	EXPECT_EQ("", dfa1.getStartState());

	// NFA

	// ENFA

	// RE
}

TEST_F(AutomataTests, subset_construction)
{

}

TEST_F(AutomataTests, modified_subset_construction)
{

}

TEST_F(AutomataTests, product_automaat)
{

}

TEST_F(AutomataTests, regex_to_ENFA)
{

}

TEST_F(AutomataTests, state_elimination)
{

}

TEST_F(AutomataTests, table_filling_algorithm)
{

}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}