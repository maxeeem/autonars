#ifndef MACNARS_CONCEPT_H
#define MACNARS_CONCEPT_H

#include "Config.h"
#include "Term.h"
#include "Truth.h"

struct Concept {
    uint32_t id;
    Term term;
    
    // Budgets
    float priority;
    float durability;
    float quality;

    // Beliefs could also be represented by an array of indices or in-line
    // To keep it simple and flat, we can store the top belief directly 
    // or keep a small array of beliefs
    Truth belief;
    float belief_creation_time;

    Concept() : id(0), priority(0), durability(0), quality(0), belief_creation_time(0) {}
};

#endif
