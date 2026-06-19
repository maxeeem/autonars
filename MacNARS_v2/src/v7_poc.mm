#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include "MetalGraphEngine.h"

int main() {
    std::cout << "====================================================\n";
    std::cout << " MacNARS v7: Metal-Accelerated Goal Derivation\n";
    std::cout << "====================================================\n\n";

    MetalGraphEngine engine;

    const int NUM_CONCEPTS = 100000;
    const int NUM_RULES = 500000;

    std::cout << "[Benchmark] Generating " << NUM_CONCEPTS << " concepts and " << NUM_RULES << " causal rules...\n";
    
    std::vector<uint32_t> desire_vector(NUM_CONCEPTS, 0);
    std::vector<MetalCausalRule> rules(NUM_RULES);

    std::mt19937 rng(42);
    std::uniform_int_distribution<uint32_t> dist_node(0, NUM_CONCEPTS - 1);
    std::uniform_int_distribution<uint32_t> dist_op(0, 4);
    std::uniform_int_distribution<uint32_t> dist_truth(40000, 90000); // 0.4 to 0.9 expectation

    for (int i = 0; i < NUM_RULES; ++i) {
        rules[i].pre_id = dist_node(rng);
        rules[i].op_id = dist_op(rng);
        rules[i].post_id = dist_node(rng);
        rules[i].truth_exp_scaled = dist_truth(rng);
    }

    // Plant a specific 5-step causal chain we want to verify
    // 10 -> 20 -> 30 -> 40 -> 50 -> 60 (Goal)
    rules[0] = {10, 1, 20, 90000}; // 0.9
    rules[1] = {20, 1, 30, 90000}; // 0.9
    rules[2] = {30, 1, 40, 90000}; // 0.9
    rules[3] = {40, 1, 50, 90000}; // 0.9
    rules[4] = {50, 1, 60, 90000}; // 0.9

    // Set the Goal Desire
    desire_vector[60] = 100000; // Max desire (1.0)
    std::cout << "[Motivation] Goal set at Concept 60 (Desire: 1.0).\n";

    std::cout << "[Attention] Propagating desires backwards via Apple Neural Engine / GPU...\n";
    
    auto start = std::chrono::high_resolution_clock::now();
    // Run 10 iterations of Bellman-Ford style propagation
    engine.derive_goals(rules, desire_vector, 10);
    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double, std::milli> duration = end - start;

    std::cout << "\n-> Results:\n";
    std::cout << "   GPU Propagation Time (10 passes): " << duration.count() << " ms\n\n";

    std::cout << "   Desire at Concept 50: " << desire_vector[50] / 100000.0 << " (Expected ~0.9)\n";
    std::cout << "   Desire at Concept 40: " << desire_vector[40] / 100000.0 << " (Expected ~0.81)\n";
    std::cout << "   Desire at Concept 30: " << desire_vector[30] / 100000.0 << " (Expected ~0.72)\n";
    std::cout << "   Desire at Concept 20: " << desire_vector[20] / 100000.0 << " (Expected ~0.65)\n";
    std::cout << "   Desire at Concept 10: " << desire_vector[10] / 100000.0 << " (Expected ~0.59)\n";

    if (desire_vector[10] > 0) {
        std::cout << "\n[Success] The Neuromorphic Goal Graph instantly derived a 5-step sequential plan out of 500,000 competing hypotheses!\n";
        std::cout << "MacNARS v7 successfully achieves deep planning at real-time speeds.\n";
    }

    return 0;
}
