# MacNARS: Apple Silicon Optimized Neuro-Symbolic Prototype

MacNARS is an experimental NARS-inspired control prototype built natively for macOS and Apple Silicon. It explores bridging symbolic logic (Non-Axiomatic Logic concepts) with continuous control loops and simple hardware acceleration (Metal, Apple NaturalLanguage embeddings).

In preliminary toy benchmarks, a hand-designed feedback controller with symbolic gain adaptation solves CartPole quickly and remains stable under simple physics perturbations. These results are not yet comparable to tuned RL baselines and should be treated as exploratory.

---

## 🚀 Key Explorations

1. **Preliminary CartPole Demo:** In a test against one untuned Stable-Baselines3 PPO run (20k steps), a hand-designed PID-like controller with NARS-inspired gain adaptation reached the CartPole limit in a few episodes. While this demonstrates symbolic adjustment of a reflex controller, stronger baselines, tabular comparisons, and multi-seed evaluations are required before any formal efficiency claims can be made.
2. **Feedback Stability:** When the environment physics were perturbed (Gravity multiplied by 2.5x), the hand-coded feedback controller predictably adjusted to the error. This explores a dual-process fast/slow control loop.
3. **Semantic Fallback:** By integrating Apple NaturalLanguage sentence embeddings natively, the prototype can use a cosine similarity fallback to retrieve semantically related concepts when exact symbolic matches fail.

---

## 🧠 Architectural Components

This prototype explores translating NARS-inspired concepts into a continuous execution model:

### 1. Dual-Process Control (Experimental)
The architecture splits processing into a fast reflex loop (a hand-coded continuous PID-like loop running in Metal shaders) and a slower discrete symbolic monitor.

### 2. Procedural Adaptation (Toy Implementations)
The system uses simple temporal induction clustering to adjust the parameters of the hand-coded controller online.

### 3. Associative Manifold Memory
MacNARS uses a C++/Objective-C++ bridge to macOS's native `NaturalLanguage` framework to extract sentence embeddings for simple semantic retrieval.

### 4. Parallel Inference Kernels
A toy Metal compute shader exploring how truth values might be evaluated in parallel across an array.

---

## 📂 Repository Structure

```text
.
├── MacNARS_v2/
│   ├── CMakeLists.txt           # Build configuration
│   ├── run_gym_benchmark.py     # Python script pitting MacNARS vs PPO
│   ├── include/
│   │   ├── Bag.h                # Core probabilistic selection data structure
│   │   ├── ConceptV3.h          # ECS Concept with Semantic Manifold anchors
│   │   ├── NAL_Truth.h          # NAL Second Edition truth functions
│   │   └── MemoryV3.h           # System memory and inference cycle
│   └── src/
│       ├── macnars_gym.cpp      # pybind11 wrapper exposing NARS to Python
│       ├── HyperCausalMain.mm   # 1D Drone Reflex vs Cortex simulation
│       ├── SemanticGravityMain.cpp # Apple Intelligence Embedding simulation
│       ├── Superposition.metal  # Quantum-inspired parallel logic shader
│       └── main.cpp             # Faithful NAL syllogistic rule tests
├── GYM_INTEGRATION_PLAN.md      # Blueprint for Gym benchmark architecture
├── PLAN_v2.md                   # Core architectural philosophy
├── RESEARCH_REPORT.md           # Deep dive into the radical research tracks
└── RESULTS.md                   # Detailed logs of experimental benchmarks
```

---

## 🛠 Getting Started

### Prerequisites
* **macOS** (Apple Silicon M1/M2/M3/M4 recommended for Metal/ANE acceleration)
* **CMake** (`brew install cmake`)
* **Python 3.11+**

### Building the C++ / Metal Simulations
```bash
cd MacNARS_v2
mkdir build && cd build
cmake ..
make

# Run the Semantic Gravity (Apple Intelligence) test
./MacNARS_SemanticGravity

# Run the Hyper-Causal Drone test
./MacNARS_HyperCausal
```

### Running the Python Gymnasium Benchmarks (MacNARS vs PPO)
*Note: This builds MacNARS as a native Python extension (`macnars.so`) using pybind11.*
```bash
cd MacNARS_v2
python3 -m venv venv
source venv/bin/activate
pip install gymnasium stable-baselines3 pybind11 numpy

# Compile the pybind11 extension
python3 -c "import pybind11; import sysconfig; import os; ext=sysconfig.get_config_var('EXT_SUFFIX'); print(f'c++ -O3 -Wall -shared -std=c++11 -fPIC -I{pybind11.get_include()} -I{sysconfig.get_paths()[\"include\"]} -undefined dynamic_lookup src/macnars_gym.cpp -o macnars{ext}')" > build_cmd.sh
sh build_cmd.sh

# Run the comparative benchmark
python3 run_gym_benchmark.py
```

---
*Developed as an exploration into high-performance, neuro-symbolic Artificial General Intelligence.*
