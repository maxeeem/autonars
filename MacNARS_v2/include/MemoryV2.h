#ifndef MACNARS_MEMORY_V2_H
#define MACNARS_MEMORY_V2_H

#include "Config.h"
#include "ConceptV2.h"
#include "RuleEngine.h"
#include <unordered_map>
#include <vector>
#include <iostream>

class MemoryV2 {
public:
    Bag<ConceptV2, Term> concept_bag;
    Bag<Task, std::string> task_bag;

    MemoryV2() : concept_bag(100, 1000, 10), task_bag(100, 1000, 10) {}

    void add_task(const Task& task) {
        // 1. Get or create concept
        std::shared_ptr<ConceptV2> concept = concept_bag.get(task.sentence.term);
        if (!concept) {
            concept = std::make_shared<ConceptV2>(task.sentence.term, 0);
            concept_bag.put_in(concept, task.sentence.term, 0.9f);
        }

        // 2. Add to concept's task bag
        concept->task_bag.put_in(std::make_shared<Task>(task), std::to_string(rand()), task.budget.priority);

        // 3. Add to general task bag
        task_bag.put_in(std::make_shared<Task>(task), std::to_string(rand()), task.budget.priority);
    }

    void cycle() {
        // 1. Select Task
        auto task = task_bag.take_out();
        if (!task) return;

        // 2. Select Concept related to Task
        auto concept = concept_bag.get(task->sentence.term);
        if (!concept) return;

        // 3. Select Belief from Concept
        auto belief = concept->belief_bag.take_out();
        
        // 4. Inference
        if (belief) {
            std::vector<Task> results = RuleEngine::run(*task, *belief);
            for (auto& r : results) {
                std::cout << "Derived: " << r.sentence.term.atoms[1] << " --> " << r.sentence.term.atoms[2] << std::endl;
                add_task(r);
            }
            // Put belief back
            concept->belief_bag.put_in(belief, std::to_string(rand()), 0.9f);
        } else {
            // If no belief, the task itself becomes a belief for the concept
            if (task->sentence.punctuation == '.') {
                concept->belief_bag.put_in(std::make_shared<Sentence>(task->sentence), std::to_string(rand()), 0.9f);
            }
        }
    }
};

#endif
