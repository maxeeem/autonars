#ifndef MACNARS_TRUTH_H
#define MACNARS_TRUTH_H

struct Truth {
    float frequency;
    float confidence;

    Truth() : frequency(0.0f), confidence(0.0f) {}
    Truth(float f, float c) : frequency(f), confidence(c) {}

    float Expectation() const {
        return confidence * (frequency - 0.5f) + 0.5f;
    }
};

#endif
