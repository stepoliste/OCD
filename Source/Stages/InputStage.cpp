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
    
    z(0) = Rin;
    z(1) = R3;
    z(2) = X2;
    z(3) = Z_C4;
    z(4) = R_L;
    z(5) = Z_C1;
    z(6) = R2;
    z(7) = R6;
    z(8) = R5;
    z(9) = Z_C6;
    z(10) = R8;

    Z = z.asDiagonal();

    // Scattering Matrix computation
    S_in = I - 2 * Z * Bi.transpose() *
    ((Bv * Z * Bi.transpose()).inverse() * Bv);
    
   
    return S_in;
    
}

float InputStage::inputStageSample(const float inputSample, Matrix11f S_in, wavesIN& waves, float drive)
{
    // Input stage
    waves.b(0) = inputSample;           // Vin(n) --> waves.b(0)
    waves.b(3) = a_init_C4;
    waves.b(5) = a_init_C1;
    waves.b(7) = V_ref;
    waves.b(9) = a_init_C6;

    if (i>20000){
        
        computeScatteringMatrix(drive);
        i=0;
    }
    
    
    waves.a = S_in * waves.b;
    i++;

    a_init_C1 = waves.a(5);
    a_init_C4 = waves.a(3);
    a_init_C6 = waves.a(9);

    
    float outputSample = (waves.a(4) + waves.b(4)) / 2;
    return outputSample;
}

void InputStage::computeScatteringMatrix(int drive){
    
    z(2) = drive;

    
    
    Z = z.asDiagonal();


    // Scattering Matrix computation
    S_in = I - 2 * Z * Bi.transpose() *
    ((Bv * Z * Bi.transpose()).inverse() * Bv);
    
    
    
}
