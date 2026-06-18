# Research Strategy: Conquering the AGI Frontiers

This document outlines the concrete implementation plan to evolve MacNARS v2 into MacNARS v3 by solving the three major bottlenecks of Symbolic AGI.

## Frontier A: Unsupervised Concept Generation (Replacing Handcrafted Harnesses)
**The Goal:** NARS must dynamically build its own discrete symbolic state space from continuous data.
**The Strategy (Online Clustering):**
1. Implement a `PerceptionCortex` in C++.
2. Instead of hardcoding `if (angle > 0.05) return CRITICAL`, the PerceptionCortex will use an **Online Distance-Based Clustering** algorithm (similar to streaming K-Means). 
3. When a continuous observation vector arrives, it is compared to existing "Latent Concepts". If it is similar (within a radius), it activates that concept. If it is highly novel, the system dynamically creates a new Concept (e.g., `State_4`).
4. The `ProceduralCortex` will then learn causal rules over these dynamically generated `State_X` symbols.

## Frontier B: Learning from Raw Pixels (Atari / Convolutional Frontends)
**The Goal:** Bridge high-dimensional pixel inputs (like Atari Breakout) to NARS without combinatorial explosion.
**The Strategy (The Neuro-Symbolic Bridge):**
1. We cannot process pixels in C++ NARS. We will use Python and PyTorch.
2. We will run a lightweight, pre-trained Convolutional Neural Network (or a fast OpenCV contour extractor) in the Python Gym wrapper.
3. The Python layer will extract **Object Bounding Boxes and Velocities**.
4. Python will translate these into Narsese strings: `<Object_1 --> [moving_down]>`.
5. These strings will be fed into the `macnars.so` PyBind11 interface, allowing the NAL-8 cortex to learn how to play Atari by reasoning over moving objects.

## Frontier C: NAL-9 Introspection (Mental Operations)
**The Goal:** Allow NARS to reflect on and modify its own reasoning process.
**The Strategy (The Meta-Cortex):**
1. Add NAL-9 mental operations: `^believe` and `^doubt`.
2. Implement **Hypothesis Evaluation**: If NARS executes a learned rule `<(State_1 &/ ^Op) =/> State_2>` but the outcome is `State_3` (Surprise), NARS will autonomously execute the `^doubt` operation on that specific rule.
3. `^doubt` will instantly slash the confidence of the hypothesis, preventing the agent from repeating the same mistake and enforcing rapid un-learning of false causality.

---
**Execution:** I will implement these sequentially, starting with the `PerceptionCortex` for Unsupervised Concept Generation.
