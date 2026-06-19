#ifndef MACNARS_UNIFIED_RULE_ENGINE_H
#define MACNARS_UNIFIED_RULE_ENGINE_H

#include "Sentence.h"
#include "NAL_Truth.h"
#include <vector>

class UnifiedRuleEngine {
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

        // Extract components
        Atom copula1 = s1.term.atoms[0];
        Atom copula2 = s2.term.atoms[0];

        // 2. NAL-8 Temporal Induction: { S1, ^op, S2 } |- <(S1 &/ ^op) =/> S2>
        // In a true implementation, we need an Event Buffer to pair S1, ^op, and S2 sequentially.
        // For this unified engine, we will assume we're provided with a sequence Task
        // representing {S1 &/ ^op} and a subsequent event Task {S2}.
        // We will simplify this logic for the proof of concept to demonstrate the capability.

        if (copula1 == OPERATOR_SEQ_CONJUNCTION && s2.term.isAtomic()) {
            // Task is <S1 &/ ^op>, Belief is S2
            Term result_term;
            result_term.atoms[0] = COPULA_PREDICTIVE_IMPLICATION;
            
            // Sub is <S1 &/ ^op> (we embed it deeply, but here we'll just link the compound term)
            // To keep it simple, we use atoms[1] as the Operator, and atoms[2] as S1.
            result_term.atoms[1] = s1.term.atoms[1]; // S1
            result_term.atoms[2] = s1.term.atoms[2]; // ^op
            result_term.atoms[3] = s2.term.atoms[0]; // S2
            
            // Generate a weak inductive truth value
            Truth ind_truth = NAL::Induction(s1.truth, s2.truth);
            results.push_back({{result_term, ind_truth, {}, '.'}, {0.8f, 0.8f, 0.8f}});
        }

        // 3. NAL-8 Predictive Deduction (Action Selection)
        // If we have a goal S2! and a belief <(S1 &/ ^op) =/> S2>
        if (s1.punctuation == '!' && copula2 == COPULA_PREDICTIVE_IMPLICATION) {
            if (s1.term.atoms[0] == s2.term.atoms[3]) { // S2 == S2
                Term sub_term;
                sub_term.atoms[0] = OPERATOR_SEQ_CONJUNCTION;
                sub_term.atoms[1] = s2.term.atoms[1]; // S1
                sub_term.atoms[2] = s2.term.atoms[2]; // ^op
                
                Truth ded_truth = NAL::Deduction(s1.truth, s2.truth);
                results.push_back({{sub_term, ded_truth, {}, '!'}, {0.9f, 0.9f, 0.9f}});
            }
        }

        return results;
    }
};

#endif
