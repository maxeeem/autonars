# RESEARCH REPORT: Superposition Control and Semantic Manifolds in MacNARS v2

## 1. Radical Hypothesis: The Inference Wavefunction
Traditional NARS systems are limited by a "bottleneck of attention"—picking one concept and one task per cycle. While this respects AIKR (Assumption of Insufficient Knowledge and Resources), it fails to utilize the massive parallelism of modern silicon.

**The Superposition Model** proposes that "Attention" is not a discrete selection, but a **Probability Distribution**. 
- **Mechanism:** We use Metal compute shaders to execute *all* high-priority inferences in parallel. 
- **The Twist:** We introduce **Phase Interference**. Two conflicting beliefs in NARS normally undergo "Revision" (additive evidence). In the Superposition model, if they are in opposite "Phases" (contextually incompatible), they destructively interfere, reducing the confidence of the resulting derivation. This allows the system to prune contradictory logic paths before they consume CPU resources.

## 2. Associative Manifold Memory
By mapping Narsese Terms to a high-dimensional vector space (using a pre-trained LoRA adapter like the Phi-2 model identified in 2026 research), we create a **Semantic Manifold**.
- **Symbolic + Neural:** The Bag-based selection is no longer just probabilistic; it is **Geodesic**. Concepts that are "close" in the neural embedding space exert a "Semantic Gravity" on each other.
- **Outcome:** If the system is thinking about "Bird", concepts like "Wing" and "Flight" receive a priority boost not just through symbolic links, but through their proximity on the manifold. This enables a form of "Intuitive Leaping" that NARS has historically struggled with.

## 3. Hyper-Causal Sensorimotor Feedback: The Continuous Logic Reflex
Standard NARS treats operations as discrete "pulses." In a physical world, this leads to jerky, high-latency control. 

**The Hyper-Causal Model** introduces a dual-process control loop:
- **The Reflex (GPU):** A high-frequency (1kHz+) PID-like controller implemented in Metal shaders. It handles the continuous adjustment of motor outputs to meet a "Setpoint."
- **The Reasoner (CPU):** The symbolic NARS logic monitors the "Surprise" metric (Causal Violation) from the Reflex. 
- **The Coupling:** 
    1. Symbolic Logic sets the `target_value` (The Goal).
    2. The Reflex executes the continuous movement.
    3. If the movement fails (Surprise!), NARS receives an immediate priority boost to re-reason about the environment (e.g., "Is the arm stuck?").
- **Innovation:** This matches the biological distinction between the **Spinal Reflex** (sub-millisecond) and **Cortical Reasoning** (hundreds of milliseconds). MacNARS v2 becomes the first NARS implementation to have a "nervous system" capable of reflexive balance and symbolic planning simultaneously.

---

## 4. The Frontiers of Scalability: Perception, Pixels, and NAL-9

While MacNARS v2 demonstrates significant advancements in sample efficiency and Dual-Process control, it encounters the classic bottlenecks of symbolic AI when scaling to arbitrarily complex environments.

### A. The "Handcrafted Harness" Problem
Currently, expanding to a new environment requires writing a custom `perceive()` function to discretize the state space (e.g., turning continuous Cartesian coordinates into `<pole --> [drifting]>`). 
*   **The Issue:** A true AGI cannot rely on a human programmer to define its ontology.
*   **The Solution:** The system needs an **Unsupervised Concept Generator**. In a Neuro-Symbolic architecture, we would use a Vector Quantized Variational Autoencoder (VQ-VAE). The VQ-VAE processes raw state inputs and clusters them into discrete "latent codes." MacNARS would ingest these arbitrary latent codes as opaque Narsese atoms (`<latent_code_42 --> [active]>`). Over time, NARS assigns semantic meaning to `latent_code_42` purely based on its causal relationship with rewards and operations.

### B. Learning from Atari Pixels
*   **Can NARS learn directly from raw pixels?** Pure symbolic NARS cannot do this efficiently. Treating each of the 100,000 pixels in an Atari frame as a separate Narsese term leads to a combinatorial explosion that violates AIKR (Assumption of Insufficient Knowledge and Resources).
*   **The Neuro-Symbolic Bridge:** To play Atari, NARS requires a **Perceptual Frontend** (a Convolutional Neural Network or Object Tracker). Instead of feeding pixels to NARS, the CNN extracts bounding boxes and object velocities (e.g., `Object_1_pos`, `Object_2_vel`). NARS then reasons over the *objects*, not the pixels: `<( <Object_1 --> [moving_down]> &/ ^move_paddle_right ) =/> <Object_1 --> [bouncing]> >`. This is exactly how the human cortex works: the retina and visual cortex (neural) extract edge and object representations long before the prefrontal cortex (symbolic NARS) plans a reaction.

### C. NAL-9: Mental Operations and Introspection
NAL-1 through NAL-8 deal with perceiving and acting upon the *external* world. **NAL-9** introduces operations that act on the *internal* world (the NARS system itself).
*   **Mental Operations:** NAL-9 introduces operators like `^believe`, `^doubt`, `^wonder`, and `^evaluate`.
*   **Self-Modification:** With NAL-9, MacNARS could observe its own reasoning process. If NARS notices that it frequently executes a rule but the expectation consistently fails, NAL-9 allows it to explicitly target that rule for deletion or severe confidence decay.
*   **Introspective Attention:** It allows the system to dynamically tune its own `Bag` parameters. If the system is "bored" (no high-priority tasks), a NAL-9 operation could increase the baseline novelty threshold, forcing the system to retrieve obscure memories via the Semantic Manifold.

**Conclusion on Frontiers:** 
To reach Atari-level General Intelligence, the MacNARS architecture must evolve into a tri-partite system: 
1. **CNN/VQ-VAE (Perception)** -> 2. **Metal Reflex (Continuous Control)** -> 3. **NAL-9 Cortex (Symbolic/Introspective Reasoning).**
