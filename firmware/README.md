# CEM Firmware Base — Control Logic (Simulation)

Portable C implementation of the base commutation logic for the CEM system.  
Matches the visual demo under `viz/index.html`.

- No sensors / no hard-coded constants
- All parameters adjustable at runtime via a simple serial CLI
- Output: JSON lines (JSONL) for easy logging and viz playback

## Build (desktop)

```bash
cd firmware
gcc cem_firmware_base.c -lm -o cem_sim
