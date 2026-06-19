import gymnasium as gym
import ale_py
import numpy as np
import cv2
import macnars_v4
import time

# A Neuro-Symbolic bridge for Atari Breakout.
# It uses OpenCV to extract object centroids (Paddle, Ball) and transforms 
# them into continuous latent vectors for the MacNARS PerceptionCortex.
class AtariNeuroSymbolicBridge:
    def __init__(self):
        # Paddle and Ball colors in Breakout (Standard palette)
        self.paddle_color = [200, 72, 72] # Red-ish
        self.ball_color = [200, 72, 72]   # Also red-ish
        self.last_ball_pos = [0.5, 0.5]
        
    def extract_latent_state(self, obs):
        # 1. Image preprocessing
        paddle_area = obs[189:194, :, :]
        ball_area = obs[93:189, :, :]
        
        # 2. Extract Paddle X-coord
        paddle_mask = cv2.inRange(paddle_area, np.array([190, 70, 70]), np.array([210, 80, 80]))
        paddle_coords = np.column_stack(np.where(paddle_mask > 0))
        paddle_x = np.mean(paddle_coords[:, 1]) / 160.0 if len(paddle_coords) > 0 else 0.5
        
        # 3. Extract Ball X/Y-coord
        ball_mask = cv2.inRange(ball_area, np.array([190, 70, 70]), np.array([210, 80, 80]))
        ball_coords = np.column_stack(np.where(ball_mask > 0))
        if len(ball_coords) > 0:
            ball_y = (np.mean(ball_coords[:, 0]) + 93.0) / 210.0
            ball_x = np.mean(ball_coords[:, 1]) / 160.0
        else:
            ball_x, ball_y = 0.5, 0.5

        # 4. Extract Velocity
        ball_vel_x = ball_x - self.last_ball_pos[0]
        ball_vel_y = ball_y - self.last_ball_pos[1]
        self.last_ball_pos = [ball_x, ball_y]

        # 5. Return continuous vector for NARS: [p_x, b_x, b_y, b_vel_x, b_vel_y, dist]
        return np.array([paddle_x, ball_x, ball_y, ball_vel_x, ball_vel_y, ball_x - paddle_x], dtype=np.float32)

def run_atari_benchmark(episodes=50):
    env = gym.make("ALE/Breakout-v5", render_mode=None)
    bridge = AtariNeuroSymbolicBridge()
    agent = macnars_v4.V4AtariAgent() # Unified NAL-8 Architecture

    print("====================================================")
    print(" MacNARS v3: Atari Breakout Neuro-Symbolic Benchmark")
    print("====================================================")
    print("Strategy: Persistent NAL-9 Cortex + Causal Event Extraction")
    
    scores = []
    
    for ep in range(episodes):
        obs, info = env.reset(seed=1337 + ep)
        terminated = truncated = False
        total_reward = 0
        reward = 0.0
        
        # Start game (Fire ball)
        obs, _, _, _, _ = env.step(1) 
        
        while not (terminated or truncated):
            latent_vec = bridge.extract_latent_state(obs)
            raw_action = agent.step(latent_vec, float(reward))
            
            # Map NARS binary output to Atari moves
            atari_action = 3 if raw_action == 0 else 2
            
            obs, reward, terminated, truncated, info = env.step(atari_action)
            total_reward += reward
            
        scores.append(total_reward)
        avg_score = np.mean(scores[-10:])
        print(f"  Episode {ep + 1}: Score: {total_reward}. Avg (last 10): {avg_score:.2f}. Cortex Inferences: {agent.get_cortex_cycles()}")

if __name__ == "__main__":
    run_atari_benchmark()
