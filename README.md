# Real-Time VST Plugin (JUCE-based)

This branch contains the implementation of a real-time VST plugin that integrates the wave digital (WD) model developed in the MATLAB branch. The plugin is designed to demonstrate the practical applicability of the model in audio production environments.

## Plugin Description

The plugin emulates the nonlinear behavior of the reference analog circuit using the precomputed WD structure. It supports real-time audio input/output and parameter control.

> ⚠️ The plugin is still under refinement. Performance tuning and testing on different DAWs are ongoing.

## GUI Preview

![GUI Preview](https://github.com/stepoliste/OCD/blob/JUCE/gui_screen.png)

## Requirements

- **Operating System**
  - Windows 10 or later (64-bit)
  - macOS 10.14 “Mojave” or later

- **Plugin Format**
  - VST3 (compatible with most modern digital audio workstations)

- **DAW Compatibility**
1. **Clone the repository (optional, for developers):**
   ```bash
   git clone --branch JUCE https://github.com/stepoliste/OCD.git
   
2. Locate the Built Plugin

After building the plugin using JUCE's Projucer or an IDE like Xcode or Visual Studio, you will find the compiled plugin file here:
OCD/builds/OCD.vst3

3. Install the Plugin Manually

#### On Windows

- Copy the file `OCD.vst3` into your system VST3 folder:
  C:\Users<YourUsername>\AppData\Common\VST3\

  
- If the `VST3` folder doesn't exist, you can create it manually.

- Restart your DAW and rescan plugins if necessary.

#### On macOS

- Copy the `OCD.vst3` plugin bundle into the system-wide VST3 directory:
  /Users/<YourUsername>/Library/Audio/Plug-Ins/VST3/


- To access the `Library` folder, open Finder, press **Cmd + Shift + G**, and enter the path manually.

- Restart your DAW to detect the new plugin.

---

4. Insert the Plugin in Your DAW

- Load **"OCD"** onto an audio track or effects bus.
- Adjust the parameters to shape your sound.

---

## 🎚️ Controls

- **Drive** – Sets the amount of distortion/saturation applied to the signal.
- **Tone** – Adjusts brightness via a high-frequency filter.
- **Level** – Controls the output gain after distortion.

---

> ⚠️ **Note:**  
> This version of the plugin is functional but not optimized.  
> While the audio processing code is correct, it is currently inefficient and results in high CPU usage, which may prevent real-time operation in some environments.  
> Future updates will focus on improving performance for smoother live use.
