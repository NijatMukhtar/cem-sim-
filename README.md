# CEM-SIM — Circular Electromagnet Motor Simulator

A minimal open-source simulator for the **Circular Electromagnet Motor (CEM)**.  
It models the rotor–stator interaction logic and visualizes commutation timing in real time.

---

## Structure


- `cem.h` → Core data structures and constants  
- `cem.c` → Simulation logic (angle computation, EM activation)  
- `cli.c` → Command interpreter (set/run loop)  
- `hal_stub.c` → Hardware abstraction layer placeholder (to be replaced by STM32 HAL)  
- `main.c` → Entry point  

---

## ⚙️ Build Instructions

### 1. Prerequisites
- **CMake ≥ 3.15**
- **gcc** or **clang**
- **Make** (usually included by default)
- On macOS:  
  ```bash
  brew install cmake


Build:
```bash
From the firmware/ directory:
```bash
mkdir -p build
cd build
cmake ..
make -j$(sysctl -n hw.ncpu)

Run:
./cem_sim


| Command                     | Description                     |
| --------------------------- | ------------------------------- |
| `set rpm 120`               | Set rotor speed (RPM)           |
| `set n 8`                   | Set number of electromagnets    |
| `set overlap 25`            | Define overlap angle in degrees |
| `set mode 1` / `set mode 2` | Toggle activation mode          |
| `set angles 0,45,90,...`    | Manually set EM timing table    |
| `run 2`                     | Run simulation for 2 seconds    |



> ⚠️ Note: This folder may be `.gitignored` if large or auto-generated logs are produced.
> Keep only representative runs (e.g., `run_log.jsonl`) for reference and documentation.

