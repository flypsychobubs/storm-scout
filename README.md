# Storm Scout — Nintendo 3DS Homebrew

A small, playable top-down storm-chasing game for Nintendo 3DS. Drive across the radar grid, follow a moving supercell, deploy probes near it, and capture storm data before you run out of fuel or vehicle condition.

## Build

Requires devkitPro with the 3DS development tools (`devkitARM`, `libctru`, and `3ds_rules`). From the project directory, run:

```sh
make
```

This produces `StormScout.3dsx` for homebrew launchers and `StormScout.elf` for debugging. To remove build outputs, run `make clean`.

## Controls

- **D-pad:** Drive one tile (hold to keep moving)
- **A:** Capture the storm when adjacent to its core
- **B:** Deploy a probe when adjacent to the storm (limited supply; improves the next capture)
- **START:** Begin a chase from the title screen, or restart after the run
- **SELECT:** Quit

## Gameplay

Chase and sample five storms. The storm drifts across the radar over time. Stay close to its core to capture it; the closer you are, the better the score. A successful capture restores a little fuel and vehicle condition. Driving costs fuel, and getting caught in the storm's core damages the vehicle. The run ends when you finish the five-storm campaign or your fuel/condition runs out.

The game uses libctru's built-in text console, so it has no external assets or extra runtime files.

## Project layout

- `source/main.c` — game loop, radar, controls, and campaign logic
- `Makefile` — devkitPro/libctru build configuration

This project is distributed under GPL-3.0, matching the repository license.
