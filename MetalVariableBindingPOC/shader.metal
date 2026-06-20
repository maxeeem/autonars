#include <metal_stdlib>
using namespace metal;

struct Term {
    uint type; // 0: Atom, 1: Variable, 2: Compound
    uint id;
    uint arg1;
    uint arg2;
};

struct Binding {
    uint var_id;
    uint term_index;
};

struct MatchResult {
    uint success;
    uint num_bindings;
    Binding bindings[8];
};

struct StackFrame {
    uint t1;
    uint t2;
};

kernel void match_terms(
    device const Term* terms [[buffer(0)]],
    device const uint2* pairs [[buffer(1)]],
    device MatchResult* results [[buffer(2)]],
    uint id [[thread_position_in_grid]]
) {
    uint t1_idx = pairs[id].x;
    uint t2_idx = pairs[id].y;
    
    MatchResult res;
    res.success = 1;
    res.num_bindings = 0;
    
    // Iterative unification using a stack to solve pointer-indirection natively
    StackFrame stack[16];
    int sp = 0;
    
    stack[sp++] = {t1_idx, t2_idx};
    
    while (sp > 0) {
        StackFrame frame = stack[--sp];
        Term term1 = terms[frame.t1];
        Term term2 = terms[frame.t2];
        
        if (term1.type == 1) { // Variable
            // Bind term1.id to frame.t2
            bool already_bound = false;
            for (uint i = 0; i < res.num_bindings; i++) {
                if (res.bindings[i].var_id == term1.id) {
                    already_bound = true;
                    // For POC, simple index check. Deep equality would require another stack!
                    if (res.bindings[i].term_index != frame.t2) {
                        res.success = 0;
                        break;
                    }
                }
            }
            if (res.success == 0) break;
            
            if (!already_bound) {
                if (res.num_bindings < 8) {
                    res.bindings[res.num_bindings].var_id = term1.id;
                    res.bindings[res.num_bindings].term_index = frame.t2;
                    res.num_bindings++;
                } else {
                    res.success = 0; // Out of binding space
                    break;
                }
            }
        } else if (term2.type == 1) { // Variable
            bool already_bound = false;
            for (uint i = 0; i < res.num_bindings; i++) {
                if (res.bindings[i].var_id == term2.id) {
                    already_bound = true;
                    if (res.bindings[i].term_index != frame.t1) {
                        res.success = 0;
                        break;
                    }
                }
            }
            if (res.success == 0) break;
            
            if (!already_bound) {
                if (res.num_bindings < 8) {
                    res.bindings[res.num_bindings].var_id = term2.id;
                    res.bindings[res.num_bindings].term_index = frame.t1;
                    res.num_bindings++;
                } else {
                    res.success = 0; 
                    break;
                }
            }
        } else if (term1.type == 0 && term2.type == 0) { // Both Atoms
            if (term1.id != term2.id) {
                res.success = 0;
                break;
            }
        } else if (term1.type == 2 && term2.type == 2) { // Both Compounds
            if (term1.id != term2.id) { // Operation mismatch
                res.success = 0;
                break;
            }
            // Push args to stack
            if (term1.arg1 != 0xFFFFFFFF && term2.arg1 != 0xFFFFFFFF) {
                stack[sp++] = {term1.arg1, term2.arg1};
            } else if (term1.arg1 != term2.arg1) {
                res.success = 0;
                break;
            }
            
            if (term1.arg2 != 0xFFFFFFFF && term2.arg2 != 0xFFFFFFFF) {
                stack[sp++] = {term1.arg2, term2.arg2};
            } else if (term1.arg2 != term2.arg2) {
                res.success = 0;
                break;
            }
        } else {
            // Mismatch type (e.g. Atom and Compound)
            res.success = 0;
            break;
        }
    }
    
    results[id] = res;
}
