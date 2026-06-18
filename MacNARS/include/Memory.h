#ifndef MACNARS_MEMORY_H
#define MACNARS_MEMORY_H

#include "Config.h"
#include "Concept.h"
#include <unordered_map>
#include <vector>

class Memory {
public:
    std::vector<Concept> concepts;
    std::unordered_map<Term, uint32_t> term_to_concept;

    Memory();
    ~Memory();

    uint32_t get_or_create_concept(const Term& term);
    Concept& get_concept(uint32_t id);
    
    // Process step
    void cycle();
};

#endif
