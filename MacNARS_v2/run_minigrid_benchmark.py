import gymnasium as gym
import minigrid
import macnars_minigrid
import time

def test_macnars_minigrid(episodes=5):
    env = gym.make("MiniGrid-Empty-6x6-v0")
    agent = macnars_minigrid.MiniGridAgent()
    
    print("====================================================")
    print(" MacNARS v2: MiniGrid (Discrete Gridworld) Benchmark")
    print("====================================================")
    
    for ep in range(episodes):
        obs, info = env.reset(seed=1337 + ep)
        steps = 0
        total_reward = 0
        
        while steps < 100: # Max steps
            # 0=Left, 1=Right, 2=Forward in MiniGrid
            action = agent.step(obs)
            
            obs, reward, terminated, truncated, info = env.step(action)
            steps += 1
            total_reward += reward
            
            if terminated:
                break
                
        status = "Goal Reached!" if terminated and reward > 0 else "Failed (Timeout)"
        print(f"  Episode {ep + 1}: {steps} steps. Status: {status} (Cortex Inferences: {agent.get_cortex_cycles()})")

if __name__ == "__main__":
    test_macnars_minigrid()
