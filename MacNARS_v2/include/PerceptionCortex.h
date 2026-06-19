#ifndef MACNARS_PERCEPTION_CORTEX_H
#define MACNARS_PERCEPTION_CORTEX_H

#include <vector>
#include <cmath>
#include <string>
#include <iostream>

// Unsupervised Concept Generator
// Discretizes continuous multi-dimensional signals into discrete Latent Concepts
class PerceptionCortex {
public:
    struct LatentState {
        int id;
        std::vector<float> centroid;
        int hit_count;
    };

    std::vector<LatentState> latent_pool;
    float similarity_threshold; // Radius for activation
    int next_id;

    PerceptionCortex(float threshold = 0.1f) 
        : similarity_threshold(threshold), next_id(0) {}

    // Maps a continuous observation vector to a discrete ID
    int discretize(const std::vector<float>& obs) {
        if (latent_pool.empty()) {
            return create_new_state(obs);
        }

        int best_id = -1;
        float min_dist = 1e9f;

        for (auto& state : latent_pool) {
            float dist = euclidean_distance(obs, state.centroid);
            if (dist < min_dist) {
                min_dist = dist;
                best_id = state.id;
            }
        }

        // If the best match is too far, it's a novel concept
        if (min_dist > similarity_threshold) {
            return create_new_state(obs);
        }

        // Online updating of the centroid (Online K-Means style)
        update_centroid(best_id, obs);
        return best_id;
    }

private:
    float euclidean_distance(const std::vector<float>& a, const std::vector<float>& b) {
        float sum = 0.0f;
        for (size_t i = 0; i < a.size(); ++i) {
            float diff = a[i] - b[i];
            sum += diff * diff;
        }
        return std::sqrt(sum);
    }

    int create_new_state(const std::vector<float>& obs) {
        int id = next_id++;
        latent_pool.push_back({id, obs, 1});
        // std::cout << "[Perception] Created New Latent State: " << id << std::endl;
        return id;
    }

    void update_centroid(int id, const std::vector<float>& obs) {
        for (auto& state : latent_pool) {
            if (state.id == id) {
                state.hit_count++;
                float alpha = 1.0f / (float)state.hit_count;
                for (size_t i = 0; i < obs.size(); ++i) {
                    state.centroid[i] = (1.0f - alpha) * state.centroid[i] + alpha * obs[i];
                }
                break;
            }
        }
    }
};

#endif
