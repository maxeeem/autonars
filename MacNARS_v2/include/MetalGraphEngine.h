#ifndef MACNARS_METAL_GRAPH_ENGINE_H
#define MACNARS_METAL_GRAPH_ENGINE_H

#import <Metal/Metal.h>
#import <Foundation/Foundation.h>
#include <vector>
#include <iostream>

struct MetalCausalRule {
    uint32_t pre_id;
    uint32_t op_id;
    uint32_t post_id;
    uint32_t truth_exp_scaled;
};

class MetalGraphEngine {
private:
    id<MTLDevice> device;
    id<MTLCommandQueue> commandQueue;
    id<MTLComputePipelineState> pipelineState;

public:
    MetalGraphEngine() {
        device = MTLCreateSystemDefaultDevice();
        commandQueue = [device newCommandQueue];
        
        NSError* error = nil;
        NSString* path = @"src/NalGraph.metal";
        NSString* source = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:&error];
        
        if (!source) {
            std::cerr << "[MetalGraphEngine] Failed to load Metal shader: " << [[error localizedDescription] UTF8String] << "\n";
            return;
        }

        id<MTLLibrary> library = [device newLibraryWithSource:source options:nil error:&error];
        id<MTLFunction> function = [library newFunctionWithName:@"propagate_desire"];
        pipelineState = [device newComputePipelineStateWithFunction:function error:&error];
    }

    // Runs Bellman-Ford style backwards chaining on the GPU
    // Executes 'iterations' passes over the rule set to propagate desires backwards through the causal graph.
    void derive_goals(const std::vector<MetalCausalRule>& rules, std::vector<uint32_t>& desire_vector, int iterations = 10) {
        if (rules.empty() || desire_vector.empty() || pipelineState == nil) return;

        NSUInteger num_rules = rules.size();
        NSUInteger num_concepts = desire_vector.size();
        
        id<MTLBuffer> rulesBuffer = [device newBufferWithBytes:rules.data() 
                                                        length:num_rules * sizeof(MetalCausalRule) 
                                                       options:MTLResourceStorageModeShared];
                                                       
        id<MTLBuffer> desireBuffer = [device newBufferWithBytes:desire_vector.data() 
                                                         length:num_concepts * sizeof(uint32_t) 
                                                        options:MTLResourceStorageModeShared];

        for (int i = 0; i < iterations; ++i) {
            id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];
            id<MTLComputeCommandEncoder> encoder = [commandBuffer computeCommandEncoder];
            
            [encoder setComputePipelineState:pipelineState];
            [encoder setBuffer:rulesBuffer offset:0 atIndex:0];
            [encoder setBuffer:desireBuffer offset:0 atIndex:1];

            MTLSize threadsPerGrid = MTLSizeMake(num_rules, 1, 1);
            
            NSUInteger w = pipelineState.maxTotalThreadsPerThreadgroup;
            if (w > num_rules) w = num_rules;
            MTLSize threadsPerThreadgroup = MTLSizeMake(w, 1, 1);
            
            [encoder dispatchThreads:threadsPerGrid threadsPerThreadgroup:threadsPerThreadgroup];
            [encoder endEncoding];
            [commandBuffer commit];
            [commandBuffer waitUntilCompleted];
        }

        // Read back the propagated desire vector
        memcpy(desire_vector.data(), desireBuffer.contents, num_concepts * sizeof(uint32_t));
    }
};

#endif
