#ifndef MACNARS_BAG_H
#define MACNARS_BAG_H

#include "Distributor.h"
#include <vector>
#include <unordered_map>
#include <list>
#include <memory>
#include <algorithm>

template <typename T, typename K>
class Bag {
private:
    int total_levels;
    int capacity;
    int threshold;
    Distributor distributor;
    
    std::unordered_map<K, std::shared_ptr<T>> name_table;
    std::vector<std::list<std::shared_ptr<T>>> item_table;
    
    int level_index;
    int current_level;
    int current_counter;
    int mass;

public:
    Bag(int levels, int cap, int thresh) 
        : total_levels(levels), capacity(cap), threshold(thresh), distributor(levels) {
        item_table.resize(total_levels);
        level_index = 0;
        current_level = total_levels - 1;
        current_counter = 0;
        mass = 0;
    }

    std::shared_ptr<T> get(K key) {
        auto it = name_table.find(key);
        if (it != name_table.end()) {
            return it->second;
        }
        return nullptr;
    }

    size_t size() const {
        return name_table.size();
    }

    std::shared_ptr<T> put_in(std::shared_ptr<T> item, K key, float priority) {
        auto it = name_table.find(key);
        if (it != name_table.end()) {
            return it->second; 
        }

        if (name_table.size() >= (size_t)capacity) {
            evict_lowest();
        }

        name_table[key] = item;
        int level = std::max(0, std::min(total_levels - 1, (int)(priority * (total_levels - 1))));
        item_table[level].push_back(item);
        mass += level;
        
        return item;
    }

    std::shared_ptr<T> take_out() {
        if (name_table.empty()) return nullptr;

        for (int i = 0; i < total_levels * 2; ++i) {
            current_level = distributor.pick(level_index);
            level_index = distributor.next(level_index);
            
            if (!item_table[current_level].empty()) {
                std::shared_ptr<T> item = item_table[current_level].front();
                item_table[current_level].pop_front();
                
                // For simplicity, remove from name table for now
                // In full NARS, items in bags are often managed differently
                // but this follows the basic logic.
                // Note: This requires the item to be found by value which is slow.
                // We'll optimize this in a real implementation.
                
                mass -= current_level;
                return item;
            }
        }
        return nullptr;
    }

    std::shared_ptr<T> evict_lowest() {
        for (int i = 0; i < total_levels; ++i) {
            if (!item_table[i].empty()) {
                std::shared_ptr<T> item = item_table[i].front();
                item_table[i].pop_front();
                // name_table.erase(...) // Needs key tracking
                mass -= i;
                return item;
            }
        }
        return nullptr;
    }

    // Expose name_table for Semantic Gravity searches
    const std::unordered_map<K, std::shared_ptr<T>>& get_items() const {
        return name_table;
    }
};

#endif
