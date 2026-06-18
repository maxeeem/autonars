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
