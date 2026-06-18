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

// Reserved Atom IDs for Copulas and Operators
constexpr Atom ATOM_NULL = 0;
constexpr Atom COPULA_INHERITANCE = 1;      // -->
constexpr Atom COPULA_SIMILARITY = 2;        // <->
constexpr Atom COPULA_INSTANCE = 3;          // {--
constexpr Atom COPULA_PROPERTY = 4;          // --]
constexpr Atom COPULA_INSTANCE_PROPERTY = 5; // {-]
constexpr Atom COPULA_IMPLICATION = 6;       // ==>
constexpr Atom COPULA_EQUIVALENCE = 7;       // <=>

constexpr Atom OPERATOR_CONJUNCTION = 8;     // (&&, ...)
constexpr Atom OPERATOR_DISJUNCTION = 9;     // (||, ...)
constexpr Atom OPERATOR_NEGATION = 10;       // --
constexpr Atom OPERATOR_PRODUCT = 11;        // (*, ...)
constexpr Atom OPERATOR_EXT_IMAGE = 12;      // (/, ...)
constexpr Atom OPERATOR_INT_IMAGE = 13;      // (\, ...)

constexpr Atom SET_EXT_OPEN = 14;            // {
constexpr Atom SET_EXT_CLOSE = 15;           // }
constexpr Atom SET_INT_OPEN = 16;            // [
constexpr Atom SET_INT_CLOSE = 17;           // ]

#endif
