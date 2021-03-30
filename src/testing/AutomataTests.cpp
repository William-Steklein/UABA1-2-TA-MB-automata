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

	dfa0.setEpsilon('*');
	EXPECT_FALSE(dfa0.isLegal());

	dfa0.setEpsilon(' ');
	EXPECT_TRUE(dfa0.isLegal());



	// DFA
	DFA dfa1("../src/testing/testInput/basic_functionality/DFA1.json");
	EXPECT_TRUE(dfa1.isLegal());

	EXPECT_TRUE(dfa1.accepts("1"));
	EXPECT_TRUE(dfa1.accepts("111"));
	EXPECT_TRUE(dfa1.accepts("00000100000"));
	EXPECT_FALSE(dfa1.accepts(""));
	EXPECT_FALSE(dfa1.accepts("11"));
	EXPECT_FALSE(dfa1.accepts("00000000000"));
	EXPECT_FALSE(dfa1.accepts("00100100110"));

	EXPECT_EQ(' ', dfa1.getEpsilon());


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