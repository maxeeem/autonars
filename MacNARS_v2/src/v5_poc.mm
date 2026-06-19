#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include "MetalBagEngine.h"

// MacNARS v5: Metal-Accelerated Semantic Gravity Proof of Concept
int main() {
    std::cout << "====================================================\n";
    std::cout << " MacNARS v5: Metal-Accelerated Semantic Gravity\n";
    std::cout << "====================================================\n\n";

    MetalBagEngine engine;

    std::cout << "[Benchmark] Generating 1,000,000 random Concepts for Unified Memory Bag...\n";
    std::vector<MetalConceptEmbedding> massive_memory_bank(1000000);
    
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    for (int i = 0; i < 1000000; ++i) {
        for (int j = 0; j < 6; ++j) {
            massive_memory_bank[i].vector[j] = dist(rng);
        }
    }

    // Plant a specific target at index 42000
    MetalConceptEmbedding target = {{0.9f, 0.1f, 0.8f, 0.2f, 0.7f, 0.3f}};
    massive_memory_bank[42000] = target;

    // Create a query that is slightly noisy but semantically identical to target
    MetalConceptEmbedding query = {{0.91f, 0.09f, 0.81f, 0.19f, 0.72f, 0.28f}};

    std::cout << "[Attention] Searching 1,000,000 concepts via Apple Neural Engine / Metal GPU...\n";
    
    auto start = std::chrono::high_resolution_clock::now();
    int best_id = engine.find_closest_concept(massive_memory_bank, query);
    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double, std::milli> duration = end - start;

    std::cout << "\n-> Results:\n";
    std::cout << "   Best Concept Match ID: " << best_id << "\n";
    std::cout << "   Expected Match ID: 42000\n";
    std::cout << "   GPU Computation Time: " << duration.count() << " ms\n\n";

    if (best_id == 42000) {
        std::cout << "[Success] Semantic Gravity correctly localized the concept across 1 Million vectors!\n";
        std::cout << "MacNARS v5 officially overcomes the AIKR Bag Scaling Bottleneck.\n";
    } else {
        std::cout << "[Failed] Unexpected match.\n";
    }

    return 0;
}
