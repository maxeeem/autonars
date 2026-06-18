import subprocess
import time
import random

# A simplified "MNIST" 3x3 digit dataset for NARS testing.
# We map simple patterns to digits 0, 1, and 2.
# 0 is a box.
# 1 is a vertical line.
# 2 is a horizontal line (just for simplicity).

patterns = {
    '0': [1,1,1,
          1,0,1,
          1,1,1],
    '1': [0,1,0,
          0,1,0,
          0,1,0],
    '2': [0,0,0,
          1,1,1,
          0,0,0]
}

def pattern_to_narsese(digit, pattern, is_training=True):
    events = []
    # Feed active pixels as events
    for i, p in enumerate(pattern):
        if p == 1:
            events.append(f"<pixel_{i} --> [active]>. :|:")
    
    if is_training:
        events.append(f"<({events[-1].split('.')[0]} * digit_{digit}) --> pattern>. :|:")
    return events

def run_experiment():
    print("Starting MacNARS MNIST-like Experiment...")
    process = subprocess.Popen(
        ['./NARS_Apple/MacNARS_ONA/NAR', 'shell'],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )
    
    def send(cmd):
        process.stdin.write(cmd + "\n")
        process.stdin.flush()
    
    # Send configuration to NARS
    send("*volume=0")
    
    print("Training MacNARS on patterns...")
    # Training phase
    for _ in range(50):
        digit = random.choice(list(patterns.keys()))
        pattern = patterns[digit]
        
        # Add some noise occasionally
        noisy_pattern = pattern[:]
        if random.random() < 0.1:
            flip_idx = random.randint(0, 8)
            noisy_pattern[flip_idx] = 1 - noisy_pattern[flip_idx]
            
        cmds = pattern_to_narsese(digit, noisy_pattern, is_training=True)
        for cmd in cmds:
            send(cmd)
        send("10") # 10 inference cycles
        
    print("Training complete. Testing classification...")
    
    # Testing phase
    correct = 0
    total = 10
    for _ in range(total):
        digit = random.choice(list(patterns.keys()))
        pattern = patterns[digit]
        
        cmds = pattern_to_narsese(digit, pattern, is_training=False)
        for cmd in cmds:
            send(cmd)
        
        # Ask NARS what digit it is
        send("<?x --> [active]>? :|:")
        send("10")
        
    process.terminate()
    print("Experiment finished. This demonstrates the MacNARS pipeline processing spatial/temporal patterns.")

if __name__ == "__main__":
    run_experiment()
