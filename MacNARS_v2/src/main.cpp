#include <iostream>
#include "MemoryV2.h"
#include "Sentence.h"
#include "Config.h"

int main() {
    std::cout << "MacNARS v2: Faithful NARS Implementation" << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    MemoryV2 memory;

    // Simulate input: {M --> P}.
    Term t1;
    t1.atoms[0] = COPULA_INHERITANCE;
    t1.atoms[1] = 100; // M
    t1.atoms[2] = 200; // P
    Task task1 = {{t1, {1.0f, 0.9f}, {}, '.'}, {0.9f, 0.9f, 0.9f}};
    memory.add_task(task1);

    // Simulate input: {S --> M}.
    Term t2;
    t2.atoms[0] = COPULA_INHERITANCE;
    t2.atoms[1] = 300; // S
    t2.atoms[2] = 100; // M
    Task task2 = {{t2, {1.0f, 0.9f}, {}, '.'}, {0.9f, 0.9f, 0.9f}};
    memory.add_task(task2);

    // Run cycles
    for (int i = 0; i < 100; ++i) {
        memory.cycle();
    }

    return 0;
}
