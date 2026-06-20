# NARS Apple Silicon Implementation Plan

## 1. Objective
Implement a "True NARS" (Non-Axiomatic Reasoning System) that leverages the unique hardware architecture of Apple Silicon (Unified Memory, Metal, Grand Central Dispatch / Swift Concurrency) while maintaining strict adherence to NARS theory and principles by Pei Wang.

## 2. Baselines and Compatibility
- **Gold Standard:** OpenNARS (Java)
- **Performance Standard:** OpenNARS for Applications (ONA, C)
- **Experiment Standard:** PyNARS (OpenNARS-4, Python) for MNIST and ONA for Robot Control tasks.
- **Goal:** The implementation must interface with the standard Narsese syntax and match or exceed the test cases/experiments of these systems.

## 3. Architecture for Apple Silicon
NARS is heavily symbolic (term unification, priority queues, concept graphs), which traditionally limits GPU acceleration. However, Apple Silicon's **Unified Memory** eliminates the CPU-GPU transfer bottleneck, allowing novel architectures:

### Data Structures (Entity-Component System approach)
To leverage the GPU and distributed processing, we must avoid scattered pointer graphs.
- **Flat Concept Pools:** Store Concepts, Tasks, and Beliefs in contiguous memory arrays (SoA - Structure of Arrays).
- **Index-based linking:** Use 32-bit/64-bit integer indices instead of pointers to link terms and concepts.

### Parallel Processing pipeline (CPU + GPU)
1. **Bag Selection (CPU/GCD):** The CPU uses Grand Central Dispatch (GCD) to efficiently manage the priority queues (Bags) and select the active concepts and tasks.
2. **Inference Batching (CPU):** CPU bundles selected tasks and concepts into a flat memory buffer.
3. **Parallel Unification & Syllogism (Metal/GPU):** Because of unified memory, the GPU can immediately read the batch buffer. Compute shaders perform variable unification and rule matching in parallel across thousands of pairs.
4. **Result Collection & Budget Updates (CPU):** CPU reads the generated derivations from unified memory, updates budgets, and inserts them back into the Bags.

### Technology Stack
- **Language:** **C++** (for core memory layout and GCD) and **Metal / Objective-C++** (for compute shaders and unified memory sharing). Alternatively, **Rust** or **Swift** (with Metal). To easily interface with PyNARS experiments, we will provide Python bindings using `pybind11` or `nanobind` (Apple's MLX uses nanobind).
- **We will use C++ with Metal Compute Shaders** to implement the core. It ensures high performance, compatibility with ONA's philosophy, and allows easy Python wrapping.

## 4. Implementation Phases

**Phase 1: Core Data Structures & Narsese Parser (CPU-bound)**
- Implement Narsese parser (compatible with standard syntax).
- Implement Term, Statement, and Concept structures in flat arrays.
- Implement the Bag (Priority Queue) with probabilistic selection.

**Phase 2: NAL Inference Engine (CPU -> Metal transition)**
- Implement Non-Axiomatic Logic (NAL) rules (NAL-1 to NAL-8).
- Implement local inference (matching tasks with beliefs).
- Offload batch rule matching and unification to Metal compute shaders using Apple's unified `MTLBuffer`s.

**Phase 3: Integration & Testing**
- Run OpenNARS Java baseline tests using our Narsese parser.
- Run ONA system tests (evaluation.py).
- Reproduce PyNARS MNIST or ONA Pong/Alien reinforcement tasks.

**Phase 4: Optimization**
- Fine-tune Grand Central Dispatch threading and Metal threadgroups.
- Document and provide justifications for any deviations in test cases.
