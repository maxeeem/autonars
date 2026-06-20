import gymnasium as gym
import minigrid
import macnars_v8_gym
import numpy as np

def extract_features(obs_dict, env):
    # obs_dict["image"] is 7x7x3.
    # [3, 5, :] is directly in front
    # 0: object type, 1: color, 2: state
    image = obs_dict["image"]
    front_obj = image[3, 5, 0]
    front_state = image[3, 5, 2]

    # features:
    is_goal = float(front_obj == 8)
    is_wall = float(front_obj == 2)
    is_door = float(front_obj == 4)
    is_key = float(front_obj == 5)
    
    # check if holding key
    carrying = env.unwrapped.carrying
    has_key = float(carrying is not None and carrying.type == 'key')
    
    # check if door is open
    door_open = float(front_obj == 4 and front_state == 0)

    # 6 features
    return np.array([is_goal, is_wall, is_door, is_key, has_key, door_open], dtype=np.float32)

def test_macnars_doorkey(episodes=10):
    env = gym.make("MiniGrid-DoorKey-5x5-v0")
    agent = macnars_v8_gym.MacNARSV8Agent()

    print("====================================================")
    print(" MacNARS v8: MiniGrid DoorKey (Partially Observable)")
    print("====================================================")

    for ep in range(episodes):
        obs, info = env.reset(seed=42 + ep)
        agent.reset_state()
        steps = 0
        total_reward = 0.0
        
        # MiniGrid actions: 0=Left, 1=Right, 2=Forward, 3=Pickup, 4=Drop, 5=Toggle
        
        while steps < 200:
            features = extract_features(obs, env)
            
            # Agent step: action is chosen based on perception and current reward
            # To train the goal, we give reward when terminated and successful
            
            action = agent.step(features, 0.0) # Step without immediate reward
            
            # MacNARS returns 0..3 maybe, but we need 0..5
            # We map best_op (0..5) directly. 
            # In engine.step, it falls back to rand() % 4. We should probably adjust that.
            
            obs, reward, terminated, truncated, info = env.step(action)
            steps += 1
            
            if terminated and reward > 0:
                # Goal reached!
                features = extract_features(obs, env)
                agent.step(features, 1.0) # Ingest reward
                total_reward += reward
                break
            
            if truncated:
                break
                
        status = "Goal Reached!" if terminated and total_reward > 0 else "Failed (Timeout)"
        print(f"  Episode {ep + 1}: {steps} steps. Status: {status}")

if __name__ == "__main__":
    test_macnars_doorkey()
