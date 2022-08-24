#include <iostream>
#include <deepstate/DeepState.hpp>
#include "../../STest/Rule/Rule.hpp"
#include "../../STest/Pick/Pick.hpp"
#include "../../STest/Scenario/Scenario.hpp"
#include "../../STest/Scenario/RandomScenario.hpp"
#include "../../STest/Scenario/BoundedScenario.hpp"

typedef uint32_t U32;
using namespace deepstate;

class ArithmeticOperations  
{ 
    public:     
        DEEPSTATE_NOINLINE U32 DivisionFunction(U32 a, U32 b);
        DEEPSTATE_NOINLINE U32 MultiplicationFunction(U32 a, U32 b);
};
DEEPSTATE_NOINLINE U32 ArithmeticOperations::MultiplicationFunction(U32 a, U32 b) { return (a * a); }
DEEPSTATE_NOINLINE U32 ArithmeticOperations::DivisionFunction(U32 a, U32 b)       { return (a / b); }

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
    U32 dividend = DeepState_UIntInRange(0, 100), divisor = DeepState_UIntInRange(0, 100);   
    ASSERT_NE(divisor, 0) << "Divisor cannot be 0! (dividend and divisor values for this run: " << dividend << ", " << divisor << ")";
    // Not really concerned about the result if it's a successful case (divisor!=0), but gotta keep the compiler happy with its' unusued variable warning:        
    int result = state.DivisionFunction(dividend, divisor);
    (void)result;
}

struct Multiplication: public STest::Rule<ArithmeticOperations> 
{
    Multiplication(const char* ruleName);
    bool precondition(const ArithmeticOperations& state);
    void action(ArithmeticOperations& truth);
};

Multiplication::Multiplication(const char* ruleName): STest::Rule<ArithmeticOperations>(ruleName) {}
bool Multiplication::precondition(const ArithmeticOperations &state) { return true; }
void Multiplication::action(ArithmeticOperations &state) 
{
    U32 multiplicant = DeepState_UIntInRange(0, UINT32_MAX), multiplier = DeepState_UIntInRange(0, UINT32_MAX);   
    ASSERT_EQ((multiplicant * multiplier) / multiplier, multiplicant) << "The multiplication returned a wrong result - check for overflows! (multiplicant and multiplier values for this run: " << multiplicant << ", " << multiplier << ")";    
    int result = state.MultiplicationFunction(multiplicant, multiplier);
    (void)result;
}

TEST(BasicArithmeticOps, RandomizedOpsCheck) 
{
    ArithmeticOperations arithmeticOpsObject;

    Division divisionRuleObject("Division");
    Multiplication multiplicationRuleObject("Multiplication");
    divisionRuleObject.apply(arithmeticOpsObject);
    multiplicationRuleObject.apply(arithmeticOpsObject);

    STest::Rule<ArithmeticOperations>* rules[] = { &divisionRuleObject, &multiplicationRuleObject };
    STest::RandomScenario<ArithmeticOperations> randomRules("Peanuts", rules, 2);
    STest::BoundedScenario<ArithmeticOperations> bounded("NASA Jesus", randomRules, 4269);  
}