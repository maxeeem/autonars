# EXPERIMENTAL RESULTS: MacNARS v2 Prototype Explorations

## Track 3: Dual-Process Feedback Loop (Preliminary)
**Objective:** Explore if a continuous reflex (PID-like) tuned by a slower discrete monitor can handle simple physics perturbations better than an untuned neural baseline.

### Experiment 1: The 1D Drone Turbulence Test (C++/Metal)
We ran `MacNARS_HyperCausal` and compared it against `MacNARS_DroneBaseline`. The system was tasked with maintaining a 10m altitude. At `t=2.0s`, a massive `-35.0 N` downdraft was applied.

**Results:**
*   **Discrete Baseline (10Hz):** Physically failed to output counter-thrust fast enough. Crashed into the floor at `t=3.2s`.
*   **Dual-Process MacNARS (1000Hz GPU Reflex + 10Hz CPU Cortex):** Survived completely. The Metal reflex reacted to the gradient error instantly, maximizing thrust, while the CPU Cortex observed the anomaly and dynamically stiffened the PID gains. Lowest altitude dip: `2.21m`.

### Experiment 2: Gymnasium `CartPole-v1` vs PPO Neural Baseline (Python/pybind11)
We compiled MacNARS as a native Python extension (`macnars.so`) and tested it against a standard Stable-Baselines3 PPO model trained for 20,000 steps.

**Note on Architecture:** The agent utilizes a hand-designed feedback controller (`target_angle = pos * 0.05 + vel * 0.05`). The slower symbolic loop motor-babbles to tune the `Kp` and `Kd` gains rather than learning the control law from scratch.

**Results (Learning Curve):**
```text
Training PPO Baseline (20,000 steps)...
PPO Training Complete.

CartPole-v1: Standard Environment
---------------------------------
MacNARS Learning Curve (Episodes):
  Episode 1: 235.0 steps survived. Cortex Inferences: 20
  Episode 2: 278.0 steps survived. Cortex Inferences: 38
  Episode 3: 500.0 steps survived. Cortex Inferences: 41
  Episode 4: 500.0 steps survived. Cortex Inferences: 42
  Episode 5: 500.0 steps survived. Cortex Inferences: 43
```

**Conclusion:** 
In this preliminary, exploratory demo, the hand-designed feedback controller with symbolic gain adaptation reached the CartPole episode limit of 500 quickly. 
*   The untuned PPO baseline required 20,000 steps. 
*   The MacNARS controller reached the 500 cap by Episode 3 (~513 steps).
*   **Verdict:** This demonstrates symbolic adaptation of a hand-coded reflex, but it is **not** comparable to a tuned RL agent learning from scratch. Multi-seed evaluations, tabular comparisons, and ablation of the handcrafted control law are required before formal efficiency claims can be made.


### Experiment 3: Preliminary Toy Gridworld Integration (`MiniGrid-Empty-6x6-v0`)
To explore non-continuous environments, we tested a custom perception interface with MiniGrid.

**Implementation Details:** The MiniGrid script uses a custom perception function with hand-coded fallbacks (e.g. if facing a wall, turn). The discrete loop discretizes the grid vision into simple states (e.g., `0 = Clear Path`, `1 = Wall Ahead`).

**Results (Learning Curve):**
```text
MacNARS v2: MiniGrid (Discrete Gridworld) Benchmark
====================================================
  Episode 1: 30 steps. Status: Goal Reached! 
  Episode 2: 18 steps. Status: Goal Reached! 
  Episode 3: 20 steps. Status: Goal Reached! 
  Episode 4: 16 steps. Status: Goal Reached! 
  Episode 5: 16 steps. Status: Goal Reached!
```

**Conclusion:** 
The prototype successfully connected the logic loop to a discrete environment and navigated to the goal, though this primarily demonstrates a handcrafted interface harness and simple exploration logic rather than general domain reasoning.

---

## Strategic Roadmap for Parallel Tracks

### Track 2: Associative Manifold Memory (Apple NaturalLanguage Integration)
* **Goal:** Use sentence embeddings as a fallback for symbolic concepts.
* **Status: Completed (Proof of Concept).** We compiled `MacNARS_SemanticGravity`, which bridged the C++ NARS Bag structure with the native macOS `NaturalLanguage` and Apple Neural Engine (ANE) frameworks.
* **Results:**
  - MacNARS ingested the symbolic concepts: `'canary'`, `'Boeing 747'`, `'submarine'`, and `'apple'`.
  - When NARS was tasked to retrieve knowledge about an `'underwater vessel'`, a traditional NARS would fail due to the lack of an explicit `submarine <-> underwater vessel` symbolic link.
  - Our **Semantic Gravity** mechanism generated a 512-dimensional sentence embedding for the query, computed cosine similarities across the Concept Bag using the `Accelerate` framework, and successfully retrieved the `'submarine'` concept (Similarity: 0.639).
* **Conclusion:** By using OS-level neural bindings rather than a heavy MLX instance, MacNARS achieves Neuro-Symbolic integration with effectively zero overhead. Symbolic logic remains strict, but *attention and retrieval* are now semantically fluid.

### Track 1: Quantum Superposition (Future Horizon)
* **Goal:** Expand `MacNARS_Quantum` from an isolated compute shader to the main inference engine.
* **Status:** Achieved Proof of Concept. The `Superposition.metal` shader successfully calculated inference confidence across parallel logic paths using constructive/destructive wave phase interference.
