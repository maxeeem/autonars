#ifndef MACNARS_NAL_TRUTH_H
#define MACNARS_NAL_TRUTH_H

#include "Truth.h"
#include <cmath>
#include <algorithm>

namespace NAL {

// Auxiliary functions defined in NAL theory
inline float w_to_c(float w) { return w / (w + 1.0f); }
inline float c_to_w(float c) { return c / (1.0f - c); }

inline float AND(float a, float b) { return a * b; }
inline float OR(float a, float b) { return a + b - a * b; }
inline float NOT(float a) { return 1.0f - a; }

// Core Truth Functions
inline Truth Deduction(Truth t1, Truth t2) {
    return Truth(AND(t1.frequency, t2.frequency), 
                 AND(AND(t1.frequency, t2.frequency), AND(t1.confidence, t2.confidence)));
}

inline Truth Abduction(Truth t1, Truth t2) {
    float w = AND(t2.frequency, AND(t1.confidence, t2.confidence));
    return Truth(t1.frequency, w_to_c(w));
}

inline Truth Induction(Truth t1, Truth t2) {
    return Abduction(t2, t1);
}

inline Truth Exemplification(Truth t1, Truth t2) {
    float w = AND(AND(t1.frequency, t2.frequency), AND(t1.confidence, t2.confidence));
    return Truth(1.0f, w_to_c(w));
}

inline Truth Comparison(Truth t1, Truth t2) {
    float f1 = t1.frequency;
    float f2 = t2.frequency;
    float f = (f1 == 0 && f2 == 0) ? 0.5f : (f1 * f2) / (f1 + f2 - f1 * f2);
    float w = AND(OR(f1, f2), AND(t1.confidence, t2.confidence));
    return Truth(f, w_to_c(w));
}

inline Truth Analogy(Truth t1, Truth t2) {
    return Truth(AND(t1.frequency, t2.frequency), 
                 AND(t1.confidence, AND(t2.frequency, t2.confidence)));
}

inline Truth Resemblance(Truth t1, Truth t2) {
    return Truth(AND(t1.frequency, t2.frequency), 
                 AND(t1.confidence, AND(t2.confidence, OR(t1.frequency, t2.frequency))));
}

// Revision: {<S --> P>, <S --> P>} |- <S --> P>
inline Truth Revision(Truth t1, Truth t2) {
    float w1 = c_to_w(t1.confidence);
    float w2 = c_to_w(t2.confidence);
    float w = w1 + w2;
    float f = (w1 * t1.frequency + w2 * t2.frequency) / w;
    return Truth(f, std::min(0.99f, w_to_c(w)));
}

} // namespace NAL

#endif
