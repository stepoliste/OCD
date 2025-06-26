# Wave Digital Modeling (MATLAB)

This branch contains the MATLAB implementation of the wave digital (WD) model for a nonlinear analog audio circuit. The main focus is on the formulation, symbolic manipulation, and simulation of the WD structures.

## Project Description

The code implements two different modeling strategies based on WD principles. Both models emulate the behavior of a nonlinear circuit and have been quantitatively validated against a high-fidelity reference obtained through Simscape simulation.

Results show that both approaches reproduce the reference behavior with high accuracy, with different trade-offs in modularity and computational performance.

## Structure

- Symbolic derivations and matrix structures
- WD scattering equations and nonlinear solvers
- Simscape reference comparison and error analysis

## Usage

Open the main script in MATLAB to run simulations and visualize results. No special toolboxes are required beyond the standard Symbolic Math Toolbox.

## Results

The output waveforms match closely with the reference, demonstrating the correctness and robustness of the WD modeling approach.

## Notes

Please refer to the `paper.pdf` for a complete explanation of the mathematical background and modeling strategies.
