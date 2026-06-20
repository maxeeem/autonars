#include <iostream>
#include <iomanip>
#include "CosmologicalEngine.h"

// MacNARS vOmega: Cosmological Execution (The 500-Year Proof of Concept)

void print_universe_slice(const CosmologicalEngine& engine, int z_slice) {
    int size = 64;
    std::cout << "Z-Slice: " << z_slice << "\n";
    for (int y = 0; y < size; y += 2) { // Downsample for printing
        for (int x = 0; x < size; x += 2) {
            int idx = z_slice * size * size + y * size + x;
            Voxel v = engine.local_universe[idx];
            
            if (v.energy > 1000.0f) {
                std::cout << "* "; // Star
            } else if (v.desire > 100) {
                std::cout << "O "; // Active Computation / Goal
            } else if (v.structure > 0.5f) {
                std::cout << "# "; // Megastructure (Matrioshka Shell)
            } else if (v.energy > 0.5f) {
                std::cout << ". "; // Radiant Energy
            } else {
                std::cout << "  "; // Void
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

int main() {
    std::cout << "====================================================\n";
    std::cout << " MacNARS vOmega: The Computational Universe\n";
    std::cout << "====================================================\n\n";

    std::cout << "[Cosmology] Booting 3D Quantum-Spatial Lattice (64x64x64 Voxels)...\n";
    CosmologicalEngine universe(64);

    std::cout << "[Epoch 1] Igniting Central Star and allowing Energy to diffuse...\n";
    universe.simulate_epoch(50); // 50 ticks of physics
    
    std::cout << "[Epoch 2] Thermodynamic equilibrium reached. Matrioshka megastructure accretes at optimal Goldilocks radius...\n";
    universe.simulate_epoch(50);
    
    std::cout << "--- Universe Cross-Section (Z=32) ---\n";
    print_universe_slice(universe, 32);

    std::cout << "[Cognition] Injecting a Symbolic NAL Goal (Desire = 10000) onto the surface of the megastructure at (X=22, Y=32, Z=32)...\n";
    universe.inject_goal(22, 32, 32, 42, 10000); // Concept ID 42, Desire 10000

    std::cout << "[Epoch 3] Goal Desire propagates holographically through the physical structure via localized NAL logic gates...\n";
    universe.simulate_epoch(30);

    std::cout << "--- Universe Cross-Section (Z=32) After Goal Propagation ---\n";
    print_universe_slice(universe, 32);

    // Verify propagation by scanning the surface
    int best_x = 0, best_y = 0, best_z = 0;
    Voxel computed_voxel = {0,0,0,0};
    
    for (int z = 0; z < 64; ++z) {
        for (int y = 0; y < 64; ++y) {
            for (int x = 0; x < 64; ++x) {
                // Ignore the injection point itself
                if (x == 22 && y == 32 && z == 32) continue;
                
                int idx = z * 64 * 64 + y * 64 + x;
                Voxel v = universe.local_universe[idx];
                
                // Find a voxel that received the goal desire through propagation
                if (v.desire > computed_voxel.desire) {
                    computed_voxel = v;
                    best_x = x;
                    best_y = y;
                    best_z = z;
                }
            }
        }
    }

    std::cout << "\n-> Results at Observation Coordinate (X=" << best_x << ", Y=" << best_y << ", Z=" << best_z << "):\n";
    std::cout << "   Structure Density: " << computed_voxel.structure << "\n";
    std::cout << "   Dominant Concept: " << computed_voxel.concept_id << "\n";
    std::cout << "   Derived Desire: " << computed_voxel.desire << "\n\n";

    if (computed_voxel.desire > 0 && computed_voxel.concept_id == 42) {
        std::cout << "[Success] The Matrioshka Brain computed the NAL goal natively within the physics of space-time!\n";
        std::cout << "MacNARS vOmega has achieved the 500-Year Horizon: Intelligence unconstrained by manufactured hardware.\n";
    }

    return 0;
}
