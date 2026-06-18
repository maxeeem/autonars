#ifndef MACNARS_CONCEPT_V3_H
#define MACNARS_CONCEPT_V3_H

#include "Config.h"
#include "Term.h"
#include "Bag.h"
#include "Sentence.h"
#include "EmbeddingService.h"
#include <string>
#include <vector>

struct ConceptV3 {
    Term term;
    std::string term_name; // String representation for the embedding model
    uint32_t id;
    
    // The Semantic Manifold Anchor
    std::vector<double> embedding;
    
    // Attentional Bags
    Bag<Task, std::string> task_bag;
    Bag<Sentence, std::string> belief_bag;

    ConceptV3(Term t, std::string name, uint32_t cid) 
        : term(t), term_name(name), id(cid), 
          task_bag(100, 100, 10), 
          belief_bag(100, 100, 10) {
        
        // Generate embedding upon concept creation
        embedding = EmbeddingService::get_embedding(term_name);
    }
    
    // Calculates the "Semantic Gravity" (attraction) between this concept and a query vector
    double get_semantic_gravity(const std::vector<double>& query_embedding) const {
        return EmbeddingService::cosine_similarity(this->embedding, query_embedding);
    }
};

#endif
