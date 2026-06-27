# Boids Simulation in OpenGL / GLUT

An interactive, high-performance, and visually elegant 2D Boids simulation following Craig Reynolds' collective movement model. The project features a customizable sidebar interface, smooth animations, dynamic color schemes, and precise coordinate mapping.

## Features
- **Reynolds rules**: Separation, Alignment, and Cohesion forces acting on each boid dynamically.
- **Continuous movement**: Smooth continuous coordinate updates with velocity limits (minimum and maximum speeds).
- **Custom UI Sidebar**: Transparent, collapsible, and beautifully styled control panel to tweak parameters in real-time.
- **Adaptive Screen Bounds**:
  - **Bounce Mode**: Elastic collision boundary reflection.
  - **Toroidal Wrapping**: Seamless teleportation to the opposite edge when passing boundaries.
- **Visual Elegance**: Bird silhouettes painted in unique HSV-to-RGB colors per agent, animated wing flapping depending on velocity, and fading movement trails.
- **Robust Input Handling**: Automatic scaling of mouse position relative to window resizing, fully compatible with tiling window managers like Hyprland under Linux.

## Requirements
- C++ Compiler (GCC support)
- OpenGL Utility Toolkit (GLUT)
- OpenGL and GLU libraries

On Debian/Ubuntu systems, install dependencies using:
```bash
sudo apt-get install build-essential freeglut3-dev libgl1-mesa-dev libglu1-mesa-dev
```

## Compilation and Execution
To compile and run the application, use the provided `Makefile`:

```bash
# Compile and build the project
make

# Clean build artifacts
make clean

# Run the simulation
./proyecto_opengl
```

## Controls
- **Mouse Click & Drag**: Interact with buttons, sliders, segmented controls, double range slider, and checkbox in the UI sidebar.
- **`I` / `i`**: Toggle the UI sidebar panel open or closed.
- **`Spacebar`**: Reset the simulation.
- **`ESC`**: Exit the application.
- **`+` / `-`**: Increase/decrease the boids count by 10.
- **`Q` / `A`**: Increase/decrease neighbor radius.
- **`W` / `S`**: Increase/decrease separation weight.
- **`E` / `D`**: Increase/decrease alignment weight.
- **`R` / `F`**: Increase/decrease cohesion weight.
- **`T` / `G`**: Increase/decrease minimum speed.
- **`Y` / `H`**: Increase/decrease maximum speed.
- **`B` / `b`**: Toggle edge boundaries mode.

## Project Structure
- `include/`: Header files for boids, input, rendering, simulation physics, and UI design.
- `src/`: Core implementation modules.
- `Makefile`: Script for clean compilation.
