#ifndef MACNARS_CONCEPT_V2_H
#define MACNARS_CONCEPT_V2_H

#include "Config.h"
#include "Term.h"
#include "Bag.h"
#include "Sentence.h"
#include <string>

struct ConceptV2 {
    Term term;
    uint32_t id;
    
    // Attentional Bags
    Bag<Task, std::string> task_bag;
    Bag<Sentence, std::string> belief_bag;

    ConceptV2(Term t, uint32_t cid) 
        : term(t), id(cid), 
          task_bag(100, 100, 10), 
          belief_bag(100, 100, 10) {}
};

#endif
