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
    

    Z_out <<  X4, 0, 0, 0, 0, 0,
              0, X5, 0, 0, 0, 0,
              0, 0, Rin, 0, 0, 0,
              0, 0, 0, Z_C10, 0, 0,
              0, 0, 0, 0, R15, 0,
              0, 0, 0, 0, 0, Z_C11;
    
    
    //scattering matrix computation
    S_out = I - 2 * Z_out * B_out.transpose() * ((B_out * Z_out * B_out.transpose()).inverse() * B_out);
    
    
    return S_out;
    
}

float OutputStage::outputStageSample(float inputWet, Matrix7f S, wavesOUT& waves, int R, float tone, float volume)
{
    
    
    waves.b(2) = inputWet;      // Index 4 in MATLAB is 3 in C++ (0-indexed)
    waves.b(3) = a_init_C10; // Index 5 in MATLAB is 4 in C++ (0-indexed)
    waves.b(5) = a_init_C11; // Index 7 in MATLAB is 6 in C++ (0-indexed)

    
    
    if (i>20000){
        
        computeScatteringMatrix(R, tone, volume);
        DBG("tcacca " << tone);
        i=0;
    }
    
    
    i++;

    
    // Compute waves.a by multiplying with S_out matrix
    waves.a = S_out * waves.b;

    // Update values for a_init_C10 and a_init_C11 based on waves.a
    a_init_C10 = waves.a(3);  // Index 5 in MATLAB is 4 in C++ (0-indexed)
    a_init_C11 = waves.a(5);  // Index 7 in MATLAB is 6 in C++ (0-indexed)

    float outputSample = (waves.a(1) + waves.b(1)) / 2;
    
    return outputSample;
}

void OutputStage::computeScatteringMatrix(bool R, int tone, int volume){
    
    if (R==true){
        
        R15 = 33e3f;
        
    }else{
        
        R15= 13200;
        
    }
    
    Z_out <<  tone, 0, 0, 0, 0, 0,
              0, volume, 0, 0, 0, 0,
              0, 0, Rin, 0, 0, 0,
              0, 0, 0, Z_C10, 0, 0,
              0, 0, 0, 0, R15, 0,
              0, 0, 0, 0, 0, Z_C11;
    
    B_out << 1, 0, -1, 1, 1, 1,
            0, 1, -1, 1, 1, 0;
    
    //scattering matrix computation
    S_out = I - 2 * Z_out * B_out.transpose() * ((B_out * Z_out * B_out.transpose()).inverse() * B_out);
    
    
    
    
}
