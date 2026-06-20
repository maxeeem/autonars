#include "MetalBagEngine.h"
#include <vector>

extern "C" {
    void* create_engine() {
        return new MetalBagEngine();
    }
    
    void destroy_engine(void* engine) {
        delete static_cast<MetalBagEngine*>(engine);
    }
    
    int find_closest(void* engine, const float* memory_bank_vectors, int memory_size, const float* query_vector, float* out_sim) {
        MetalBagEngine* eng = static_cast<MetalBagEngine*>(engine);
        
        std::vector<MetalConceptEmbedding> memory(memory_size);
        for(int i = 0; i < memory_size; ++i) {
            for(int j = 0; j < 6; ++j) {
                memory[i].vector[j] = memory_bank_vectors[i*6 + j];
            }
        }
        
        MetalConceptEmbedding query;
        for(int j = 0; j < 6; ++j) {
            query.vector[j] = query_vector[j];
        }
        
        return eng->find_closest_concept(memory, query, *out_sim);
    }
}
