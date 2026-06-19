#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <map>
#include <random>
#include <string>
#include "PerceptionCortex.h"

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
class ProceduralCortex {
public:
    std::map<std::string, Truth> hypotheses;
    std::map<int, Truth> state_values; // Maps state_id to its observed reward value
    
    int last_state;
    int last_op;
    bool has_history;

    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;

    ProceduralCortex() : last_state(-1), last_op(-1), has_history(false), rng(42), dist(0.0f, 1.0f) {}

    std::string make_key(int pre, int op, int post) {
        return std::to_string(pre) + "_" + std::to_string(op) + "_" + std::to_string(post);
    }

    // NAL-9 Mental Operations
    void mental_op_doubt(const std::string& key) {
        if (hypotheses.find(key) != hypotheses.end()) {
            // Rapidly slash confidence (Introspective pruning)
            hypotheses[key].confidence *= 0.1f; 
        }
    }

    void mental_op_believe(const std::string& key) {
        if (hypotheses.find(key) != hypotheses.end()) {
            // Boost confidence if prediction was correct
            hypotheses[key].confidence = std::min(0.99f, hypotheses[key].confidence * 1.1f);
        }
    }

    void learn(int current_state, float reward) {
        if (state_values.find(current_state) == state_values.end()) {
            state_values[current_state] = Truth(0.5f, 0.01f);
        }
        update_state_value(current_state, reward > 0.0f ? 1.0f : 0.0f);

        if (!has_history) return;

        // 1. Prediction Verification (Introspection)
        // Check if we arrived at the state we expected
        std::string expected_key = "";
        float max_exp = -1.0f;
        for (int post_id : observed_states) {
            std::string k = make_key(last_state, last_op, post_id);
            if (hypotheses.find(k) != hypotheses.end()) {
                if (hypotheses[k].expectation() > max_exp) {
                    max_exp = hypotheses[k].expectation();
                    expected_key = k;
                }
            }
        }

        std::string actual_key = make_key(last_state, last_op, current_state);
        
        if (!expected_key.empty() && expected_key != actual_key && max_exp > 0.6f) {
            // NAL-9 Introspection: "I was wrong. I expected a different outcome."
            mental_op_doubt(expected_key);
        } else if (expected_key == actual_key) {
            // NAL-9 Introspection: "My causal model is working."
            mental_op_believe(actual_key);
        }

        // 2. Standard Temporal Induction (Evidence collection)
        update_truth(actual_key, 1.0f);

        for (int other_post : observed_states) {
            if (other_post != current_state) {
                std::string alt_key = make_key(last_state, last_op, other_post);
                update_truth(alt_key, 0.0f);
            }
        }
    }

    void update_truth(const std::string& key, float f_ev) {
        Truth& t = hypotheses[key];
        float w = t.confidence / (1.0f - t.confidence);
        w += 1.0f;
        t.frequency = ((t.frequency * (w - 1.0f)) + f_ev) / w;
        t.confidence = w / (w + 1.0f);
        t.confidence *= 0.99f;
    }

    void update_state_value(int id, float f_ev) {
        Truth& t = state_values[id];
        float w = t.confidence / (1.0f - t.confidence);
        w += 1.0f;
        t.frequency = ((t.frequency * (w - 1.0f)) + f_ev) / w;
        t.confidence = w / (w + 1.0f);
    }

    std::vector<int> observed_states;

    int decide(int current_state) {
        int best_op = 0; 
        float best_utility = -1.0f;

        // Search: <(current_state &/ ?op) =/> ?post> where ?post has high state_value
        for (int op = 0; op <= 4; ++op) {
            for (auto const& [post_id, val_truth] : state_values) {
                std::string key = make_key(current_state, op, post_id);
                if (hypotheses.find(key) != hypotheses.end()) {
                    // Utility = P(Post | Pre, Op) * Value(Post)
                    float utility = hypotheses[key].expectation() * val_truth.expectation();
                    if (utility > best_utility) {
                        best_utility = utility;
                        best_op = op;
                    }
                }
            }
        }

        if (best_utility < 0.55f || dist(rng) < 0.15f) {
            best_op = (int)(dist(rng) * 5.0f);
        }

        last_state = current_state;
        last_op = best_op;
        has_history = true;
        return best_op;
    }
};

class AtariAgent {
public:
    int cortex_cycles;
    int tick_counter;

    ProceduralCortex cortex;
    PerceptionCortex perception;

    AtariAgent() : perception(0.25f) { // lower threshold for Atari since normalized 0-1
        cortex_cycles = 0;
        tick_counter = 0;
    }

    int step(py::array_t<float> obs, float reward) {
        auto buf = obs.unchecked<1>();
        std::vector<float> obs_vec;
        for (int i = 0; i < buf.shape(0); i++) {
            obs_vec.push_back(buf(i));
        }
        
        int current_state = perception.discretize(obs_vec);
        if (std::find(cortex.observed_states.begin(), cortex.observed_states.end(), current_state) == cortex.observed_states.end()) {
            cortex.observed_states.push_back(current_state);
        }

        tick_counter++;
        
        cortex.learn(current_state, reward);
        int op = cortex.decide(current_state);
        cortex_cycles++;
        
        return op % 2;
    }

    int get_cortex_cycles() { return cortex_cycles; }
};

PYBIND11_MODULE(macnars_atari, m) {
    py::class_<AtariAgent>(m, "AtariAgent")
        .def(py::init<>())
        .def("step", &AtariAgent::step)
        .def("get_cortex_cycles", &AtariAgent::get_cortex_cycles);
}
