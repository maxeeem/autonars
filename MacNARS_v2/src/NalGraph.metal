#include <metal_stdlib>
using namespace metal;

// MacNARS v7: NAL-8 Backward Chaining via Parallel Graph Propagation

struct CausalRule {
    uint pre_id;
    uint op_id;
    uint post_id;
    uint truth_exp_scaled; // Truth expectation scaled by 100000 (0 to 100000)
};

kernel void propagate_desire(
    device const CausalRule* rules [[ buffer(0) ]],
    device atomic_uint* desire_vector [[ buffer(1) ]],
    uint id [[ thread_position_in_grid ]]
) {
    CausalRule rule = rules[id];
    
    // Read the current desire of the post-condition (the goal)
    uint post_desire = atomic_load_explicit(&desire_vector[rule.post_id], memory_order_relaxed);
    
    if (post_desire > 0) {
        // NAL Deduction: SubGoal Desire = Goal Desire * Rule Expectation
        // We do this in integer arithmetic: (post_desire * rule_exp) / 100000
        uint derived_desire = (uint)(((ulong)post_desire * (ulong)rule.truth_exp_scaled) / 100000);
        
        // Propagate backwards: Update the desire of the pre-condition
        uint current_pre_desire = atomic_load_explicit(&desire_vector[rule.pre_id], memory_order_relaxed);
        
        // atomic_max to keep the highest desire path
        while (derived_desire > current_pre_desire) {
            bool success = atomic_compare_exchange_weak_explicit(
                &desire_vector[rule.pre_id], 
                &current_pre_desire, 
                derived_desire, 
                memory_order_relaxed, 
                memory_order_relaxed
            );
            if (success) break;
            // current_pre_desire is automatically updated if exchange fails
        }
    }
}
