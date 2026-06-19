#ifndef MACNARS_METAL_BAG_ENGINE_H
#define MACNARS_METAL_BAG_ENGINE_H

#import <Metal/Metal.h>
#import <Foundation/Foundation.h>
#include <vector>
#include <iostream>

struct MetalConceptEmbedding {
    float vector[6];
};

struct GravityResult {
    float similarity;
    int concept_id;
};

class MetalBagEngine {
private:
    id<MTLDevice> device;
    id<MTLCommandQueue> commandQueue;
    id<MTLComputePipelineState> pipelineState;

public:
    MetalBagEngine() {
        device = MTLCreateSystemDefaultDevice();
        commandQueue = [device newCommandQueue];
        
        NSError* error = nil;
        NSString* path = @"src/SemanticGravity.metal";
        NSString* source = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:&error];
        
        if (!source) {
            std::cerr << "Failed to load Metal shader: " << [[error localizedDescription] UTF8String] << "\n";
            return;
        }

        id<MTLLibrary> library = [device newLibraryWithSource:source options:nil error:&error];
        id<MTLFunction> function = [library newFunctionWithName:@"compute_semantic_gravity"];
        pipelineState = [device newComputePipelineStateWithFunction:function error:&error];
    }

    // Searches the entire Bag for the closest semantic match in O(1) perceived time using GPU threads
    int find_closest_concept(const std::vector<MetalConceptEmbedding>& memory_bank, const MetalConceptEmbedding& query, float& out_similarity) {
        if (memory_bank.empty() || pipelineState == nil) {
            out_similarity = -1.0f;
            return -1;
        }

        NSUInteger num_concepts = memory_bank.size();
        
        id<MTLBuffer> memoryBuffer = [device newBufferWithBytes:memory_bank.data() 
                                                        length:num_concepts * sizeof(MetalConceptEmbedding) 
                                                       options:MTLResourceStorageModeShared];
                                                       
        id<MTLBuffer> queryBuffer = [device newBufferWithBytes:&query 
                                                       length:sizeof(MetalConceptEmbedding) 
                                                      options:MTLResourceStorageModeShared];
                                                      
        id<MTLBuffer> resultBuffer = [device newBufferWithLength:num_concepts * sizeof(GravityResult) 
                                                         options:MTLResourceStorageModeShared];

        id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];
        id<MTLComputeCommandEncoder> encoder = [commandBuffer computeCommandEncoder];
        
        [encoder setComputePipelineState:pipelineState];
        [encoder setBuffer:memoryBuffer offset:0 atIndex:0];
        [encoder setBuffer:queryBuffer offset:0 atIndex:1];
        [encoder setBuffer:resultBuffer offset:0 atIndex:2];

        MTLSize threadsPerGrid = MTLSizeMake(num_concepts, 1, 1);
        
        // Optimize thread group size based on device
        NSUInteger w = pipelineState.maxTotalThreadsPerThreadgroup;
        if (w > num_concepts) w = num_concepts;
        MTLSize threadsPerThreadgroup = MTLSizeMake(w, 1, 1);
        
        [encoder dispatchThreads:threadsPerGrid threadsPerThreadgroup:threadsPerThreadgroup];
        [encoder endEncoding];
        [commandBuffer commit];
        [commandBuffer waitUntilCompleted];

        GravityResult* results = (GravityResult*)resultBuffer.contents;
        
        // Find the max locally (since reducing on GPU requires another kernel)
        // With 10,000 items, a local CPU max-find is instantaneous.
        float max_sim = -1.0f;
        int best_id = -1;
        for (NSUInteger i = 0; i < num_concepts; ++i) {
            if (results[i].similarity > max_sim) {
                max_sim = results[i].similarity;
                best_id = results[i].concept_id;
            }
        }
        
        out_similarity = max_sim;
        return best_id;
    }
};

#endif
