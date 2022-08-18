#include <iostream>
#include "STest/Rule/Rule.hpp"
#include "STest/Pick/Pick.hpp"
#include "STest/Scenario/Scenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"
#include "STest/Scenario/BoundedScenario.hpp"

typedef double D;
typedef uint32_t U32;

class ExampleClass { public: D ExampleFunction(D a, D b);  };
D ExampleClass::ExampleFunction(D a, D b) { return (a / b); }

struct ExampleRule: public STest::Rule<ExampleClass> 
{
    ExampleRule(const char* ruleName);
    bool precondition(const ExampleClass& state);
    void action(ExampleClass& truth);
};

ExampleRule::ExampleRule(const char* ruleName): STest::Rule<ExampleClass>(ruleName) {}
bool ExampleRule::precondition(const ExampleClass &state) { return true; }
void ExampleRule::action(ExampleClass &state) 
{
    D dividend = STest::Pick::lowerUpper(0, 100), divisor = STest::Pick::lowerUpper(0, 100);
    D result = state.ExampleFunction(dividend, divisor);
    std::cout << "Result of division: " << dividend << "/" << divisor << " = " << result << "\n";
}

int main()
{
    ExampleClass exampleClassObject;
    ExampleRule exampleRuleObject("ExampleRule");
    exampleRuleObject.apply(exampleClassObject); 
}    