/*
  ============================================================================
    MXRdist.
    Created: 15 Mar 2025 5:00:0
    Author:  Stefano Polime
  =============================================================================
*/
#include "MXRdist.h"
#include <iostream>
#include <cmath>
#include <array>
#include <vector>
#include <stdio.h>
#include "../../eigen-3.4.0/Eigen/Dense"
#include <juce_core/juce_core.h> // For juce::String and juce::Logger
using namespace juce;
using namespace Eigen;


MXRdist::MXRdist(){

  Bser << 1, 1, 1;
  Qpar << 1, 1, 1;
  Bser_t = Bser.transpose();
  Qpar_t = Qpar.transpose();

  I.fill(1);
  Sser1.fill(0);
  Spar1.fill(0);
  Spar2.fill(0);
}
    
    


MXRdist::~MXRdist() {}

void MXRdist::computeScatteringMatrices(float sampleRate) 
{
    Rin = 1;
    R1 = 1e4;
    Rout = 1e4;
    C1 = 1e-6;
    C2 = 1e-9;
    Ts = 1.0f / sampleRate;
    Z5 = Ts / (2 * C2);
    Z8 = Ts / (2 * C1);
    Z6 = Rout;
    Z9 = Rin + R1;
    Z4 = (Z5 * Z6) / (Z5 + Z6);
    Z1 = Z4;
    Z7 = Z8 + Z9;
    Z2 = Z7;
    Z3 = (Z1 * Z2) / (Z1 + Z2);


    // Define your Z matrices

    Zpar1 << Z1, 0, 0,
             0, Z2, 0,
             0, 0, Z3;


    Zpar2 << Z4, 0, 0,
             0, Z5, 0,
             0, 0, Z6;

 
    Zser1 << Z7, 0, 0,
             0, Z8, 0,
             0, 0, Z9;

    
    // Scattering matrix Sser1
    Sser1 = I - 2 * Zser1 * Bser_t * (1/(Bser * Zser1 * Bser_t)) * Bser;

    // Scattering matrix Spar1
    Spar1 = 2 * Qpar_t * (Qpar * Zpar1.inverse() * Qpar_t).inverse() * (Qpar * Zpar1.inverse()) - I;

    // Scattering matrix Spar2
    Spar2 = 2 * Qpar_t * (Qpar * Zpar2.inverse() * Qpar_t).inverse() * (Qpar * Zpar2.inverse()) - I;
    

}

float MXRdist::process(const float input) {
    AntiparallelDiodes diodes;


    // Simulation Algorithm
    a9 = input; // Replace with input[n] if processing multiple samples
    a5 = b5;
    a8 = b8;

    
    // Forward Scan
    b4 = Spar2(0, 0) * 0 + Spar2(0, 1) * a5 + Spar2(0, 2) * a6;
    b7 = Sser1(0, 0) * 0 + Sser1(0, 1) * a8 + Sser1(0, 2) * a9;
    a1 = b4;
    a2 = b7;
    b3 = Spar1(2, 0) * a1 + Spar1(2, 1) * a2 + Spar1(2, 2) * 0;

    // Local Root Scattering
    a3 = diodes.process(b3, Z3);

    // Backward Scan
    b1 = Spar1(0, 0) * a1 + Spar1(0, 1) * a2 + Spar1(0, 2) * a3;
    b2 = Spar1(1, 0) * a1 + Spar1(1, 1) * a2 + Spar1(1, 2) * a3;
    a4 = b1;
    a7 = b2;
    b5 = Spar2(1, 0) * a4 + Spar2(1, 1) * a5 + Spar2(1, 2) * a6;
    b8 = Sser1(1, 0) * a7 + Sser1(1, 1) * a8 + Sser1(1, 2) * a9;
    b6 = Spar2(2, 0) * a4 + Spar2(2, 1) * a5 + Spar2(2, 2) * a6;
    b9 = Sser1(2, 0) * a7 + Sser1(2, 1) * a8 + Sser1(2, 2) * a9;


    
    // Read Output
    vout = -(a6 + b6) / 2.0f;
    

    return vout; // Return the first sample output, modify as needed
}
