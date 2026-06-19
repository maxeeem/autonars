#ifndef MACNARS_COSMOLOGICAL_ENGINE_H
#define MACNARS_COSMOLOGICAL_ENGINE_H

#import <Metal/Metal.h>
#import <Foundation/Foundation.h>
#include <vector>
#include <iostream>
#include <cmath>

struct Voxel {
    float energy;
    float structure;
    uint32_t concept_id;
    uint32_t desire;
};

class CosmologicalEngine {
private:
    id<MTLDevice> device;
    id<MTLCommandQueue> commandQueue;
    id<MTLComputePipelineState> pipelineState;
    
    int grid_size;
    int total_voxels;
    
    id<MTLBuffer> bufferA;
    id<MTLBuffer> bufferB;
    bool use_buffer_a;

public:
    std::vector<Voxel> local_universe;

    CosmologicalEngine(int size = 64) : grid_size(size), total_voxels(size * size * size), use_buffer_a(true) {
        device = MTLCreateSystemDefaultDevice();
        commandQueue = [device newCommandQueue];
        
        NSError* error = nil;
        NSString* path = @"src/MatrioshkaUniverse.metal";
        NSString* source = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:&error];
        
        if (!source) {
            std::cerr << "[Cosmology] Failed to load Universe physics: " << [[error localizedDescription] UTF8String] << "\n";
            return;
        }

        id<MTLLibrary> library = [device newLibraryWithSource:source options:nil error:&error];
        id<MTLFunction> function = [library newFunctionWithName:@"simulate_universe_tick"];
        pipelineState = [device newComputePipelineStateWithFunction:function error:&error];
        
        local_universe.resize(total_voxels, {0.0f, 0.0f, 0, 0});
        
        // Ignite the central star (Voxel coordinate 32,32,32)
        int center = (grid_size/2) * grid_size * grid_size + (grid_size/2) * grid_size + (grid_size/2);
        local_universe[center].energy = 1000000.0f; // Infinite energy source
        
        bufferA = [device newBufferWithBytes:local_universe.data() 
                                      length:total_voxels * sizeof(Voxel) 
                                     options:MTLResourceStorageModeShared];
        bufferB = [device newBufferWithLength:total_voxels * sizeof(Voxel) 
                                      options:MTLResourceStorageModeShared];
    }
    
    // Inject a NAL symbolic goal into the outer crust of the megastructure
    void inject_goal(int x, int y, int z, uint32_t concept, uint32_t desire_val) {
        int idx = z * grid_size * grid_size + y * grid_size + x;
        local_universe[idx].concept_id = concept;
        local_universe[idx].desire = desire_val;
        local_universe[idx].structure = 1.0f; // Anchor it to physical matter
        
        // Push to buffer
        Voxel* data = use_buffer_a ? (Voxel*)bufferA.contents : (Voxel*)bufferB.contents;
        data[idx] = local_universe[idx];
    }

    void simulate_epoch(int ticks) {
        for (int i = 0; i < ticks; ++i) {
            id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];
            id<MTLComputeCommandEncoder> encoder = [commandBuffer computeCommandEncoder];
            
            [encoder setComputePipelineState:pipelineState];
            
            if (use_buffer_a) {
                [encoder setBuffer:bufferA offset:0 atIndex:0];
                [encoder setBuffer:bufferB offset:0 atIndex:1];
            } else {
                [encoder setBuffer:bufferB offset:0 atIndex:0];
                [encoder setBuffer:bufferA offset:0 atIndex:1];
            }
            
            // Constrain the star's energy
            int center = (grid_size/2) * grid_size * grid_size + (grid_size/2) * grid_size + (grid_size/2);
            Voxel* current_data = use_buffer_a ? (Voxel*)bufferA.contents : (Voxel*)bufferB.contents;
            current_data[center].energy = 1000000.0f;

            MTLSize threadsPerGrid = MTLSizeMake(grid_size, grid_size, grid_size);
            
            NSUInteger w = pipelineState.threadExecutionWidth;
            NSUInteger h = pipelineState.maxTotalThreadsPerThreadgroup / w;
            MTLSize threadsPerThreadgroup = MTLSizeMake(w, h, 1);
            
            [encoder dispatchThreads:threadsPerGrid threadsPerThreadgroup:threadsPerThreadgroup];
            [encoder endEncoding];
            [commandBuffer commit];
            [commandBuffer waitUntilCompleted];
            
            use_buffer_a = !use_buffer_a; // Swap buffers
        }
        
        // Sync back to local memory
        Voxel* latest_data = use_buffer_a ? (Voxel*)bufferA.contents : (Voxel*)bufferB.contents;
        memcpy(local_universe.data(), latest_data, total_voxels * sizeof(Voxel));
    }
};

#endif
