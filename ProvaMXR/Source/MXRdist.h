/*
  ==============================================================================

    MXRdist.h
    Created: 15 Mar 2025 4:59:34pm
    Author:  Stefano Polimeno

  ==============================================================================
*/

#pragma once

#include "AntiparallelDiodes.h"
#include <iostream>
#include <stdio.h>
#include "../../eigen-3.4.0/Eigen/Dense"
using namespace Eigen;

class MXRdist {
public:
    MXRdist();
    ~MXRdist();
    void computeScatteringMatrices(float sampleRate);
    float process(const float input);

private:
    
    Matrix<float, 3, 3> Zpar1, Zpar2, Zser1, Sser1, Spar1, Spar2, I;
    Matrix<float, 1, 3> Bser, Qpar;
    Matrix<float, 3, 1> Bser_t, Qpar_t;
    float Ts;
    float Rin, R1, Rout, C1, C2;
    float Z1, Z2, Z3, Z4, Z5, Z6, Z7, Z8, Z9;
    // Declaration of variables
    float a1 = 0.0f, a2 = 0.0f, a3 = 0.0f, a4 = 0.0f, a5 = 0.0f,a6 = 0.0f, a7 = 0.0f, a8 = 0.0f, a9 = 0.0f,b1 = 0.0f, b2 = 0.0f, b3 = 0.0f, b4 = 0.0f, b5 = 0.0f, b6 = 0.0f, b7 = 0.0f, b8 = 0.0f, b9 = 0.0f, vout =0.0f;

};

