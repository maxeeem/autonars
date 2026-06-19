#ifndef MACNARS_CORTICAL_COLUMN_H
#define MACNARS_CORTICAL_COLUMN_H

#include "MacNARSEngine.h"
#include "GlobalWorkspace.h"
#include <thread>
#include <atomic>
#include <iostream>

// Represents a specialized NAL-8 Engine running asynchronously
class CorticalColumn {
public:
    int id;
    std::string specialization;
    MacNARSEngine engine;
    GlobalWorkspace* workspace;
    
    std::thread worker_thread;
    std::atomic<bool> running;
    int last_read_tick;

    CorticalColumn(int col_id, std::string spec, GlobalWorkspace* ws) 
        : id(col_id), specialization(spec), workspace(ws), running(false), last_read_tick(-1) {}

    ~CorticalColumn() {
        stop();
    }

    void start() {
        running = true;
        worker_thread = std::thread(&CorticalColumn::run_loop, this);
    }

    void stop() {
        running = false;
        if (worker_thread.joinable()) {
            worker_thread.join();
        }
    }

private:
    void run_loop() {
        while (running) {
            // 1. Read Global Workspace for cross-modal ideas
            auto recent_msgs = workspace->read_recent(last_read_tick);
            if (!recent_msgs.empty()) {
                last_read_tick = workspace->get_tick();
                for (const auto& msg : recent_msgs) {
                    if (msg.source_column_id != id) {
                        // Ingest the broadcasted vector into this column's local manifold
                        engine.ingest_perception(msg.semantic_payload);
                    }
                }
            }

            // 2. Perform local Deep Planning & Induction
            // We simulate a continuous environment stream. For the swarm, we just pick a random existing concept and plan.
            if (!engine.semantic_manifold.empty()) {
                int test_id = rand() % engine.semantic_manifold.size();
                int action = engine.decide_action(test_id);
                
                // If a high-utility action is found, broadcast the underlying concept as highly salient
                if (action != -1 && (rand() % 100) < 5) { // 5% chance to broadcast for POC
                    workspace->broadcast(id, engine.semantic_manifold[test_id], Truth(1.0f, 0.9f));
                }
            }
            
            // Sleep to yield CPU
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
};

#endif
