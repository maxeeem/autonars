#include "Memory.h"

Memory::Memory() {
    concepts.reserve(CONCEPT_MAX);
}

Memory::~Memory() {
}

uint32_t Memory::get_or_create_concept(const Term& term) {
    auto it = term_to_concept.find(term);
    if (it != term_to_concept.end()) {
        return it->second;
    }

    uint32_t new_id = concepts.size();
    if (new_id >= CONCEPT_MAX) {
        // Concept memory is full. Needs forgetting mechanism.
        // For now, just overwrite the last one or do nothing
        return 0; 
    }

    Concept c;
    c.id = new_id;
    c.term = term;
    c.priority = 0.9f;
    c.durability = 0.9f;
    c.quality = 0.9f;

    concepts.push_back(c);
    term_to_concept[term] = new_id;

    return new_id;
}

Concept& Memory::get_concept(uint32_t id) {
    return concepts[id];
}

void Memory::cycle() {
    // Basic NARS cycle: select concept, select task, perform inference
}
