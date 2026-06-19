#ifndef MACNARS_UNIFIED_MEMORY_H
#define MACNARS_UNIFIED_MEMORY_H

#include "Config.h"
#include "UnifiedConcept.h"
#include "UnifiedRuleEngine.h"
#include <unordered_map>
#include <vector>
#include <iostream>
#include <memory>

class UnifiedMemory {
public:
    Bag<UnifiedConcept, Term> concept_bag;
    Bag<Task, std::string> task_bag;

    int next_state_atom_id = 1000; // Atoms from 0-999 are reserved (e.g. Copulas)
    double similarity_threshold = 0.95;

    UnifiedMemory() : concept_bag(1000, 5000, 10), task_bag(1000, 5000, 10) {}

    // Ingests a continuous latent vector. Natively clusters it via Semantic Gravity
    // and returns the discrete Term representing this state.
    Term ingest_state(const std::vector<double>& latent_vec) {
        std::shared_ptr<UnifiedConcept> best_concept = nullptr;
        double max_gravity = -1.0;

        for (const auto& pair : concept_bag.get_items()) {
            double gravity = pair.second->get_semantic_gravity(latent_vec);
            if (gravity > max_gravity) {
                max_gravity = gravity;
                best_concept = pair.second;
            }
        }

        if (max_gravity >= similarity_threshold && best_concept != nullptr) {
            return best_concept->term;
        }

        // Novel State
        Term new_term;
        new_term.atoms[0] = next_state_atom_id++;
        
        auto concept = std::make_shared<UnifiedConcept>(new_term, new_term.atoms[0], latent_vec);
        concept_bag.put_in(concept, new_term, 0.9f);
        return new_term;
    }

    void add_task(const Task& task) {
        auto concept = concept_bag.get(task.sentence.term);
        if (!concept) {
            // Cannot process task if we don't have the concept
            return;
        }

        concept->task_bag.put_in(std::make_shared<Task>(task), std::to_string(rand()), task.budget.priority);
        task_bag.put_in(std::make_shared<Task>(task), std::to_string(rand()), task.budget.priority);
    }

    void cycle() {
        auto task = task_bag.take_out();
        if (!task) return;

        auto concept = concept_bag.get(task->sentence.term);
        if (!concept) return;

        auto belief = concept->belief_bag.take_out();
        
        if (belief) {
            std::vector<Task> results = UnifiedRuleEngine::run(*task, *belief);
            for (auto& r : results) {
                add_task(r);
            }
            concept->belief_bag.put_in(belief, std::to_string(rand()), 0.9f);
        } else {
            if (task->sentence.punctuation == '.') {
                concept->belief_bag.put_in(std::make_shared<Sentence>(task->sentence), std::to_string(rand()), 0.9f);
            }
        }
    }
};

#endif
