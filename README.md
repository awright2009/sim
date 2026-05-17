# eVTOL Simulation

A simple C++ simulation for modeling eVTOL vehicle operations, charging behavior, and fleet statistics.

The simulation models:
- Vehicle flight operations
- Battery energy usage
- Charging station contention
- FIFO charger queues
- Vehicle state transitions
- Aggregate operational statistics

The project was written as a lightweight systems-oriented simulation exercise with an emphasis on:
- deterministic state-machine behavior
- simple runtime architecture
- low allocation overhead
- unit testing
- Doxygen documentation

---

# Features

- State-machine driven vehicle simulation
- Airborne / Ground / Charging vehicle states
- FIFO charging queue support
- Configurable simulation duration
- CSV-driven configuration
- Aggregate simulation statistics
- Google Test unit tests
- Doxygen documentation support

---

# Project Structure

```text
src/        Main source files
include/    Header files
test/       Google Test unit tests
docs/       Generated Doxygen documentation
input/      Example CSV input files
```

---

# Building

## Linux

```bash
make
```

## Debug Build

```bash
make debug
```

## Clean

```bash
make clean
```

---

# Running

```bash
./sim 3.0
```

# Doxygen Documentation

Generate documentation using:

```bash
doxygen Doxyfile
```

Open generated documentation:

```bash
docs/html/index.html
```


# Design Notes

The simulation intentionally remains relatively lightweight and single-threaded.

Future improvements could include:
- OpenMP parallelization
- Geographic coordinates
- Visualization support
- Physics modeling
- Event-driven scheduling
- Battery degradation models

# Dependencies

- C++20
- GNU Make
- Google Test
- Doxygen
- Graphviz (optional for graphs)

Ubuntu example:

```bash
sudo apt install build-essential doxygen graphviz libgtest-dev
```

---

# License

MIT License
