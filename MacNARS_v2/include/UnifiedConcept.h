#ifndef MACNARS_UNIFIED_CONCEPT_H
#define MACNARS_UNIFIED_CONCEPT_H

#include "Config.h"
#include "Term.h"
#include "Bag.h"
#include "Sentence.h"
#include <vector>
#include <cmath>

// MacNARS v4: Unified Concept
// Represents a discrete symbol natively anchored to a continuous high-dimensional vector space.
struct UnifiedConcept {
    Term term;
    uint32_t id;
    
    // The Semantic Manifold Anchor (can come from Apple Neural Engine OR from RL State Vectors)
    std::vector<double> latent_embedding;
    
    // Attentional Bags
    Bag<Task, std::string> task_bag;
    Bag<Sentence, std::string> belief_bag;

    UnifiedConcept(Term t, uint32_t cid, const std::vector<double>& emb) 
        : term(t), id(cid), latent_embedding(emb),
          task_bag(100, 100, 10), 
          belief_bag(100, 100, 10) {}
          
    double get_semantic_gravity(const std::vector<double>& query_embedding) const {
        if (latent_embedding.empty() || query_embedding.empty() || latent_embedding.size() != query_embedding.size()) {
            return 0.0;
        }
        
        double dot = 0.0, norm_a = 0.0, norm_b = 0.0;
        for (size_t i = 0; i < latent_embedding.size(); ++i) {
            dot += latent_embedding[i] * query_embedding[i];
            norm_a += latent_embedding[i] * latent_embedding[i];
            norm_b += query_embedding[i] * query_embedding[i];
        }
        return dot / (std::sqrt(norm_a) * std::sqrt(norm_b));
    }
};

#endif
