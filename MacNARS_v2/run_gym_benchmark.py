import gymnasium as gym
import numpy as np
from stable_baselines3 import PPO
import macnars

def train_ppo():
    env = gym.make("CartPole-v1")
    print("Training PPO Baseline (20,000 steps)...")
    model = PPO("MlpPolicy", env, verbose=0)
    model.learn(total_timesteps=20000)
    print("PPO Training Complete.")
    return model

def test_ppo(model, gravity_multiplier=1.0):
    env = gym.make("CartPole-v1")
    if gravity_multiplier != 1.0:
        env.unwrapped.gravity *= gravity_multiplier
        print(f"PPO Testing on modified gravity: {env.unwrapped.gravity}")
    
    obs, info = env.reset(seed=42)
    total_reward = 0
    for _ in range(500):
        action, _states = model.predict(obs, deterministic=True)
        obs, reward, terminated, truncated, info = env.step(int(action))
        total_reward += reward
        if terminated or truncated:
            break
    print(f"PPO Total Reward: {total_reward}")
    return total_reward

def test_macnars(gravity_multiplier=1.0, episodes=5):
    env = gym.make("CartPole-v1")
    if gravity_multiplier != 1.0:
        env.unwrapped.gravity *= gravity_multiplier
        print(f"MacNARS Testing on modified gravity: {env.unwrapped.gravity}")

    agent = macnars.DualProcessAgent()
    
    final_reward = 0
    print("MacNARS Learning Curve (Episodes):")
    for ep in range(episodes):
        obs, info = env.reset(seed=42 + ep)
        total_reward = 0
        for _ in range(500):
            action = agent.step(obs)
            obs, reward, terminated, truncated, info = env.step(action)
            total_reward += reward
            if terminated or truncated:
                break
        print(f"  Episode {ep + 1}: {total_reward} steps survived. Cortex Inferences: {agent.get_cortex_cycles()}")
        final_reward = total_reward
    
    return final_reward

if __name__ == "__main__":
    print("====================================================")
    print(" Training Baselines")
    print("====================================================")
    ppo_model = train_ppo()
    
    print("\n====================================================")
    print(" CartPole-v1: Standard Environment")
    print("====================================================")
    ppo_reward_standard = test_ppo(ppo_model, 1.0)
    macnars_reward_standard = test_macnars(1.0)

    print("\n====================================================")
    print(" CartPole-v1: Extreme Perturbation (Gravity x2.5)")
    print("====================================================")
    ppo_reward_heavy = test_ppo(ppo_model, 2.5)
    macnars_reward_heavy = test_macnars(2.5)

    print("\n====================================================")
    print(" Summary Results")
    print("====================================================")
    print(f"PPO (Trained 20k steps, Standard Env): {ppo_reward_standard} / 500.0")
    print(f"MacNARS (Online Learning, Standard Env): {macnars_reward_standard} / 500.0")
    print(f"PPO (Trained 20k steps, Gravity x2.5): {ppo_reward_heavy} / 500.0")
    print(f"MacNARS (Online Learning, Gravity x2.5): {macnars_reward_heavy} / 500.0")
