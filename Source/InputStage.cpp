//
//  inputStage.cpp
//  Sfaser25 - VST3
//
//  Created by Samuele Del Moro on 30/04/23.
//

#include "InputStage.h"
#include <JuceHeader.h>

InputStage::InputStage()
{
    
    Bv << 1, 0, 0, 0, 0, 0,-1, 0, 0, 0, 0,
          0, 1, 0, 0, 0, 1,-1, 1, 0, 0, 0,
          0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1,
          0, 0, 0, 1, 0, 0, 0,-1, 1, 0, 0,
          0, 0, 0, 0, 1, 0, 0,-1, 0,-1, 0;
    
    Bi << 1, 0, 0, 0, 0, 0,-1, 0, 0, 0, 0,
          0, 1, 0, 0, 0, 1,-1, 1, 0, 0, 0,
          0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1,
          0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0,
          0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0;

    I = Matrix<float, 11, 11>::Identity();
    S_in.fill(0);
}

Matrix11f InputStage::prepareInputStage(float sampleRate)
{      
    float Ts = 1/sampleRate; 
    float Z_C1 = Ts / (2 * 22e-9);
    float Z_C4 = Ts / (2 * 100e-9);
    float Z_C6 = Ts / (2 * 220e-12);
    
    Vector11f z(Rin, R3, X2, Z_C4, R_L, Z_C1, R2, R6, R5, Z_C6, R8);

    // Diagonal Z
    Matrix11f Z = z.asDiagonal();

    // Scattering Matrix computation
    S_in = I - 2 * Z * Bi.transpose() *
    ((Bv * Z * Bi.transpose()).inverse() * Bv);
    
   
    return S_in;
    
}

float InputStage::inputStageSample(const float inputSample, Matrix11f S_in, wavesIN& waves)
{
    // Input stage
    waves.b(0) = inputSample;           // Vin(n) --> waves.b(0)
    waves.b(3) = a_init_C4;
    waves.b(5) = a_init_C1;
    waves.b(7) = V_ref;
    waves.b(9) = a_init_C6;

    //computeScatteringMatrix(drive);
    
    waves.a = S_in * waves.b;

    a_init_C1 = waves.a(5);
    a_init_C4 = waves.a(3);
    a_init_C6 = waves.a(9);

    
    float outputSample = (waves.a(4) + waves.b(4)) / 2;
    return outputSample;
}

void InputStage::computeScatteringMatrix(int drive){
    
    X2 = drive;
    DBG("X2 = " + juce::String(X2));
    
    Vector11f z(Rin, R3, X2, Z_C4, R_L, Z_C1, R2, R6, R5, Z_C6, R8);

    // Diagonal Z
    Matrix11f Z = z.asDiagonal();

    // Scattering Matrix computation
    S_in = I - 2 * Z * Bi.transpose() *
    ((Bv * Z * Bi.transpose()).inverse() * Bv);
    
}
