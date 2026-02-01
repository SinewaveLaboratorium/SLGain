# SLGain VST Audio Plugin

![License](https://img.shields.io/badge/license-Open%20Consultation%20Only-red) ![Platform](https://img.shields.io/badge/platform-Windows-green) ![Language](https://img.shields.io/badge/language-C++-orange)

This repository contains the core logic and architectural source code for **SLGain**, a gain-staging VST plugin.

> ⚠️ **Licensing Notice:** This project is **Source Available** for professional evaluation and portfolio review purposes only. It is not an Open Source project. Graphical assets, compiled binaries, and third-party framework dependencies have been excluded from this repository. Please refer to the `LICENSE` file for full terms.

## 🛠 Project Overview

**SLGain** is a utility plugin designed to provide precise control over signal levels. Unlike standard gain plugins, it allows users to define custom **Minimum** and **Maximum** gain ranges, making it an essential tool for automation and gain-staging within a DAW (Digital Audio Workstation).

![](https://sinewavelab.com/wp-content/uploads/2023/12/SLGAIN_VST.webp)

- **Commercial Product:** [sinewavelab.com/products/slgain-free-gain-vst/](https://sinewavelab.com/products/slgain-free-gain-vst/)
- **Framework:** Built using the **WDL-OL (IPlug)** framework.
- **Language:** C++

## 💻 Technical Highlights

This repository showcases the handling of specific challenges of real-time audio programming and Digital Signal Processing (DSP):

### 1. Custom DSP Signal Smoothing

Implementation of a **First-Order IIR Filter** (`CParamSmooth` class) to handle parameter changes. By calculating coefficients based on the sample rate ($a = e^{-2\pi \cdot F_c}$), the plugin ensures that gain adjustments are click-free and prevent "zipper noise" during real-time processing.

### 2. Non-Linear Knob Tapering

A power-curve algorithm (`findShape`) to map linear UI movements to logarithmic audio scales. This ensures the "feel" of the gain knob is consistent and intuitive, regardless of the user-defined dB range.

### 3. State Serialization & Thread Safety

- **Serialization:** State management (`SerializeState` and `UnserializeState`) to ensure the plugin's "memory" is preserved across DAW sessions.
- **Thread Safety:** The code utilizes **Mutex Locks** (`IMutexLock`) to ensure thread safety between the GUI thread and the high-priority audio processing thread.

## 📂 Repository Structure

- **`SLGain.h`**: The architectural core. It contains the DSP smoother class, custom UI control classes, and the mathematical logic for logarithmic knob tapering.
- **`SLGain.cpp`**: The implementation layer. It manages the plugin lifecycle, the real-time audio processing loop (`ProcessDoubleReplacing`), and parameter-to-GUI synchronization.
- **`LICENSE`**: The custom "Read-Only" license for portfolio review.

## 🚀 Build Dependencies

This repository is intended for code review. To compile a functional binary, the following dependencies are required:

- **WDL-OL Framework:** (zlib license).
- **Steinberg VST3 SDK:**.
- **Proprietary Resources:** Graphical assets (bitmaps) and `resource.h` definitions are excluded from this repository.
