#include <metal_stdlib>
using namespace metal;

// Representation of a Sensorimotor State
struct SensorState {
    float current_value;
    float target_value;
    float integral_error;
    float last_error;
    
    // NARS causal parameters
    float confidence; // How much the system "trusts" this causal link
    float priority;   // Attentional weight of this sensor loop
};

struct PIDConstants {
    float Kp;
    float Ki;
    float Kd;
    float dt;
};

kernel void hyper_causal_reflex_loop(
    device SensorState* sensors [[buffer(0)]],
    constant PIDConstants& constants [[buffer(1)]],
    device float* motor_outputs [[buffer(2)]],
    uint id [[thread_position_in_grid]])
{
    SensorState s = sensors[id];
    
    // 1. Classical PID Logic (The "Reflex")
    float error = s.target_value - s.current_value;
    float p_term = constants.Kp * error;
    
    float i_error = s.integral_error + error * constants.dt;
    float i_term = constants.Ki * i_error;
    
    float d_term = constants.Kd * (error - s.last_error) / constants.dt;
    
    float output = p_term + i_term + d_term;
    
    // 2. Hyper-Causal NARS Coupling
    // The output is modulated by the NARS confidence. 
    // If the system is "unsure" of the causality, it dampens the reflex.
    motor_outputs[id] = output * s.confidence;
    
    // 3. Feedback for NARS
    // Update state for next GPU cycle
    sensors[id].integral_error = i_error;
    sensors[id].last_error = error;
    
    // Radical: The "Surprise" Metric
    // If the error delta is high despite the motor output, 
    // it signals a "Causal Violation" to the higher-level NARS logic.
    // This priority boost forces NARS to re-reason about this specific sensor.
    if (abs(error - s.last_error) > 0.1f) {
        sensors[id].priority = 1.0f; // Force attentional selection
    }
}
