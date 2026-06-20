#include <iostream>
#include "Memory.h"
#include "Term.h"
#include "Truth.h"
#include "Introspection.h"

int main() {
    std::cout << "MacNARS: Apple Silicon Optimized NARS" << std::endl;
    std::cout << "-------------------------------------" << std::endl;

    // Run NAL-9 Introspection Proof of Concept
    Introspection::run_introspection("/Users/maxeeem/autonars/MacNARS/src");

    Memory memory;

    Term t;
    t.atoms[0] = 1; // dummy atom 'cat'
    
    uint32_t cid = memory.get_or_create_concept(t);
    std::cout << "Created concept with ID: " << cid << std::endl;

    return 0;
}
