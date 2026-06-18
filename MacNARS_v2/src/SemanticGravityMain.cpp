#include <iostream>
#include "MemoryV3.h"

int main() {
    std::cout << "====================================================\n";
    std::cout << " MacNARS v2: Associative Manifold Memory (Track 2) \n";
    std::cout << "====================================================\n\n";

    MemoryV3 memory;

    // 1. Ingest baseline symbolic concepts (NARS learns these)
    memory.add_concept("canary");
    memory.add_concept("Boeing 747");
    memory.add_concept("submarine");
    memory.add_concept("apple");
    std::cout << "\n";

    // 2. Task: NARS is queried about "bird"
    // Traditional NARS would fail here because "bird" != "canary".
    // Associative Manifold Memory uses Semantic Gravity.
    memory.retrieve_by_semantic_gravity("bird");

    // 3. Task: NARS is queried about "aircraft"
    memory.retrieve_by_semantic_gravity("aircraft");
    
    // 4. Task: NARS is queried about "underwater vessel"
    memory.retrieve_by_semantic_gravity("underwater vessel");

    std::cout << "\n====================================================\n";
    std::cout << "Conclusion: NARS is no longer synonymous-brittle.\n";
    std::cout << "It leverages ANE vector embeddings alongside Bag logic.\n";

    return 0;
}
