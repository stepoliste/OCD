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
    Matrix<float, 7, 1> a = {0,0,0,0,0,0,0};
    Matrix<float, 7, 1> b = {0,0,0,0,0,0,0};
};

typedef Matrix<float,7,7> Matrix7f;

class OutputStage {
public:
    OutputStage();
    ~OutputStage() {};
    Matrix7f prepareOutputStage(float sampleRate);
    float outputStageSample(float inputWet, Matrix7f S_out, wavesOUT& waves);

private:
    
    float Rin = 1e-6;
    float R14 = 22e3f;
    float R15 = 33e3f;
    float X4  = 10e3f;
    float X5  = 500e3f;
    
    float a_init_C10 = 0.0f;
    float a_init_C11 = 0.0f;
    
    Matrix<float, 3, 7> B_out;
   
    Matrix7f I=Matrix7f::Identity(7,7);


    Matrix7f S_out;
};
