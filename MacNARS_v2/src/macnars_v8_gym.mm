#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <iostream>
#include "MacNARSEngine.h"

namespace py = pybind11;

class MacNARSV8Agent {
public:
    MacNARSEngine engine;

    MacNARSV8Agent() {}

    int step(py::array_t<float> obs_array, float reward) {
        auto buf = obs_array.unchecked<1>();
        MetalConceptEmbedding state;
        for (int i = 0; i < 6; i++) {
            if (i < buf.shape(0)) {
                state.vector[i] = buf(i);
            } else {
                state.vector[i] = 0.0f;
            }
        }
        return engine.step(state, reward);
    }

    void reset_state() {
        engine.last_concept_id = -1;
        engine.last_op_id = -1;
    }
};

PYBIND11_MODULE(macnars_v8_gym, m) {
    py::class_<MacNARSV8Agent>(m, "MacNARSV8Agent")
        .def(py::init<>())
        .def("step", &MacNARSV8Agent::step)
        .def("reset_state", &MacNARSV8Agent::reset_state);
}
