#ifndef MACNARS_QUANTUM_NAL_H
#define MACNARS_QUANTUM_NAL_H

#include <complex>
#include <vector>
#include <cmath>

// MacNARS v11: Quantum-Symbolic NAL Simulation
// Represents Truth and Concepts as Quantum Probability Amplitudes

typedef std::complex<double> QuantumAmplitude;

struct QuantumConcept {
    int id;
    QuantumAmplitude amplitude; // The complex wave function of the concept
};

class QuantumBagEngine {
private:
    std::vector<QuantumConcept> superposition_state;
    int num_states;

public:
    QuantumBagEngine(int n) : num_states(n) {
        // Initialize all concepts in a uniform superposition
        double initial_prob = 1.0 / std::sqrt((double)num_states);
        for (int i = 0; i < num_states; ++i) {
            superposition_state.push_back({i, QuantumAmplitude(initial_prob, 0.0)});
        }
    }

    // A Causal Rule (Pre -> Post) acts as a Unitary Gate mixing the amplitudes
    // For this simulation, we apply a phase shift and amplitude transfer
    void apply_causal_gate(int pre_id, int post_id, double coupling_strength) {
        QuantumAmplitude pre_amp = superposition_state[pre_id].amplitude;
        QuantumAmplitude post_amp = superposition_state[post_id].amplitude;

        // Simulate a unitary rotation (like an RX/RY gate) transferring probability
        // If Pre is active, it "causes" Post to become active
        double theta = coupling_strength * M_PI / 2.0;
        
        QuantumAmplitude new_pre = pre_amp * std::cos(theta) - post_amp * std::sin(theta);
        QuantumAmplitude new_post = pre_amp * std::sin(theta) + post_amp * std::cos(theta);

        superposition_state[pre_id].amplitude = new_pre;
        superposition_state[post_id].amplitude = new_post;
    }

    // Mark the goal state by flipping its phase (The Quantum Oracle)
    void apply_goal_oracle(int goal_id) {
        superposition_state[goal_id].amplitude *= -1.0;
    }

    // Apply the Diffusion Operator (Grover's Algorithm step) to amplify the goal's causal precursors
    void apply_diffusion_operator() {
        // Calculate the mean amplitude
        QuantumAmplitude mean(0.0, 0.0);
        for (const auto& concept : superposition_state) {
            mean += concept.amplitude;
        }
        mean /= (double)num_states;

        // Invert about the mean
        for (auto& concept : superposition_state) {
            concept.amplitude = (2.0 * mean) - concept.amplitude;
        }
    }

    // Collapse the wave function to observe the highest probability path
    int measure_optimal_state() const {
        double max_prob = -1.0;
        int best_id = -1;

        for (const auto& concept : superposition_state) {
            double prob = std::norm(concept.amplitude); // |psi|^2
            if (prob > max_prob) {
                max_prob = prob;
                best_id = concept.id;
            }
        }
        return best_id;
    }

    double get_probability(int id) const {
        return std::norm(superposition_state[id].amplitude);
    }
};

#endif
