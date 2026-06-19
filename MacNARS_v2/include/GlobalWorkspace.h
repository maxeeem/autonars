#ifndef MACNARS_GLOBAL_WORKSPACE_H
#define MACNARS_GLOBAL_WORKSPACE_H

#include "MetalBagEngine.h"
#include "Truth.h"
#include <vector>
#include <mutex>
#include <unordered_map>

// MacNARS v10: Global Workspace for Cortical Swarm
// Implements Baars' Global Workspace Theory natively on the GPU memory structure.

struct BroadcastMessage {
    MetalConceptEmbedding semantic_payload;
    Truth truth_value;
    int source_column_id;
    int timestamp;
};

class GlobalWorkspace {
private:
    std::vector<BroadcastMessage> messages;
    std::mutex rw_mutex;
    int clock_tick = 0;

public:
    GlobalWorkspace() {
        messages.reserve(10000);
    }

    // A Cortical Column broadcasts a highly salient concept to the entire swarm
    void broadcast(int column_id, const MetalConceptEmbedding& payload, Truth t) {
        std::lock_guard<std::mutex> lock(rw_mutex);
        messages.push_back({payload, t, column_id, clock_tick});
    }

    // Other Columns read the global workspace to ingest cross-modal insights
    std::vector<BroadcastMessage> read_recent(int since_tick) {
        std::lock_guard<std::mutex> lock(rw_mutex);
        std::vector<BroadcastMessage> recent;
        for (auto it = messages.rbegin(); it != messages.rend(); ++it) {
            if (it->timestamp > since_tick) {
                recent.push_back(*it);
            } else {
                break;
            }
        }
        return recent;
    }

    void tick() {
        std::lock_guard<std::mutex> lock(rw_mutex);
        clock_tick++;
    }

    int get_tick() const {
        return clock_tick;
    }
};

#endif
