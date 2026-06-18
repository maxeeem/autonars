#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <algorithm>

struct PIDConstants {
    float Kp;
    float Ki;
    float Kd;
    float dt;
};

// Physics constants
const float GRAVITY = -9.81f;
const float MASS = 1.0f;
const float TARGET_ALTITUDE = 10.0f;

// Simulates a single physics step (1ms)
void physics_step(float& altitude, float& velocity, float thrust, float wind_force) {
    thrust = std::max(0.0f, std::min(thrust, 30.0f)); 
    float acceleration = (thrust + (GRAVITY * MASS) + wind_force) / MASS;
    velocity += acceleration * 0.001f;
    altitude += velocity * 0.001f;
    if (altitude < 0.0f) {
        altitude = 0.0f;
        velocity = 0.0f;
    }
}

void run_simulation(bool is_dual_process) {
    float altitude = 0.0f;
    float velocity = 0.0f;
    float motor_thrust = 0.0f;
    
    float current_error = 0.0f;
    float integral_error = 0.0f;
    float last_error = 0.0f;

    PIDConstants pid = { 2.5f, 0.1f, 1.5f, 0.001f }; // Base PID (insufficient)
    if (!is_dual_process) {
        pid.dt = 0.1f; // Discrete NARS updates control at 10Hz
    }

    std::cout << "\n--- Running " << (is_dual_process ? "Dual-Process MacNARS (1000Hz Reflex)" : "Baseline Discrete NARS (10Hz Control)") << " ---\n";
    std::cout << std::left << std::setw(10) << "Time(s)" << std::setw(15) << "Altitude(m)" << "Status\n";

    bool crashed = false;

    for (int tick_1ms = 0; tick_1ms <= 5000; ++tick_1ms) {
        float current_time = tick_1ms * 0.001f;
        float wind_force = (current_time >= 2.0f && current_time < 2.5f) ? -35.0f : 0.0f; // Massive downdraft

        // 1. NARS Cortex Cycle (10Hz)
        if (tick_1ms % 100 == 0) { // Every 100ms
            // Cognitive Strategy Adjustment (Surprise detection)
            if (altitude < TARGET_ALTITUDE * 0.5f && current_time > 0.5f) {
                pid.Kp = 4.5f; // NARS adapts
            }

            // In Baseline Discrete NARS, motor thrust is ONLY calculated here (10Hz)
            if (!is_dual_process) {
                current_error = TARGET_ALTITUDE - altitude;
                integral_error += current_error * pid.dt;
                float d_term = pid.Kd * (current_error - last_error) / pid.dt;
                motor_thrust = (pid.Kp * current_error) + (pid.Ki * integral_error) + d_term;
                last_error = current_error;
            }
            
            if (tick_1ms % 500 == 0 || (current_time >= 2.0f && current_time <= 2.6f && tick_1ms % 100 == 0)) {
                std::cout << std::left << std::setw(10) << current_time << std::setw(15) << altitude 
                          << ((wind_force < 0) ? "[WIND GUST]" : "") << "\n";
            }
        }

        // 2. Reflex Spinal Cycle (1000Hz) - ONLY active in Dual-Process
        if (is_dual_process) {
            current_error = TARGET_ALTITUDE - altitude;
            integral_error += current_error * pid.dt;
            float d_term = pid.Kd * (current_error - last_error) / pid.dt;
            motor_thrust = (pid.Kp * current_error) + (pid.Ki * integral_error) + d_term;
            last_error = current_error;
        }

        // 3. Physics update (1000Hz)
        physics_step(altitude, velocity, motor_thrust, wind_force);

        if (altitude <= 0.0f && current_time > 1.0f && !crashed) {
            std::cout << ">>> CRASH DETECTED AT t=" << current_time << "s <<<\n";
            crashed = true;
        }
    }
    
    std::cout << "Final Altitude: " << altitude << "m " << (crashed ? "(FAILED)" : "(SURVIVED)") << "\n";
}

int main() {
    std::cout << "====================================================\n";
    std::cout << " Empirical Baseline: Drone Altitude Disturbance \n";
    std::cout << "====================================================\n";
    
    run_simulation(false); // Baseline ONA style (10Hz Control)
    run_simulation(true);  // MacNARS Dual-Process (1000Hz Reflex)
    
    return 0;
}
