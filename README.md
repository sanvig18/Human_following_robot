# Autonomous Target-Tracking Robot Controller (C++) 🤖⚡

An embedded C++ controller utilizing object-oriented interfaces to govern real-time navigation pipelines for human-following autonomous robotics. This system couples object-distance linear interpolation algorithms with discrete logic inputs to maintain strict spatial positioning bounds without CPU execution blocks.

Architected to showcase clean **Object-Oriented Design (OOD)**, hardware abstraction techniques, and mathematical processing for the Google Apprenticeship application framework.

---

## 🧠 Architectural Insights for Reviewers
- **Hardware Abstraction Layer (HAL)**: Employs a pure virtual `HardwareInterface` class. This isolates the core analytical execution engine from raw physical registers, making the software instantly portable between Arduino, ESP32, STM32, or high-fidelity simulation wrappers.
- **Linear Interpolation Dynamics**: Rather than using step-wise speed tiers, the algorithm calculates real-time PWM transitions linearly mapping sensor space to mechanical voltage fields:
  $$PWM_{out} = Speed_{base} + \left( \frac{Dist_{current} - Dist_{min}}{Dist_{max} - Dist_{min}} \right) \times (Speed_{max} - Speed_{base})$$
- **Non-Blocking Execution Profile**: Designed to evaluate sensor state streams asynchronously inside high-frequency cyclic execution loops, satisfying low-latency embedded safety limits.

---

## 🛠️ Compilation and Target Deployment

### Direct Local Execution
To verify the tracking state machine logic directly inside your native desktop environment using standard C++17 compilers:

```bash
g++ -std=c++17 main.cpp -o tracking_system
./tracking_system
