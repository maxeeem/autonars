#import <Metal/Metal.h>
#import <Foundation/Foundation.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

struct SensorState {
    float current_value;
    float target_value;
    float integral_error;
    float last_error;
    float confidence; 
    float priority;   
};

struct PIDConstants {
    float Kp;
    float Ki;
    float Kd;
    float dt;
};

class HyperCausalEngine {
private:
    id<MTLDevice> device;
    id<MTLCommandQueue> commandQueue;
    id<MTLComputePipelineState> pipelineState;

public:
    HyperCausalEngine() {
        device = MTLCreateSystemDefaultDevice();
        commandQueue = [device newCommandQueue];
        
        NSError* error = nil;
        NSString* path = @"/Users/maxeeem/autonars/MacNARS_v2/src/HyperCausal.metal";
        NSString* source = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:&error];
        
        id<MTLLibrary> library = [device newLibraryWithSource:source options:nil error:&error];
        id<MTLFunction> function = [library newFunctionWithName:@"hyper_causal_reflex_loop"];
        pipelineState = [device newComputePipelineStateWithFunction:function error:&error];
    }

    void execute_reflex(SensorState& state, const PIDConstants& constants, float& motor_output) {
        id<MTLBuffer> stateBuffer = [device newBufferWithBytes:&state length:sizeof(SensorState) options:MTLResourceStorageModeShared];
        id<MTLBuffer> constBuffer = [device newBufferWithBytes:&constants length:sizeof(PIDConstants) options:MTLResourceStorageModeShared];
        id<MTLBuffer> outputBuffer = [device newBufferWithLength:sizeof(float) options:MTLResourceStorageModeShared];

        id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];
        id<MTLComputeCommandEncoder> encoder = [commandBuffer computeCommandEncoder];
        
        [encoder setComputePipelineState:pipelineState];
        [encoder setBuffer:stateBuffer offset:0 atIndex:0];
        [encoder setBuffer:constBuffer offset:0 atIndex:1];
        [encoder setBuffer:outputBuffer offset:0 atIndex:2];

        MTLSize threadsPerGroup = MTLSizeMake(1, 1, 1);
        MTLSize groups = MTLSizeMake(1, 1, 1);
        
        [encoder dispatchThreadgroups:groups threadsPerThreadgroup:threadsPerGroup];
        [encoder endEncoding];
        [commandBuffer commit];
        [commandBuffer waitUntilCompleted];

        // Retrieve updated state and output
        state = *(SensorState*)stateBuffer.contents;
        motor_output = *(float*)outputBuffer.contents;
    }
};

int main() {
    std::cout << "====================================================\n";
    std::cout << " MacNARS v2: Hyper-Causal Drone Altitude Simulation\n";
    std::cout << "====================================================\n";

    HyperCausalEngine engine;

    SensorState nars_memory = { 0.0f, 10.0f, 0.0f, 0.0f, 1.0f, 0.1f };
    PIDConstants pid = { 2.5f, 0.1f, 1.5f, 0.001f }; // 1000Hz = 0.001s dt
    float motor_thrust = 0.0f;

    // Physics constants
    const float gravity = -9.81f;
    const float mass = 1.0f;
    float velocity = 0.0f;
    float altitude = 0.0f; // Start at ground
    
    std::cout << "Target Altitude: " << nars_memory.target_value << "m\n\n";
    std::cout << std::left << std::setw(10) << "Time(s)" 
              << std::setw(15) << "Altitude(m)" 
              << std::setw(15) << "NARS Priority" 
              << "Event\n";
    std::cout << "----------------------------------------------------\n";

    // Simulate 5 seconds. NARS runs at 10Hz (every 100ms)
    // Reflex runs at 1000Hz (every 1ms)
    for (int nars_tick = 0; nars_tick <= 50; ++nars_tick) {
        float current_time = nars_tick * 0.1f;
        
        // 1. NARS Cortex Cycle (10Hz)
        if (nars_memory.priority > 0.8f) {
            std::cout << std::left << std::setw(10) << current_time 
                      << std::setw(15) << altitude 
                      << std::setw(15) << nars_memory.priority 
                      << "--> NARS SURPRISE! Causal anomaly detected. Adapting strategy (Kp: " << pid.Kp << " -> " << pid.Kp + 2.0f << ")\n";
            // NARS Reasoning: High surprise means wind or obstacle. 
            // Adjust PID aggressiveness or target.
            pid.Kp += 2.0f; // Increase proportional gain significantly
            nars_memory.priority = 0.1f; // Reset attention
        } else if (nars_tick % 5 == 0) { // Print every 0.5s
            std::string event = (current_time >= 2.0f && current_time < 2.5f) ? "[WIND GUST ACTIVE]" : "NARS Monitoring...";
            std::cout << std::left << std::setw(10) << current_time 
                      << std::setw(15) << altitude 
                      << std::setw(15) << nars_memory.priority 
                      << event << "\n";
        }

        // 2. Reflex Spinal Cycle (1000Hz - 100 sub-ticks)
        for (int reflex_tick = 0; reflex_tick < 100; ++reflex_tick) {
            float wind_force = 0.0f;
            
            // Introduce chaotic wind disturbance at t=2.0s to t=2.5s
            if (current_time >= 2.0f && current_time < 2.5f) {
                wind_force = -15.0f; // Strong downdraft
            }

            nars_memory.current_value = altitude;
            
            // Execute Metal PID Reflex
            engine.execute_reflex(nars_memory, pid, motor_thrust);

            // Physics Update (Euler integration)
            // Clamp thrust (motors can't pull down, and have a max thrust)
            motor_thrust = std::max(0.0f, std::min(motor_thrust, 30.0f)); 
            
            float acceleration = (motor_thrust + (gravity * mass) + wind_force) / mass;
            velocity += acceleration * pid.dt;
            altitude += velocity * pid.dt;
            
            // Floor collision
            if (altitude < 0.0f) {
                altitude = 0.0f;
                velocity = 0.0f;
            }
        }
    }
    
    std::cout << "====================================================\n";
    std::cout << "Simulation Complete.\n";
    return 0;
}
