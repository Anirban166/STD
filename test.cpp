#include <iostream>
#include "STest/Rule/Rule.hpp"
#include "STest/Pick/Pick.hpp"
#include "STest/Scenario/Scenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"
#include "STest/Scenario/BoundedScenario.hpp"

typedef double D;
typedef uint32_t U32;
#define DRANGE 100000

class ArithmeticOperations          
{ 
  public: 
    D DivisionFunction(D a, D b);          // Division by zero check
    U32 AdditionFunction(U32 a, U32 b);    // U32 overflow check
};
D ArithmeticOperations::DivisionFunction(D a, D b)       { return (a / b); }
U32 ArithmeticOperations::AdditionFunction(U32 a, U32 b) { return (a + b); }

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
    D dividend = STest::Pick::lowerUpper(0, DRANGE), divisor = STest::Pick::lowerUpper(0, DRANGE);
    D result = state.DivisionFunction(dividend, divisor);
    std::cout << "Result of division: " << dividend << "/" << divisor << " = " << result << "\n";
}

struct Addition: public STest::Rule<ArithmeticOperations> 
{
    Addition(const char* ruleName);
    bool precondition(const ArithmeticOperations& state);
    void action(ArithmeticOperations& truth);
};

Addition::Addition(const char* ruleName): STest::Rule<ArithmeticOperations>(ruleName) {}
bool Addition::precondition(const ArithmeticOperations &state) { return true; }
void Addition::action(ArithmeticOperations &state) 
{
    U32 firstU32 = STest::Pick::lowerUpper(0, UINT32_MAX), secondU32 = STest::Pick::lowerUpper(0, UINT32_MAX);
    U32 result = state.AdditionFunction(firstU32, secondU32);
    std::cout << "Result of addition: " << firstU32 << "+" << secondU32 << " = " << result << "\n";
}

int main()
{
    ArithmeticOperations exampleObject;

    Division exampleDivisionRuleObject("Division");
    Addition exampleAdditionRuleObject("Addition");
    exampleDivisionRuleObject.apply(exampleObject);
    exampleAdditionRuleObject.apply(exampleObject);

    STest::Rule<ArithmeticOperations>* rules[] = { &exampleDivisionRuleObject, &exampleAdditionRuleObject };
    STest::RandomScenario<ArithmeticOperations> randomRules("Peanuts", rules, 2);
    STest::BoundedScenario<ArithmeticOperations> bounded("NASA Jesus", randomRules, 42069);
}