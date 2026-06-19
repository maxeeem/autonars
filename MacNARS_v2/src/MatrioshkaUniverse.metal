#include <metal_stdlib>
using namespace metal;

// MacNARS vOmega: Cosmological Lattice Simulation
// Implements the Computational Universe Hypothesis & Holographic Principle

struct Voxel {
    float energy;       // Raw physical energy (diffuses from center)
    float structure;    // Density of the megastructure
    uint concept_id;    // Symbolic NAL information 
    uint desire;        // Goal propagation across the surface
};

// 3D Grid dimensions (for the POC, we simulate a small local universe)
constant int GRID_SIZE = 64;

int get_index(int x, int y, int z) {
    // Clamp to boundaries to prevent out-of-bounds access
    x = clamp(x, 0, GRID_SIZE - 1);
    y = clamp(y, 0, GRID_SIZE - 1);
    z = clamp(z, 0, GRID_SIZE - 1);
    return z * GRID_SIZE * GRID_SIZE + y * GRID_SIZE + x;
}

kernel void simulate_universe_tick(
    device const Voxel* universe_in [[ buffer(0) ]],
    device Voxel* universe_out [[ buffer(1) ]],
    uint3 id [[ thread_position_in_grid ]]
) {
    if (id.x >= GRID_SIZE || id.y >= GRID_SIZE || id.z >= GRID_SIZE) return;
    
    int current_idx = get_index(id.x, id.y, id.z);
    Voxel current = universe_in[current_idx];
    
    float neighbor_energy = 0.0;
    float neighbor_structure = 0.0;
    uint max_desire = 0;
    uint dominant_concept = current.concept_id;

    // Gather from 6 immediate orthogonal neighbors (von Neumann neighborhood)
    int offsets[6][3] = {
        {1,0,0}, {-1,0,0}, {0,1,0}, {0,-1,0}, {0,0,1}, {0,0,-1}
    };
    
    for (int i = 0; i < 6; ++i) {
        int nx = id.x + offsets[i][0];
        int ny = id.y + offsets[i][1];
        int nz = id.z + offsets[i][2];
        
        if (nx >= 0 && nx < GRID_SIZE && ny >= 0 && ny < GRID_SIZE && nz >= 0 && nz < GRID_SIZE) {
            int n_idx = get_index(nx, ny, nz);
            Voxel neighbor = universe_in[n_idx];
            
            neighbor_energy += neighbor.energy;
            neighbor_structure += neighbor.structure;
            
            // Information propagates holographically along structural surfaces
            if (neighbor.structure > 0.5 && current.structure > 0.5) {
                if (neighbor.desire > max_desire) {
                    max_desire = neighbor.desire;
                    dominant_concept = neighbor.concept_id;
                }
            }
        }
    }
    
    // Physics Rules
    // 1. Energy Diffusion (Radiation from the central star)
    float new_energy = current.energy * 0.9 + (neighbor_energy / 6.0) * 0.1;
    
    // 2. Matrioshka Assembly (Structure builds where energy gradient is optimal)
    float new_structure = current.structure;
    if (new_energy > 0.2 && new_energy < 0.8) {
        new_structure = min(1.0, current.structure + 0.05); // Accrete mass
    } else if (new_energy >= 0.8) {
        new_structure = max(0.0, current.structure - 0.1); // Star is too hot, melts structure
    }

    // 3. Symbolic Logic computation consumes energy
    uint new_desire = current.desire;
    uint new_concept = current.concept_id;
    
    if (new_structure > 0.5 && new_energy > 0.1) {
        // NAL Graph Derivation embedded in spatial physics
        if (max_desire > current.desire) {
            // "Desire" decays spatially, simulating Rule Expectation < 1.0
            new_desire = (max_desire * 90) / 100; 
            new_concept = dominant_concept;
            
            // Consumes energy to compute
            new_energy -= 0.01; 
        }
    }

    universe_out[current_idx] = { max(0.0, new_energy), new_structure, new_concept, new_desire };
}
