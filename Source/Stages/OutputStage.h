/*
  ==============================================================================

    outputStage.h
    Created: 1 May 2023 2:37:54pm
    Author:  Samuele Del Moro

  ==============================================================================
*/
#include "../../eigen-3.4.0/Eigen/Dense"
#include <stdio.h>
#include <iostream>

#pragma once

using namespace Eigen;


struct wavesOUT
{
    Matrix<float, 6, 1> a = {0,0,0,0,0,0};
    Matrix<float, 6, 1> b = {0,0,0,0,0,0};
};

typedef Matrix<float,6,6> Matrix7f;

class OutputStage {
public:
    OutputStage();
    ~OutputStage() {};
    Matrix<float, 6, 6> prepareOutputStage(float sampleRate);
    float outputStageSample(float inputWet, Matrix7f S_out, wavesOUT& waves, int R, float tone, float volume);
    void computeScatteringMatrix(bool R, int tone, int volume);

private:
    
    float Ts;
    float Z_C10;
    float Z_C11;
    float Rin = 1e-6;
    float R14 = 22e3f;
    float R15 = 33e3f;
    float X4  = 10e3f;
    float X5  = 500e3f;
    int i=0;
    
    float a_init_C10 = 0.0f;
    float a_init_C11 = 0.0f;
    
    Matrix<float, 2, 6> B_out;
    Matrix<float, 6, 6> Z_out;
    Matrix7f I=Matrix7f::Identity(6,6);
    
    Matrix7f S_out;
};
