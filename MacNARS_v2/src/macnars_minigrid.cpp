#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <cmath>
#include <iostream>
#include <map>
#include <random>
#include <string>

namespace py = pybind11;

// NAL Truth Value (Shared logic)
struct Truth {
    float frequency;
    float confidence;

    Truth() : frequency(0.5f), confidence(0.01f) {}
    Truth(float f, float c) : frequency(f), confidence(c) {}

    float expectation() const {
        return confidence * (frequency - 0.5f) + 0.5f;
    }
};

// MiniGrid NAL-8 Procedural Cortex
class MiniGridCortex {
public:
    std::map<std::string, Truth> hypotheses;
    int last_state;
    int last_op;
    bool has_history;
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;
    int inferences;

    MiniGridCortex() : last_state(-1), last_op(-1), has_history(false), rng(1337), dist(0.0f, 1.0f), inferences(0) {}

    // State Discretization for MiniGrid-Empty-6x6
    // We get a 7x7x3 image from the agent's POV.
    // Index [3][5] is directly in front. [3][4] is one step further.
    // [0]=Object Type (1=Empty, 2=Wall, 8=Goal)
    int perceive(py::array_t<int> obs_image) {
        auto buf = obs_image.unchecked<3>();
        int obj_front = buf(3, 5, 0); 
        
        if (obj_front == 8) return 2; // Goal in front
        if (obj_front == 2) return 1; // Wall in front
        return 0;                     // Empty/Clear path
    }

    std::string make_key(int pre, int op, int post) {
        return std::to_string(pre) + "_" + std::to_string(op) + "_" + std::to_string(post);
    }

    void learn(int current_state) {
        if (!has_history) return;

        // Learn transition: <(last_state &/ last_op) =/> current_state>
        std::string actual_key = make_key(last_state, last_op, current_state);
        update_truth(actual_key, 1.0f); 

        // Negative evidence for states we didn't end up in
        for (int other_post = 0; other_post <= 2; ++other_post) {
            if (other_post != current_state) {
                std::string alt_key = make_key(last_state, last_op, other_post);
                update_truth(alt_key, 0.0f);
            }
        }
    }

    void update_truth(const std::string& key, float f_ev) {
        if (hypotheses.find(key) == hypotheses.end()) {
            hypotheses[key] = Truth(0.5f, 0.01f);
        }
        
        Truth& t = hypotheses[key];
        float w = t.confidence / (1.0f - t.confidence);
        w += 1.0f; // Evidence addition
        t.frequency = ((t.frequency * (w - 1.0f)) + f_ev) / w;
        t.confidence = w / (w + 1.0f);
        t.confidence *= 0.95f; // Decay for plasticity
    }

    // MiniGrid Actions: 0=Left, 1=Right, 2=Forward
    int decide(int current_state, int goal_state) {
        int best_op = -1; 
        float best_exp = 0.5f;

        // Search: <(current_state &/ ?op) =/> goal_state>
        for (int op = 0; op <= 2; ++op) {
            std::string key = make_key(current_state, op, goal_state);
            if (hypotheses.find(key) != hypotheses.end()) {
                float exp = hypotheses[key].expectation();
                if (exp > best_exp) {
                    best_exp = exp;
                    best_op = op;
                }
            }
        }

        // Active Exploration (Motor Babbling)
        if (best_exp <= 0.51f || dist(rng) < 0.2f) {
            best_op = (int)(dist(rng) * 3.0f); // Random 0, 1, or 2
        }

        // Fallback constraint (If stuck facing a wall, prioritize turning)
        if (current_state == 1 && best_op == 2) {
             best_op = (dist(rng) > 0.5f) ? 0 : 1; 
        }

        last_state = current_state;
        last_op = best_op;
        has_history = true;
        inferences++;

        return best_op;
    }
};

class MiniGridAgent {
public:
    MiniGridCortex cortex;

    MiniGridAgent() {}

    int step(py::dict obs_dict) {
        py::array_t<int> obs_image = obs_dict["image"].cast<py::array_t<int>>();
        
        int current_state = cortex.perceive(obs_image);
        cortex.learn(current_state);

        // Goal is state 2 (Goal in front)
        // If not in goal, we want an operation that leads to 2. 
        // If no op directly leads to 2, we want an op that leads to 0 (Clear Path) so we can explore.
        int action = -1;
        
        // 1. Try to directly reach goal state
        action = cortex.decide(current_state, 2); 
        
        // 2. If we don't know how to reach goal, try to reach clear path (exploration heuristic)
        // (In full NARS, this chaining is handled by backward inference. Here we simplify the heuristic)
        if (cortex.last_op == -1 || cortex.hypotheses[cortex.make_key(current_state, cortex.last_op, 2)].expectation() <= 0.51f) {
            action = cortex.decide(current_state, 0); 
        }

        return action;
    }

    int get_cortex_cycles() {
        return cortex.inferences;
    }
};

PYBIND11_MODULE(macnars_minigrid, m) {
    py::class_<MiniGridAgent>(m, "MiniGridAgent")
        .def(py::init<>())
        .def("step", &MiniGridAgent::step)
        .def("get_cortex_cycles", &MiniGridAgent::get_cortex_cycles);
}
