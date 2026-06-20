# MacNARS v2: The Faithful & Accelerated Implementation Plan

## 1. Architectural Philosophy
MacNARS v2 rejects the pragmatic simplifications of ONA in favor of strict adherence to the **Non-Axiomatic Logic (NAL) Second Edition (2025)** and the **Bag-based Control Model**. It leverages Apple Silicon not just for speed, but to enable the "Parallel Bag" architecture that was theoretically described but computationally difficult in the past.

## 2. Core Components

### A. The Bag-Based Control Model (CPU)
- **True Probabilistic Selection:** Uses the `Distributor` and `Bag` levels (0-99) to ensure that every task and concept is selected with a probability proportional to its priority.
- **Resource Allocation:** Strict adherence to AIKR. Memory management (Forgetting) is handled by Bag eviction.
- **Concurrency:** Uses GCD to manage multiple "Attentional Channels" (distinct Bags for different sensory modalities).

### B. Probabilistic Batching (CPU-GPU Bridge)
- Instead of a single (Task, Concept) selection per cycle, MacNARS v2 selects a **Batch** (e.g., 256 or 512 pairs).
- This maintains the probabilistic nature of NARS while saturating the parallel compute units of the M-series chip.

### C. NAL Rule Kernel (Metal Compute)
- **Syllogistic Inference:** deduction, induction, abduction, comparison, analogy.
- **Structural Inference:** NAL-2 to NAL-4 (Sets, Relations, etc.).
- **Variable Unification (NAL-6):** Implemented as a parallel constraint satisfaction problem on the GPU.
- **Temporal/Procedural (NAL-7/8):** Handling sequences and operations.

### D. Neuro-Symbolic Integration (Future-Proofing)
- **Narsese LLM Gateway:** Compiling natural language directly into Narsese Statements (following the 2026 Gabriel & Wang paper).
- **Concept Embeddings:** Augmenting symbolic Concepts with vector embeddings stored in Unified Memory for fast semantic retrieval.

## 3. Implementation Phases

**Phase 1: Foundation (Current)**
- [x] Bag and Distributor classes in C++.
- [ ] Narsese Parser (Faithful to OpenNARS Java syntax).
- [ ] Memory structure (Concepts, Tasks, Beliefs) in page-aligned Unified Memory.

**Phase 2: The NAL Kernel**
- [ ] Implementation of Truth Functions and Budget Functions.
- [ ] Implementation of the Syllogistic Rule Table in C++.
- [ ] Porting of the Rule Table to Metal Compute Shaders (`NalRules.metal`).

**Phase 3: The Reasoning Cycle**
- [ ] The "Select-Infer-Insert" loop.
- [ ] Integration of the Metal command queue for Batch Inference.
- [ ] Validation against OpenNARS Java JUnit tests.

**Phase 4: Radical Research (Persona Shift)**
- [ ] Exploration of Quantum-inspired NARS (Superposition of Beliefs).
- [ ] Deep Sensorimotor Integration with Apple's Accelerate framework.
- [ ] Neuro-symbolic feedback loops.
