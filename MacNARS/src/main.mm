#include <iostream>
#include "Memory.h"
#include "Term.h"
#include "Truth.h"

int main() {
    std::cout << "MacNARS: Apple Silicon Optimized NARS" << std::endl;
    std::cout << "-------------------------------------" << std::endl;

    Memory memory;

    Term t;
    t.atoms[0] = 1; // dummy atom 'cat'
    
    uint32_t cid = memory.get_or_create_concept(t);
    std::cout << "Created concept with ID: " << cid << std::endl;

    return 0;
}
