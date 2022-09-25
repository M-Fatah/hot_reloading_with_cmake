<!-- badges: start -->
[![Build status](https://github.com/M-Fatah/hot_reloading_with_cmake/workflows/CI/badge.svg)](https://github.com/M-Fatah/hot_reloading_with_cmake/actions?workflow=CI)
<!-- badges: end -->
---

# **Hot reloading with CMake**

##### An example that demonstrates how to implement hot reloading for a game engine and integrate it with `CMake`.
##### The code example provided compiles and runs on both `Linux` and `Windows`.
[hot_reloading_with_cmake.webm](https://user-images.githubusercontent.com/6799254/192156217-d6ae8a05-b6f5-48d3-aa7c-eaaa21dd1c3e.webm)

## **The basic idea behind it:**
The engine neeeds to be built as a `dll`.
- Copy the game engine's `dll` to a new file.
- Load the copied `dll` from the `platform` layer or a `launcher` at runtime.
- Every frame check if the original `dll` has been modified and if so unload the copied `dll` and repeat the process.
- In debug mode, ensure that the generated `pdb` files are named randomly to avoid locking.

## **Platforms:**
- Linux.
- Windows.

## **Prerequisites:**
#### **Linux:**
- ```sudo apt install -y cmake```
#### **Windows:**
- Download and install [CMake](https://cmake.org/download/) (version 3.20 atleast).

## **Building:**
- Run one of the provided scripts, for example `build_debug.sh` or `build_debug.bat`.
- Output is in `build/bin/${CONFIG}/` directory.
