/*
  ==============================================================================

    outputStage.cpp
    Created: 1 May 2023 2:37:54pm
    Author:  Samuele Del Moro

  ==============================================================================
*/

#include "OutputStage.h"
#include <JuceHeader.h>

OutputStage::OutputStage()
{

    B_out << 1, 0, -1, 1, 1, 1,
            0, 1, -1, 1, 1, 0;
    
    S_out.fill(0);
}

Matrix<float, 6, 6> OutputStage::prepareOutputStage(float sampleRate)
{
    
    Ts = 1/sampleRate;
    Z_C10 = Ts / (2 * 10e-6f);
    Z_C11 = Ts / (2 * 47e-9f);
    

    /*
    Z_out <<  X4, 0, 0, 0, 0, 0,
              0, X5, 0, 0, 0, 0,
              0, 0, Rin, 0, 0, 0,
              0, 0, 0, Z_C10, 0, 0,
              0, 0, 0, 0, R15, 0,
              0, 0, 0, 0, 0, Z_C11;
    
    
    //scattering matrix computation
    S_out = I - 2 * Z_out * B_out.transpose() * ((B_out * Z_out * B_out.transpose()).inverse() * B_out);
     
     */
    int R=true;
    computeScatteringMatrix(R, X4, X5);
    
    
    
    return S_out;
    
}

float OutputStage::outputStageSample(float inputWet, Matrix6f S_out, wavesOUT& waves, int R, float tone, float volume)
{
    
    
    waves.b(2) = inputWet;      // Index 4 in MATLAB is 3 in C++ (0-indexed)
    waves.b(3) = a_init_C10; // Index 5 in MATLAB is 4 in C++ (0-indexed)
    waves.b(5) = a_init_C11; // Index 7 in MATLAB is 6 in C++ (0-indexed)

    /*
    
    if (i>10000){
        
        computeScatteringMatrix(R, tone, volume);
        DBG("tone " << tone);
        i=0;
    }
    
    
    i++;

     
     */
    
    // Compute waves.a by multiplying with S_out matrix
    waves.a = S_out * waves.b;

    // Update values for a_init_C10 and a_init_C11 based on waves.a
    a_init_C10 = waves.a(3);  // Index 5 in MATLAB is 4 in C++ (0-indexed)
    a_init_C11 = waves.a(5);  // Index 7 in MATLAB is 6 in C++ (0-indexed)

    float outputSample = (waves.a(1) + waves.b(1)) / 2;
    
    return outputSample;
}

