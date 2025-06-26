/*
  ==============================================================================

    outputStage.h
    Created: 1 May 2023 2:37:54pm
    Author:  Stefano Polimeno

  ==============================================================================
*/
#include "../../eigen-3.4.0/Eigen/Dense"
#include <stdio.h>
#include <iostream>

#pragma once

using namespace Eigen;



typedef Matrix<float,6,6> Matrix6f;

class OutputStage {
public:
    OutputStage();
    ~OutputStage() {};
    void prepareOutputStage(float sampleRate);
    float outputStageSample(float inputWet,  int R, float tone, float volume);
    void computeScatteringMatrix(bool R, int tone, int volume);

private:
  

    struct wavesOUT {
        Eigen::Matrix<float, 6, 1> a;
        Eigen::Matrix<float, 6, 1> b;
        
        wavesOUT() {
            a.setZero();
            b.setZero();
        }
    };

    wavesOUT waves;

    float Ts;
    float Z_C10;
    float Z_C11;
    float Rin = 1e-6;
    float R14 = 22e3f;
    float R15 = 13200;//33e3f;
    float X4  = 10e3f;
    float X5  = 500e3f;
    int i=0;
    
    float a_init_C10 = 0.0f;
    float a_init_C11 = 0.0f;
    
    // Track last parameter values to avoid unnecessary recalculation
    int lastR = -1;
    float lastTone = -1.0f;
    float lastVolume = -1.0f;
    
    Matrix<float, 2, 6> B_out;
    Matrix<float, 6, 6> Z_out;
    Matrix6f I=Matrix6f::Identity(6,6);
    
    Matrix6f S_out;
};
