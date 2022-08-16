#include <iostream>
#include "STest/Rule/Rule.hpp"
#include "STest/Pick/Pick.hpp"
#include "STest/Scenario/Scenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"
#include "STest/Scenario/BoundedScenario.hpp"

typedef int32_t  I32;
typedef uint32_t U32;

class Example { U32 func(U32 a, U32 b); };
U32 Example::func(U32 a, U32 b) { return (a / b); }

int main()
{

}