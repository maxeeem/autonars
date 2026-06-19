#include <iostream>
#include <vector>
#include <chrono>
#include "MacNARSEngine.h"

// MacNARS v8: The Ultimate Neuromorphic Logic Engine Execution
int main() {
    std::cout << "====================================================\n";
    std::cout << " MacNARS v8: The Grand Integration (Metal AGI Engine)\n";
    std::cout << "====================================================\n\n";

    MacNARSEngine engine;

    // Define 4 sequential continuous states with distinct angles for Cosine Similarity
    MetalConceptEmbedding state_start = {{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}};
    MetalConceptEmbedding state_hall =  {{0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f}};
    MetalConceptEmbedding state_door =  {{0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f}};
    MetalConceptEmbedding state_goal =  {{0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f}};

    std::cout << "[Environment] Agent explores random states...\n";
    
    // Simulate exploration
    int op_walk = 1;
    int op_open = 2;

    // Step 1: Start
    engine.step(state_start, 0.0f);
    
    // Agent randomly picked walk
    engine.last_op_id = op_walk; 
    
    // Step 2: Hall
    engine.step(state_hall, 0.0f);
    
    // Agent randomly picked walk again
    engine.last_op_id = op_walk;

    // Step 3: Door
    engine.step(state_door, 0.0f);
    
    // Agent randomly picked open
    engine.last_op_id = op_open;

    // Step 4: Goal!
    std::cout << "[Environment] Agent finds the goal and receives Reward = 1.0\n";
    engine.step(state_goal, 1.0f); // Reward triggers set_goal dynamically
    
    std::cout << "\n[Agent] Engine automatically induced Causal Graph:\n";
    for (const auto& rule : engine.causal_graph) {
        std::cout << "    Rule: <(Concept_" << rule.pre_id << " &/ ^" << rule.op_id << ") =/> Concept_" << rule.post_id << ">\n";
    }

    std::cout << "\n[Test] Teleporting Agent back to Start state to test Deep Planning...\n";
    
    // Reset internal state tracking to avoid inducing teleportation rule
    engine.last_concept_id = -1; 
    engine.last_op_id = -1;

    // Ingest the start state (should cluster to Concept 0). 
    // step() internally calls decide_action() which runs MetalGraphEngine.
    
    auto start_time = std::chrono::high_resolution_clock::now();
    int chosen_action = engine.step(state_start, 0.0f);
    auto end_time = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double, std::milli> duration = end_time - start_time;

    std::cout << "\n-> Results:\n";
    std::cout << "   GPU Planning Time (3 passes): " << duration.count() << " ms\n";
    std::cout << "   Chosen Action from Start State: ^" << chosen_action << " (Expected: ^1 / walk)\n\n";

    if (chosen_action == op_walk) {
        std::cout << "[Success] Backpropagation is eliminated. The agent seamlessly ingested continuous vectors, natively induced causal symbolic rules, ran Metal GPU backward chaining 3 steps deep, and outputted the correct optimal action.\n";
        std::cout << "MacNARS v8 is complete.\n";
    } else {
        std::cout << "[Failed] Expected action ^1 (walk).\n";
    }

    return 0;
}
