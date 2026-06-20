# MacNARS v4: The Grand Unified Architecture (State of the Art)

## The SOTA Bottleneck
MacNARS v2 and v3 achieved substantial milestones by isolating the challenges of Symbolic AGI into specific tracks:
1. **Semantic Gravity (Track 2)**: Overcame synonym brittleness using Apple Intelligence Embeddings.
2. **Hyper-Causal Control (Track 3)**: Overcame the discrete-time bottleneck using Metal PID reflexes.
3. **Unsupervised Perception (Frontier A/B)**: Allowed NARS to play Atari from raw pixels via latent clustering.
4. **NAL-9 Introspection (Frontier C)**: Enabled rapid hypothesis pruning via internal mental operators (`^doubt`).

However, the **State of the Art bottleneck** is that these systems are fragmented. The Atari agent uses a fast tabular `ProceduralCortex` rather than the true AIKR-compliant `MemoryV3` Bag architecture. The Semantic Gravity module is isolated in `MemoryV3` and not hooked up to the visual perception system.

## The Next Research Goal: The Unified Neuromorphic Bag (MacNARS v4)

To push further beyond the SOTA, we must fuse these disparate elements into a single, unified execution graph.

### 1. Vector-Anchored Concept Bags
The `MemoryV3` Bag will be the sole memory structure. When the `PerceptionCortex` extracts an object (e.g., `<paddle --> [moving_right]>`), it is immediately mapped to a vector embedding (using `AppleIntelligenceBridge`). 
*   **The Breakthrough:** Latent states generated from Atari pixels will naturally group together in the semantic manifold. The system won't just learn that `State_42` leads to reward; it will inherently know that `State_43` (a slightly different paddle position) should inherit the same causal rules via **Semantic Gravity**.

### 2. NAL-7/8 Metal Acceleration
The current `RuleEngine.h` only supports NAL-2 syllogisms (`-->`). We will expand the NAL Rule Engine to handle Temporal and Procedural operations (`=/>` and `&/`).
*   Instead of `ProceduralCortex` iterating over a `std::map`, tasks like `(! (hit_brick))` will be dropped into the Bag.
*   The system will abduce `(! (move_paddle_right &/ hit_brick))` and select the `^right` operation entirely through symbolic probabilistic bag selection.

### 3. Hypothesis: The "Zero-Shot" Transfer
By integrating Semantic Gravity directly into the NAL-8 action selection loop, we hypothesize that an agent trained to play Breakout will exhibit **Zero-Shot transfer** to Pong. 
Why? Because the concept of `<paddle --> [intercepts] --> ball>` will be semantically close in the unified embedding space, allowing the NARS bags to leak priority from Breakout strategies directly into Pong strategies without retraining.

---
**Critique of MacNARS v4 Plan:**
*   **Risk:** Bag-based temporal induction is notoriously slow because events must be paired concurrently. 
*   **Mitigation:** We will leverage the Metal Compute queue (`HyperCausal.metal`) to perform batch-pairwise temporal comparisons, evaluating 10,000 NAL-7 `=/>` induction rules per frame, effectively turning the NAL reasoning cycle into a parallel matrix operation.
