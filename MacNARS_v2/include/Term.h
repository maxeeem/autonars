#ifndef MACNARS_TERM_H
#define MACNARS_TERM_H

#include "Config.h"
#include <array>
#include <functional>

struct Term {
    std::array<Atom, COMPOUND_TERM_SIZE_MAX> atoms;

    Term() {
        atoms.fill(ATOM_NULL);
    }

    bool operator==(const Term& other) const {
        return atoms == other.atoms;
    }

    uint64_t hash() const {
        uint64_t h = 0;
        for (int i = 0; i < COMPOUND_TERM_SIZE_MAX; ++i) {
            h ^= atoms[i] + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
        }
        return h;
    }

    bool isAtomic() const {
        return atoms[1] == ATOM_NULL; // If the left child is null, it's atomic (assuming root at 0 and children at 2*i+1, 2*i+2)
    }
};

namespace std {
    template <>
    struct hash<Term> {
        size_t operator()(const Term& t) const {
            return t.hash();
        }
    };
}

#endif