void OutputStage::computeScatteringMatrix(bool R, int X4, int X5){
    
    if (R==true){
        
        R15 = 13200;
        
    }else{
        
        R15= 33e3f;
        
    }

    // Common denominator for all elements
    float denom = X4 + X5 + Z_C11 + 1000000.0f * R15 * X4 + 1000000.0f * R15 * X5 + 
                  1000000.0f * R15 * Z_C11 + 1000000.0f * X4 * X5 + 1000000.0f * X4 * Z_C10 + 
                  1000000.0f * X5 * Z_C10 + 1000000.0f * X5 * Z_C11 + 1000000.0f * Z_C10 * Z_C11;
    
    // Common terms for efficiency
    float term_R15_X5_Z_C10_1 = 1000000.0f * R15 + 1000000.0f * X5 + 1000000.0f * Z_C10 + 1.0f;
    float term_R15_Z_C10_1 = 1000000.0f * R15 + 1000000.0f * Z_C10 + 1.0f;
    float term_R15_X4_Z_C10_Z_C11_1 = 1000000.0f * R15 + 1000000.0f * X4 + 1000000.0f * Z_C10 + 1000000.0f * Z_C11 + 1.0f;
    float term_X4_Z_C11 = X4 + Z_C11;
    float term_X4_X5_Z_C11 = X4 + X5 + Z_C11;
    
    // Row 0
    S_out(0, 0) = 1.0f - (2.0f * X4 * term_R15_X5_Z_C10_1) / denom;
    S_out(0, 1) = (2.0f * X4 * term_R15_Z_C10_1) / denom;
    S_out(0, 2) = (2000000.0f * X4 * X5) / denom;
    S_out(0, 3) = -(2000000.0f * X4 * X5) / denom;
    S_out(0, 4) = -(2000000.0f * X4 * X5) / denom;
    S_out(0, 5) = -(2.0f * X4 * term_R15_X5_Z_C10_1) / denom;
    
    // Row 1
    S_out(1, 0) = (2.0f * X5 * term_R15_Z_C10_1) / denom;
    S_out(1, 1) = 1.0f - (2.0f * X5 * term_R15_X4_Z_C10_Z_C11_1) / denom;
    S_out(1, 2) = (2000000.0f * X5 * term_X4_Z_C11) / denom;
    S_out(1, 3) = -(2000000.0f * X5 * term_X4_Z_C11) / denom;
    S_out(1, 4) = -(2000000.0f * X5 * term_X4_Z_C11) / denom;
    S_out(1, 5) = (2.0f * X5 * term_R15_Z_C10_1) / denom;
    
    // Row 2
    S_out(2, 0) = (2.0f * X5) / denom;
    S_out(2, 1) = (2.0f * term_X4_Z_C11) / denom;
    S_out(2, 2) = (1000000.0f * R15 * X4 - X5 - Z_C11 - X4 + 1000000.0f * R15 * X5 + 
               1000000.0f * R15 * Z_C11 + 1000000.0f * X4 * X5 + 1000000.0f * X4 * Z_C10 + 
               1000000.0f * X5 * Z_C10 + 1000000.0f * X5 * Z_C11 + 1000000.0f * Z_C10 * Z_C11) / denom;
    S_out(2, 3) = (2.0f * term_X4_X5_Z_C11) / denom;
    S_out(2, 4) = (2.0f * term_X4_X5_Z_C11) / denom;
    S_out(2, 5) = (2.0f * X5) / denom;
    
    // Row 3
    S_out(3, 0) = -(2000000.0f * X5 * Z_C10) / denom;
    S_out(3, 1) = -(2000000.0f * Z_C10 * term_X4_Z_C11) / denom;
    S_out(3, 2) = (2000000.0f * Z_C10 * term_X4_X5_Z_C11) / denom;
    S_out(3, 3) = (X4 + X5 + Z_C11 + 1000000.0f * R15 * X4 + 1000000.0f * R15 * X5 + 
               1000000.0f * R15 * Z_C11 + 1000000.0f * X4 * X5 - 1000000.0f * X4 * Z_C10 - 
               1000000.0f * X5 * Z_C10 + 1000000.0f * X5 * Z_C11 - 1000000.0f * Z_C10 * Z_C11) / denom;
    S_out(3, 4) = -(2000000.0f * Z_C10 * term_X4_X5_Z_C11) / denom;
    S_out(3, 5) = -(2000000.0f * X5 * Z_C10) / denom;
    
    // Row 4
    S_out(4, 0) = -(2000000.0f * R15 * X5) / denom;
    S_out(4, 1) = -(2000000.0f * R15 * term_X4_Z_C11) / denom;
    S_out(4, 2) = (2000000.0f * R15 * term_X4_X5_Z_C11) / denom;
    S_out(4, 3) = -(2000000.0f * R15 * term_X4_X5_Z_C11) / denom;
    S_out(4, 4) = (X4 + X5 + Z_C11 - 1000000.0f * R15 * X4 - 1000000.0f * R15 * X5 - 
               1000000.0f * R15 * Z_C11 + 1000000.0f * X4 * X5 + 1000000.0f * X4 * Z_C10 + 
               1000000.0f * X5 * Z_C10 + 1000000.0f * X5 * Z_C11 + 1000000.0f * Z_C10 * Z_C11) / denom;
    S_out(4, 5) = -(2000000.0f * R15 * X5) / denom;
    
    // Row 5
    S_out(5, 0) = -(2.0f * Z_C11 * term_R15_X5_Z_C10_1) / denom;
    S_out(5, 1) = (2.0f * Z_C11 * term_R15_Z_C10_1) / denom;
    S_out(5, 2) = (2000000.0f * X5 * Z_C11) / denom;
    S_out(5, 3) = -(2000000.0f * X5 * Z_C11) / denom;
    S_out(5, 4) = -(2000000.0f * X5 * Z_C11) / denom;
    S_out(5, 5) = 1.0f - (2.0f * Z_C11 * term_R15_X5_Z_C10_1) / denom;
    
    
}
