#ifndef MACNARS_SENTENCE_H
#define MACNARS_SENTENCE_H

#include "Term.h"
#include "Truth.h"
#include <vector>

struct Stamp {
    std::vector<uint32_t> occurrence_id; // Simple evidential base tracking
    long occurrence_time;
};

struct Sentence {
    Term term;
    Truth truth;
    Stamp stamp;
    char punctuation; // '.', '!', '?'
};

struct Budget {
    float priority;
    float durability;
    float quality;
};

struct Task {
    Sentence sentence;
    Budget budget;
};

#endif
