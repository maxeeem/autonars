#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <map>
#include <random>
#include <string>

namespace py = pybind11;

// NAL Truth Value
struct Truth {
    float frequency;
    float confidence;

    Truth() : frequency(0.5f), confidence(0.01f) {}
    Truth(float f, float c) : frequency(f), confidence(c) {}

    float expectation() const {
        return confidence * (frequency - 0.5f) + 0.5f;
    }
};

// NAL-8 Procedural Cortex
// Learns implications: < (Precondition &/ Operation) =/> Postcondition >
class ProceduralCortex {
public:
    // Implication Key: "Precond_Op_Postcond"
    std::map<std::string, Truth> hypotheses;
    
    int last_state;
    int last_op;
    bool has_history;

    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;

    ProceduralCortex() : last_state(-1), last_op(-1), has_history(false), rng(42), dist(0.0f, 1.0f) {}

    // Discretize state for symbolic reasoning
    int perceive(float angle, float pos) {
        float abs_angle = std::abs(angle);
        if (abs_angle < 0.02f) return 0; // Stable
        if (abs_angle < 0.08f) return 1; // Warning
        return 2; // Critical
    }

    std::string make_key(int pre, int op, int post) {
        return std::to_string(pre) + "_" + std::to_string(op) + "_" + std::to_string(post);
    }

    // NAL-8 Temporal Induction
    void learn(int current_state) {
        if (!has_history) return;

        // We executed last_op in last_state, and arrived at current_state.
        // Update Truth of: < (last_state &/ last_op) =/> current_state >
        
        // Positive evidence for the actual transition
        std::string actual_key = make_key(last_state, last_op, current_state);
        update_truth(actual_key, 1.0f); // Frequency 1.0 (It happened)

        // Negative evidence for other possible post-conditions
        for (int other_post = 0; other_post <= 2; ++other_post) {
            if (other_post != current_state) {
                std::string alt_key = make_key(last_state, last_op, other_post);
                update_truth(alt_key, 0.0f); // Frequency 0.0 (It didn't happen)
            }
        }
    }

    void update_truth(const std::string& key, float f_ev) {
        if (hypotheses.find(key) == hypotheses.end()) {
            hypotheses[key] = Truth(0.5f, 0.01f);
        }
        
        Truth& t = hypotheses[key];
        
        // NARS Revision (simplified Evidence addition)
        // w = c / (1 - c). Evidential weight.
        float w = t.confidence / (1.0f - t.confidence);
        w += 1.0f; // Add 1 unit of evidence
        
        // New frequency is weighted average
        t.frequency = ((t.frequency * (w - 1.0f)) + f_ev) / w;
        
        // New confidence is w / (w + 1)
        t.confidence = w / (w + 1.0f);
        
        // Decay to prevent total rigidity
        t.confidence *= 0.99f;
    }

    // NAL-8 Decision Making
    int decide(int current_state, int goal_state) {
        int best_op = 0; // 0 = Do nothing
        float best_exp = 0.5f;

        // Search hypotheses: < (current_state &/ ?op) =/> goal_state >
        // We have 5 operations: 0(Wait), 1(IncP), 2(DecP), 3(IncD), 4(DecD)
        for (int op = 0; op <= 4; ++op) {
            std::string key = make_key(current_state, op, goal_state);
            if (hypotheses.find(key) != hypotheses.end()) {
                float exp = hypotheses[key].expectation();
                if (exp > best_exp) {
                    best_exp = exp;
                    best_op = op;
                }
            }
        }

        // Motor Babbling (Exploration)
        // If we have no strong hypothesis, or purely randomly 10% of the time, explore.
        if (best_exp <= 0.55f || dist(rng) < 0.1f) {
            best_op = (int)(dist(rng) * 5.0f);
        }

        last_state = current_state;
        last_op = best_op;
        has_history = true;

        return best_op;
    }
};

class DualProcessAgent {
public:
    float target_pos;
    float target_angle;
    
    float Kp_angle;
    float Kd_angle;
    float Ki_angle;
    
    float int_angle;
    float last_angle;
    
    int cortex_cycles;
    int tick_counter;

    ProceduralCortex cortex;

    DualProcessAgent() {
        target_pos = 0.0f;
        target_angle = 0.0f;
        
        // Start with terrible, uncalibrated parameters. The Cortex MUST learn to fix this.
        Kp_angle = 1.0f; 
        Kd_angle = 0.0f;
        Ki_angle = 0.0f;
        
        int_angle = 0.0f;
        last_angle = 0.0f;
        
        cortex_cycles = 0;
        tick_counter = 0;
    }

    // Input state from CartPole: [pos, vel, angle, ang_vel]
    int step(py::array_t<float> obs) {
        auto buf = obs.unchecked<1>();
        float pos = buf(0);
        float vel = buf(1);
        float angle = buf(2);
        float ang_vel = buf(3);

        target_angle = (pos * 0.05f) + (vel * 0.05f);

        // 1. NARS Cortex (Symbolic Procedural Learning) - Runs at 10Hz (every 100 ticks assuming Gym runs at some fast dt, wait CartPole is 50Hz. Let's run cortex every 5 ticks).
        tick_counter++;
        if (tick_counter % 5 == 0) {
            int current_state = cortex.perceive(angle, pos);
            
            // Learn from the last action's consequence
            cortex.learn(current_state);

            // If we are not in the Goal State (Stable = 0), we must decide on an operation.
            if (current_state != 0) {
                int op = cortex.decide(current_state, 0); // 0 is Goal (Stable)
                
                // Execute Operation
                if (op == 1) Kp_angle += 5.0f;
                else if (op == 2) Kp_angle = std::max(0.0f, Kp_angle - 5.0f);
                else if (op == 3) Kd_angle += 1.0f;
                else if (op == 4) Kd_angle = std::max(0.0f, Kd_angle - 1.0f);
                
                if (op != 0) cortex_cycles++;
            } else {
                // If stable, we still record history so we learn what keeps us stable
                cortex.last_state = current_state;
                cortex.last_op = 0; // Wait
                cortex.has_history = true;
            }
        }

        // 2. Continuous Reflex (Spinal PID)
        float err_angle = target_angle - angle;
        
        // We calculate continuous force needed to correct angle
        float force = (Kp_angle * err_angle) - (Kd_angle * ang_vel);
        
        // Bang-bang mapping for Gym CartPole
        return force < 0.0f ? 1 : 0;
    }

    int get_cortex_cycles() {
        return cortex_cycles;
    }
};

PYBIND11_MODULE(macnars, m) {
    py::class_<DualProcessAgent>(m, "DualProcessAgent")
        .def(py::init<>())
        .def("step", &DualProcessAgent::step)
        .def("get_cortex_cycles", &DualProcessAgent::get_cortex_cycles);
}
