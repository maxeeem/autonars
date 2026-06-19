#include <iostream>
#include <iomanip>
#include "QuantumNAL.h"

int main() {
    std::cout << "====================================================\n";
    std::cout << " MacNARS v11: Quantum-Symbolic Superposition (Simulation)\n";
    std::cout << "====================================================\n\n";

    const int TOTAL_CONCEPTS = 1024; // 10 qubits worth of states
    QuantumBagEngine q_engine(TOTAL_CONCEPTS);

    std::cout << "[Initialization] Memory Bag prepared in uniform quantum superposition.\n";
    std::cout << "  (Each of the " << TOTAL_CONCEPTS << " states has initial probability: " 
              << q_engine.get_probability(0) << ")\n\n";

    // Define a causal chain: State 42 -> Action -> State 100 (Goal)
    int state_pre = 42;
    int state_goal = 100;

    std::cout << "[Induction] Applying Causal Unitary Gate connecting Concept " << state_pre << " to Concept " << state_goal << "...\n";
    // Apply a weak coupling to simulate a learned temporal rule
    q_engine.apply_causal_gate(state_pre, state_goal, 0.5);

    // Goal Derivation via Amplitude Amplification (Grover's Algorithm)
    std::cout << "[Motivation] Goal set at Concept " << state_goal << " (Applying Quantum Oracle phase-flip)...\n";
    
    int grover_iterations = std::round((M_PI / 4.0) * std::sqrt((double)TOTAL_CONCEPTS));
    std::cout << "[Reasoning] Executing " << grover_iterations << " iterations of Quantum Diffusion...\n";

    for (int i = 0; i < grover_iterations; ++i) {
        // Oracle marks the goal and its causal precursors
        q_engine.apply_goal_oracle(state_goal);
        
        // Because of the causal unitary gate, the phase shift on the goal entangles with its precursor
        // Diffusion amplifies everything that was marked
        q_engine.apply_diffusion_operator();
    }

    std::cout << "\n[Attention] Collapsing the Wave Function...\n";
    int measured_state = q_engine.measure_optimal_state();

    std::cout << "\n-> Results:\n";
    std::cout << "   Highest Probability State: Concept " << measured_state << "\n";
    std::cout << "   Measured Probability: " << std::fixed << std::setprecision(4) << q_engine.get_probability(measured_state) << "\n\n";

    if (measured_state == state_goal) {
        std::cout << "[Success] The Quantum-Symbolic simulation successfully amplified the goal state out of 1,024 possibilities!\n";
        std::cout << "  By entangling causal rules as Unitary Matrices, the physical universe itself executes NAL Deduction.\n";
        std::cout << "  Welcome to the 100-Year Frontier.\n";
    }

    return 0;
}
