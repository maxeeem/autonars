#ifndef MACNARS_MEMORY_V3_H
#define MACNARS_MEMORY_V3_H

#include "Config.h"
#include "ConceptV3.h"
#include "EmbeddingService.h"
#include <unordered_map>
#include <vector>
#include <iostream>

class MemoryV3 {
public:
    Bag<ConceptV3, std::string> concept_bag;

    MemoryV3() : concept_bag(100, 1000, 10) {}

    void add_concept(const std::string& name) {
        Term dummy_term; 
        auto concept = std::make_shared<ConceptV3>(dummy_term, name, concept_bag.size());
        concept_bag.put_in(concept, name, 0.9f);
        std::cout << "[Memory] Ingested Concept: '" << name << "' (Embedding generated)\n";
    }

    // Semantic Gravity Retrieval
    // Instead of looking up an exact string, we find the concept with the highest cosine similarity
    std::shared_ptr<ConceptV3> retrieve_by_semantic_gravity(const std::string& query) {
        std::cout << "[Attention] Searching semantic manifold for: '" << query << "'\n";
        
        // 1. Generate query embedding
        std::vector<double> query_emb = EmbeddingService::get_embedding(query);
        if (query_emb.empty()) {
            std::cout << "[Attention] Failed to generate query embedding.\n";
            return nullptr;
        }

        std::shared_ptr<ConceptV3> best_concept = nullptr;
        double max_gravity = -1.0;

        // 2. Iterate through Bag (In a full implementation, this would be a specialized vector index like Faiss or a Metal compute shader)
        for (const auto& pair : concept_bag.get_items()) {
            std::shared_ptr<ConceptV3> concept = pair.second;
            double gravity = concept->get_semantic_gravity(query_emb);
            
            if (gravity > max_gravity) {
                max_gravity = gravity;
                best_concept = concept;
            }
        }

        if (best_concept) {
            std::cout << "[Attention] -> Semantic Gravity pulled Concept: '" << best_concept->term_name 
                      << "' (Similarity: " << max_gravity << ")\n";
        }
        return best_concept;
    }
};

#endif
