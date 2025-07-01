/*
  ==============================================================================

    DistStage.h
    Created: 23 May 2025 2:24:08pm
    Author:  Stefano Polimeno

  ==============================================================================
*/

#pragma once
#include "../../eigen-3.4.0/Eigen/Dense"
#include <stdio.h>
#include <iostream>

using namespace Eigen;
using Vector11f = Eigen::Matrix<float, 11, 1>;
using Matrix11f = Eigen::Matrix<float, 11, 11>;




class DistStage {

public:

    DistStage();
    ~DistStage() {}
    void prepareDistStage(float sampleRate);
    float DistStageSample(const float DistSample);
   

private:
    struct wavesDist {
        Vector11f a;
        Vector11f b;

        wavesDist() {
            a.setZero();
            b.setZero();
        }
    };

    // Add these member variables
    struct S_cache{
        float S10, S13, S14, S16, S18;  // Row 1 coefficients
        float S30, S31, S33, S36, S38;  // Row 3 coefficients 
        float S40, S41, S43, S44, S46, S48;  // Row 4 coefficients
        float S50, S51, S53, S54, S56, S58;  // Row 5 coefficients
        float S60, S61, S62, S66, S68;  // Row 6 coefficients
    } ;

    S_cache S_cache;
    wavesDist waves;
    float R9 = 10e3;
    float R10 = 10e3;
    float R12 = 220e3;
    float R11 = 39e3;
    float R13 = 150e3;

    float Z_C7 = 0;
    float Z_C8 = 0;
    float Z_C9 = 0;

    float Rin = 1e-6;
    float NL_adapt =493.986254297973;
    float R_L = 1000000;

    int i= 0;
    

    float a_init_C7 = 0.0f;
    float a_init_C8 = 0.0f;
    float a_init_C9 = 0.0f;
    float V_ref = 4.5f;
    

    void computeScatteringMatrix(float Z_C7,float Z_C8,float Z_C9);

    float CPWL_function(float a);

    
    Eigen::Matrix<float, 1, 42> aVect;
    
    Eigen::Matrix<float, 1, 42> etaVect;

    Matrix<float, 11, 11> S_dist;
    Matrix<float, 11, 11> I;

    Eigen::Matrix<float, 1, 42> absDiff;
    
    int N=42;
    int idx=0;
    float mu0=-0.984741527402546f;
    float mu1=-0.995080545246249f;

};
