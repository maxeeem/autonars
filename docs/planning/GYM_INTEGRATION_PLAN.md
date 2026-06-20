# MacNARS Dual-Process vs Standard Neural Baselines: Gym Integration Plan

## Objective
To definitively prove that MacNARS (Dual-Process) materially surpasses the state-of-the-art in autonomous operation, we must benchmark it in a standardized environment against established neural baselines. 

We will use the **Gymnasium (formerly OpenAI Gym)** ecosystem.

## Target Environment: `CartPole-v1` and `LunarLander-v2`
- **Why CartPole?** It's the "Hello World" of continuous control and reinforcement learning. We want to demonstrate that MacNARS can balance the pole *zero-shot* by relying on the Metal PID reflex, while the NARS cortex adjusts the setpoint if the cart drifts too far.
- **Why LunarLander?** It introduces significant non-linear dynamics, gravity, and multiple thrusters.

## Architecture Bridge
MacNARS is written in C++/Metal. Gymnasium runs in Python. We need an efficient bridge.

### Approach: `pybind11` native module
Instead of piping strings via stdin/stdout (which was fine for the slow MNIST experiment), we will compile MacNARS as a native Python extension `macnars.so`.

```python
import gymnasium as gym
import macnars

env = gym.make("CartPole-v1")
# Initialize MacNARS Memory and Metal Engine
agent = macnars.DualProcessAgent(hz_reflex=1000, hz_cortex=10)

observation, info = env.reset()
for _ in range(1000):
    # NARS ingests observation (angle, velocity, pos)
    # The Metal Reflex computes the continuous motor output (Force)
    action = agent.step(observation) 
    
    observation, reward, terminated, truncated, info = env.step(action)
    
    if terminated or truncated:
        observation, info = env.reset()
```

## Comparison Metrics against Neural Baselines (PPO / SAC)
1. **Sample Efficiency (Zero-Shot vs Training):**
   - Standard PPO/SAC requires thousands of episodes (samples) to learn to balance the CartPole.
   - **Hypothesis:** MacNARS will solve the environment in **1 episode** (Zero-Shot) because the Metal PID Reflex inherently knows how to minimize error, while the NARS Cortex symbolically learns that "pushing right keeps the cart centered."
2. **Robustness to Perturbation:**
   - Neural baselines overfit to the exact gravity/mass of the training environment. If we suddenly double gravity in episode 50, PPO fails.
   - **Hypothesis:** When gravity is doubled, the MacNARS Reflex will register "Surprise" (Error > Threshold). The NARS Cortex will infer `<gravity --> [increased]>` and instantly boost the `Kp` gain, recovering within milliseconds without needing 1000 episodes to relearn the weights.
3. **Interpretability:**
   - PPO outputs an opaque matrix of weights.
   - MacNARS outputs an explicit logic trace: `If (Pole Angle > 0) AND (Surprise == High) THEN Increase(Reflex.Kp)`.

## Execution Roadmap
1. **Develop `pybind11` wrapper** for the `HyperCausalEngine` and `MemoryV2`.
2. **Deploy CartPole experiment script** comparing MacNARS zero-shot survival duration vs PPO.
3. **Publish Results.**
