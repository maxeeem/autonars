#ifndef MACNARS_DISTRIBUTOR_H
#define MACNARS_DISTRIBUTOR_H

#include <vector>

class Distributor {
public:
    std::vector<int> order;
    int capacity;

    Distributor(int range) {
        int index = 0;
        capacity = (range * (range + 1)) / 2;
        order.assign(capacity, -1);
        
        for (int rank = range; rank > 0; rank--) {
            for (int time = 0; time < rank; time++) {
                index = ((capacity / rank) + index) % capacity;
                while (order[index] >= 0) {
                    index = (index + 1) % capacity;
                }
                order[index] = rank - 1;
            }
        }
    }

    int pick(int index) const {
        return order[index];
    }

    int next(int index) const {
        return (index + 1) % capacity;
    }
};

#endif
