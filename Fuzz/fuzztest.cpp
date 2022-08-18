#include <iostream>
#include "../STest/Rule/Rule.hpp"
#include "../STest/Pick/Pick.hpp"
//#include "../STest/Scenario/Scenario.hpp"
//#include "../STest/Scenario/RandomScenario.hpp"
//#include "../STest/Scenario/BoundedScenario.hpp"
#define N 1000000000
typedef uint32_t U32;

class ArithmeticOperations             { public: U32 DivisionFunction(U32 a, U32 b); };
U32 ArithmeticOperations::DivisionFunction(U32 a, U32 b) { return (a / (a - b + 42)); }

// Not checking for overflows (U32 for instance, like in ../test.cpp) since that does not raise an exception or crash. 
// If you still want to just fuzz for that though, the addition function returning something like a + b + (UINT32_MAX - some not so big number) will make it a more appropriate candidate.
// A better example than the division by zero case could possibly be a sequence of similar operations.
// These 'operations' could be flight software related commands (mnemonic + opcodes) if you're coming from fprime and/or want to fuzz FSW.
// Or they could be stuff like pushing and popping from a stack, enqueues and dequeues in a circular buffer, etc. 

struct Division: public STest::Rule<ArithmeticOperations> 
{
    Division(const char* ruleName);
    bool precondition(const ArithmeticOperations& state);
    void action(ArithmeticOperations& truth);
};

Division::Division(const char* ruleName): STest::Rule<ArithmeticOperations>(ruleName) {}
bool Division::precondition(const ArithmeticOperations &state) { return true; }
void Division::action(ArithmeticOperations &state) 
{
    U32 dividend = STest::Pick::lowerUpper(0, 100), divisor = STest::Pick::lowerUpper(0, 100);
    U32 result = state.DivisionFunction(dividend, divisor);
    std::cout << "Result of division: " << dividend << "/" << divisor << " = " << result << "\n";
}

int main()
{
    ArithmeticOperations arithmeticOpsObject;
    Division divisionRuleObject("Division");
    // Even after 2N randomly generated numbers, it doesn't find the denominator (modified divisor) to be equal to 0, thus missing out on an exception/potential-crash/UB (compiler-specific) case - a candidate for fuzz testing.
    for(int i = 0; i < N; ++i) { divisionRuleObject.apply(arithmeticOpsObject); }
}    