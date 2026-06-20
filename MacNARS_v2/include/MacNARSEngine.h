#ifndef MACNARS_ENGINE_H
#define MACNARS_ENGINE_H

#include "MetalBagEngine.h"
#include "MetalGraphEngine.h"
#include <vector>
#include <iostream>

// MacNARS v8: The Ultimate Neuromorphic Logic Engine
// Unifies MetalBagEngine (Perception/Attention) and MetalGraphEngine (Planning/Reasoning)
class MacNARSEngine {
public:
    MetalBagEngine bag_engine;
    MetalGraphEngine graph_engine;
    
    std::vector<MetalConceptEmbedding> semantic_manifold;
    std::vector<MetalCausalRule> causal_graph;
    std::vector<uint32_t> desire_vector;
    
    int last_concept_id;
    int last_op_id;

    MacNARSEngine() : last_concept_id(-1), last_op_id(-1) {
        // Pre-allocate capacities
        semantic_manifold.reserve(100000);
        causal_graph.reserve(500000);
        desire_vector.reserve(100000);
    }
    
    // Ingest continuous perception into discrete symbolic concept
    int ingest_perception(const MetalConceptEmbedding& obs) {
        float similarity_score;
        int best_id = bag_engine.find_closest_concept(semantic_manifold, obs, similarity_score);
        
        // If novel state (or first state), append to manifold
        if (best_id == -1 || similarity_score < 0.95f) {
            best_id = semantic_manifold.size();
            semantic_manifold.push_back(obs);
            desire_vector.push_back(0); // Initialize desire to 0
        }
        return best_id;
    }
    
    // Learn causal sequence dynamically (NAL-8 Temporal Induction)
    void learn_causality(int current_concept_id) {
        if (last_concept_id != -1 && last_op_id != -1) {
            MetalCausalRule new_rule = {
                (uint32_t)last_concept_id, 
                (uint32_t)last_op_id, 
                (uint32_t)current_concept_id, 
                90000 // Fixed initial expectation of 0.9 for simplicity in v8
            };
            causal_graph.push_back(new_rule);
        }
    }
    
    // Derive optimal action given current state and global goals
    int decide_action(int current_concept_id) {
        // Run deep planning backwards from goals
        // We run 3 iterations (planning 3 steps ahead)
        graph_engine.derive_goals(causal_graph, desire_vector, 3);
        
        int best_op = -1;
        uint32_t best_utility = 0;
        
        // Find best immediately available action
        for (const auto& rule : causal_graph) {
            if (rule.pre_id == (uint32_t)current_concept_id) {
                uint32_t utility = (uint32_t)(((uint64_t)desire_vector[rule.post_id] * (uint64_t)rule.truth_exp_scaled) / 100000);
                if (utility > best_utility) {
                    best_utility = utility;
                    best_op = rule.op_id;
                }
            }
        }
        
        // Epsilon greedy exploration fallback could go here
        if (best_op == -1) {
            best_op = rand() % 6; // Assuming 6 discrete actions for MiniGrid
        }
        
        last_concept_id = current_concept_id;
        last_op_id = best_op;
        
        return best_op;
    }
    
    // External interface to inject goals (e.g., Reward states)
    void set_goal(int concept_id, float desire_float) {
        if (concept_id >= 0 && concept_id < desire_vector.size()) {
            desire_vector[concept_id] = (uint32_t)(desire_float * 100000);
        }
    }
    
    // Core Agent Loop
    int step(const MetalConceptEmbedding& obs, float reward) {
        int current_id = ingest_perception(obs);
        learn_causality(current_id);
        
        if (reward > 0.0f) {
            set_goal(current_id, 1.0f); // Reinforce current state as goal
        }
        
        return decide_action(current_id);
    }
};

#endif
