# Particle Simulator


This project implements Verlet integration to simulate particle physics in C, using GLFW for real-time visualization. This is a buildable project via Make which generate a binary executable that can run the different scripts.
<div align="center">
  <img src="https://github.com/davidomanovic/particle-simulator/blob/main/assets/demo_program.gif" width="250" height="250" />
</div>

---

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Requirements](#requirements)
- [Installation](#installation)
- [Project Structure](#project-structure)
- [Usage](#usage)
- [Misc](#misc)


---

## Overview

Verlet integration is a numerical method used to simulate the motion of particles with great stability and minimal computational cost, even when forces become complex. This project leverages the Verlet algorithm to simulate realistic particle physics, including collision detection and response, in a 2D space. GLFW provides real-time rendering, enabling visual feedback and parameter adjustments like gravity, particle interaction, and potential fields.

---

## Features

- **Stable Verlet Integration**: Accurately simulates particle movement with position-based integration.
- **GLFW Visualization**: Real-time rendering of particles in a 2D environment.
- **Collision Handling**: Particle-to-particle and boundary collision detection with response.
- **Performance Efficient**: Handles a large number of particles smoothly.
- 
---

### Project Structure
```makefile
Verlet-Particles/
├── bin/                # Compiled binaries
├── include/            # Header files
├── libs/               # External dependencies (e.g. GLFW, GLUT, Unity added as submodules)
├── scripts/            # Example scripts (e.g., gas_simulation.c, impulse_simulation.c)
├── src/                # Core simulation logic
├── tests/              # Unit tests
├── assets/             # Assets for visualization (e.g., GIFs, images)
├── Makefile            # Build rules
└── README.md           # Documentation
```

## Requirements

To build and run the project, ensure the following are installed:

- **C compiler** (e.g., GCC or Clang) - add it to your system environment PATH
- **Make** for building the project

### Ubuntu Installation:

```bash
sudo apt update
sudo apt install -y build-essential \
  libgl1-mesa-dev mesa-common-dev \
  libx11-dev xorg-dev \
  libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev \
  libglfw3-dev
```

### Windows Installation:
Install MSYS2 and use its package manager:
```bash
pacman -S mingw-w64-x86_64-glfw mingw-w64-x86_64-gcc
```


### How to use project

After installing necessary stuff, it is very straight forward to use my project.

1. Clone the repository recursively:
```bash
git clone --recursive https://github.com/davidomanovic/particle-simulator.git
```
2. Build the project with `make` simply by writing in terminal (inside the project repo):
```bash
make
```

3. Run the binaries from /bin (executable files) after building
```bash
./bin/gas_simulation
```

## Contributions
Contributions are welcome! Fork this repository, make your changes, and submit a pull request.

## License
This project is licensed under the MIT License.
