#ifndef MACNARS_RULE_ENGINE_H
#define MACNARS_RULE_ENGINE_H

#include "Sentence.h"
#include "NAL_Truth.h"
#include <vector>

class RuleEngine {
public:
    static std::vector<Task> run(const Task& task, const Sentence& belief) {
        std::vector<Task> results;
        
        const Sentence& s1 = task.sentence;
        const Sentence& s2 = belief;

        // 1. Revision
        if (s1.term == s2.term) {
            Sentence result_sentence = s1;
            result_sentence.truth = NAL::Revision(s1.truth, s2.truth);
            results.push_back({result_sentence, {0.8f, 0.8f, 0.8f}});
            return results;
        }

        // 2. Syllogisms
        // Extract components (assuming simple statements for now)
        Atom copula1 = s1.term.atoms[0];
        Atom sub1 = s1.term.atoms[1];
        Atom pre1 = s1.term.atoms[2];

        Atom copula2 = s2.term.atoms[0];
        Atom sub2 = s2.term.atoms[1];
        Atom pre2 = s2.term.atoms[2];

        if (copula1 == COPULA_INHERITANCE && copula2 == COPULA_INHERITANCE) {
            // Deduction: {M --> P, S --> M} |- S --> P
            if (sub1 == pre2) { // M == M
                Term result_term;
                result_term.atoms[0] = COPULA_INHERITANCE;
                result_term.atoms[1] = sub2; // S
                result_term.atoms[2] = pre1; // P
                results.push_back({{result_term, NAL::Deduction(s1.truth, s2.truth), {}, '.'}, {0.7f, 0.7f, 0.7f}});
            }
            else if (sub2 == pre1) { // M == M (swapped)
                Term result_term;
                result_term.atoms[0] = COPULA_INHERITANCE;
                result_term.atoms[1] = sub1; // S
                result_term.atoms[2] = pre2; // P
                results.push_back({{result_term, NAL::Deduction(s2.truth, s1.truth), {}, '.'}, {0.7f, 0.7f, 0.7f}});
            }
            
            // Induction: {M --> P, M --> S} |- S --> P
            if (sub1 == sub2) {
                Term result_term;
                result_term.atoms[0] = COPULA_INHERITANCE;
                result_term.atoms[1] = pre2; // S
                result_term.atoms[2] = pre1; // P
                results.push_back({{result_term, NAL::Induction(s1.truth, s2.truth), {}, '.'}, {0.6f, 0.6f, 0.6f}});
            }

            // Abduction: {P --> M, S --> M} |- S --> P
            if (pre1 == pre2) {
                Term result_term;
                result_term.atoms[0] = COPULA_INHERITANCE;
                result_term.atoms[1] = sub2; // S
                result_term.atoms[2] = sub1; // P
                results.push_back({{result_term, NAL::Abduction(s1.truth, s2.truth), {}, '.'}, {0.6f, 0.6f, 0.6f}});
            }
        }

        return results;
    }
};

#endif
