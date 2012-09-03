#include <iostream>
#include <string>

#include "TestAccuracy.hpp"

// Actions
#include "ActionGESV.hpp"

using namespace std;

int main(int argc, char **argv)
{
    bool do_gesv = false;

    // Parse input
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "general_solve") do_gesv = true;
    }

    if (do_gesv)
        testAccuracy<ActionGESV, double>(4, 2000, 20);
}
