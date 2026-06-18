#include <metal_stdlib>
using namespace metal;

struct Item {
    int id; // concept ID or task ID
    float priority;
    float durability;
    float quality;
    // padding for alignment could be needed depending on C struct
};

kernel void decay_items(device Item* items [[buffer(0)]],
                        constant float& decay_factor [[buffer(1)]],
                        constant uint& num_items [[buffer(2)]],
                        uint id [[thread_position_in_grid]])
{
    if (id < num_items) {
        // Apply decay to priority based on durability
        // In NARS, priority decay is usually proportional to durability.
        // new_priority = priority * durability
        // We also apply a global decay factor.
        items[id].priority *= decay_factor * items[id].durability;
    }
}
