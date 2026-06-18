#ifndef MACNARS_CONFIG_H
#define MACNARS_CONFIG_H

#include <cstdint>

// Maximum elements in a Term tree (binary heap encoding)
// Depth 5 tree = 2^5 - 1 = 31. Let's use 31 to fit nicely, or 63.
constexpr int COMPOUND_TERM_SIZE_MAX = 31;

// Maximum concepts in memory
constexpr int CONCEPT_MAX = 16384;

// Budgets
constexpr float BUDGET_THRESHOLD = 0.01f;

// Atoms are represented by 32-bit integers
typedef uint32_t Atom;

// 0 is reserved for 'Empty/Null' atom
constexpr Atom ATOM_NULL = 0;

#endif
