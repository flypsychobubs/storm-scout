# Storm Scout — Nintendo 3DS Homebrew

Storm Scout is a top-down storm-chasing game for Nintendo 3DS. Read the radar, drive the chase vehicle, deploy probes, and capture five storm samples before fuel or vehicle condition runs out.

## Build

Requires devkitPro with devkitARM and libctru installed. From the project directory, run:

```sh
make
```

This produces `StormScout.3dsx` for homebrew launchers and `StormScout.elf` for debugging. Run `make clean` to remove build outputs.

## Controls

- **D-pad:** Drive one tile (hold to keep moving)
- **A:** Capture storm data when within one radar tile
- **B:** Deploy a probe nearby for a score bonus
- **START:** Begin a chase or restart after the run
- **SELECT:** Quit

## Gameplay and interface

The upper screen presents the live radar, storm range, chase score, fuel, vehicle condition, probes, and signal status. Its tile radar distinguishes clear road, rain bands, the storm core, and the player's vehicle. The lower screen shows chase-radio guidance, mission status, and the latest dispatch. Storms drift as the chase continues. Samples restore a little fuel and vehicle condition; driving consumes fuel and entering a storm core damages the vehicle.

The game uses libctru's built-in dual-screen text console and has no external assets or runtime files. It is a fictional game—never enter real storms, floodwater, or closed roads.

## Project layout

- `source/main.c` — game loop, dual-screen interface, radar, and campaign logic
- `Makefile` — devkitPro/libctru build configuration

This project is distributed under GPL-3.0, matching the repository license.
