#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <random>
#include <string>
#include "UnifiedMemory.h"

namespace py = pybind11;

// MacNARS v4 Agent: Integrates UnifiedMemory (Semantic Gravity) & NAL-8 Procedural Rules
class V4AtariAgent {
public:
    int cortex_cycles;
    int tick_counter;

    UnifiedMemory memory;
    int last_state_id;
    int last_op;

    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;

    // Track state values explicitly for the bridging mechanism (since we lack full NAL goal-derivation graph search in this PoC)
    std::unordered_map<int, Truth> state_values;

    V4AtariAgent() : memory(), rng(42), dist(0.0f, 1.0f) {
        cortex_cycles = 0;
        tick_counter = 0;
        last_state_id = -1;
        last_op = -1;
        memory.similarity_threshold = 0.90; // High clustering threshold
    }

    int step(py::array_t<float> obs, float reward) {
        auto buf = obs.unchecked<1>();
        std::vector<double> obs_vec;
        for (int i = 0; i < buf.shape(0); i++) {
            obs_vec.push_back(buf(i));
        }
        
        // 1. Ingest continuous state directly into Unified Concept Bag via Semantic Gravity
        Term current_state = memory.ingest_state(obs_vec);
        int current_state_id = current_state.atoms[0];

        // Track Reward Value
        if (state_values.find(current_state_id) == state_values.end()) {
            state_values[current_state_id] = Truth(0.5f, 0.01f);
        }
        update_truth(state_values[current_state_id], reward > 0.0f ? 1.0f : 0.0f);

        // 2. NAL-8 Temporal Induction
        if (last_state_id != -1 && last_op != -1) {
            Term seq_term;
            seq_term.atoms[0] = OPERATOR_SEQ_CONJUNCTION;
            seq_term.atoms[1] = last_state_id;
            seq_term.atoms[2] = last_op;
            
            Task t_seq = {{seq_term, Truth(1.0f, 0.9f), {}, '.'}, {1.0f, 1.0f, 1.0f}};
            Task t_res = {{current_state, Truth(1.0f, 0.9f), {}, '.'}, {1.0f, 1.0f, 1.0f}};

            std::vector<Task> induced = UnifiedRuleEngine::run(t_seq, t_res.sentence);
            for (const auto& task : induced) {
                memory.add_task(task);
            }
        }

        tick_counter++;
        
        // 3. Action Selection (Predictive Deduction)
        int best_op = 0;
        float best_utility = -1.0f;

        // Scan memory for applicable NAL-8 predictive rules
        for (const auto& task_pair : memory.task_bag.get_items()) {
            const Sentence& s = task_pair.second->sentence;
            if (s.term.atoms[0] == COPULA_PREDICTIVE_IMPLICATION && s.term.atoms[1] == current_state_id) {
                int op = s.term.atoms[2];
                int post_id = s.term.atoms[3];
                
                if (state_values.find(post_id) != state_values.end()) {
                    float utility = s.truth.Expectation() * state_values[post_id].Expectation();
                    if (utility > best_utility) {
                        best_utility = utility;
                        best_op = op;
                    }
                }
            }
        }

        // Epsilon-Greedy exploration
        if (best_utility < 0.55f || dist(rng) < 0.15f) {
            best_op = (int)(dist(rng) * 2.0f); // 0 or 1 for Left/Right
        }

        last_state_id = current_state_id;
        last_op = best_op;
        cortex_cycles++;
        
        return best_op;
    }

    int get_cortex_cycles() { return cortex_cycles; }

private:
    void update_truth(Truth& t, float f_ev) {
        float w = t.confidence / (1.0f - t.confidence);
        w += 1.0f;
        t.frequency = ((t.frequency * (w - 1.0f)) + f_ev) / w;
        t.confidence = w / (w + 1.0f);
    }
};

PYBIND11_MODULE(macnars_v4, m) {
    py::class_<V4AtariAgent>(m, "V4AtariAgent")
        .def(py::init<>())
        .def("step", &V4AtariAgent::step)
        .def("get_cortex_cycles", &V4AtariAgent::get_cortex_cycles);
}
