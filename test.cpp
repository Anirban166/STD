#include <iostream>
#include "STest/Rule/Rule.hpp"
#include "STest/Pick/Pick.hpp"
#include "STest/Scenario/Scenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"
#include "STest/Scenario/BoundedScenario.hpp"

typedef int32_t  I32;
typedef uint32_t U32;

class ExampleClass  { public: U32 ExampleFunction(U32 a, U32 b); };
U32 ExampleClass::ExampleFunction(U32 a, U32 b) { return (a / b); }

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
    U32 dividend = STest::Pick::lowerUpper(0, 100), divisor = STest::Pick::lowerUpper(0, 100);
    assert(divisor != 0);
    U32 result = state.ExampleFunction(dividend, divisor);
    std::cout << "Result of division: " << dividend << "/" << divisor << " = " << result << "\n";
}

int main()
{
    ExampleClass exampleClassObject;
    ExampleRule exampleRuleObject("Hello");
    exampleRuleObject.apply(exampleClassObject);
}
