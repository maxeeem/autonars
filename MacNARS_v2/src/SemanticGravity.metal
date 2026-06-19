#include <metal_stdlib>
using namespace metal;

// MacNARS v5: Semantic Gravity Metal compute shader

struct ConceptEmbedding {
    float vector[6]; // We use the 6-element Atari latent vector size for this POC
};

struct GravityResult {
    float similarity;
    int concept_id;
};

kernel void compute_semantic_gravity(
    device const ConceptEmbedding* concept_array [[ buffer(0) ]],
    device const ConceptEmbedding* query_vector [[ buffer(1) ]],
    device GravityResult* results [[ buffer(2) ]],
    uint id [[ thread_position_in_grid ]]
) {
    float dot_product = 0.0;
    float norm_a = 0.0;
    float norm_b = 0.0;
    
    // Unroll the loop for speed
    for (int i = 0; i < 6; ++i) {
        float a = concept_array[id].vector[i];
        float b = query_vector->vector[i];
        
        dot_product += a * b;
        norm_a += a * a;
        norm_b += b * b;
    }
    
    float similarity = dot_product / (sqrt(norm_a) * sqrt(norm_b));
    
    results[id].concept_id = id;
    results[id].similarity = similarity;
}
