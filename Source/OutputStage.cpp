/*
  ==============================================================================

    outputStage.cpp
    Created: 1 May 2023 2:37:54pm
    Author:  Samuele Del Moro

  ==============================================================================
*/

#include "OutputStage.h"

OutputStage::OutputStage()
{

    B_out << 1, 0, 0, 0, 0, -1, 0,
            0, 1, 0, -1, 1, 1, 1,
            0, 0, 1, -1, 1, 1, 0;
    S_out.fill(0);
}

Matrix7f OutputStage::prepareOutputStage(float sampleRate)
{
    
    float Ts = 1/sampleRate;
    float Z_C10 = Ts / (2 * 10e-6f);
    float Z_C11 = Ts / (2 * 47e-9f);
    
    Eigen::Matrix<float, 7, 7> Z_out;

    Z_out << R14, 0, 0, 0, 0, 0, 0,
             0, X4, 0, 0, 0, 0, 0,
             0, 0, X5, 0, 0, 0, 0,
             0, 0, 0, Rin, 0, 0, 0,
             0, 0, 0, 0, Z_C10, 0, 0,
             0, 0, 0, 0, 0, R15, 0,
             0, 0, 0, 0, 0, 0, Z_C11;
    
    //scattering matrix computation
    S_out = I - 2 * Z_out * B_out.transpose() * ((B_out * Z_out * B_out.transpose()).inverse() * B_out);
    
    std::cout << "S_in matrix:" << std::endl;
    std::cout << S_out.format(Eigen::IOFormat(Eigen::FullPrecision, 0, ", ", "\n", "[", "]")) << std::endl;
        
    
    return S_out;
    
}

float OutputStage::outputStageSample(float inputWet, Matrix7f S, wavesOUT& waves)
{
    waves.b(3) = inputWet;      // Index 4 in MATLAB is 3 in C++ (0-indexed)
    waves.b(4) = a_init_C10; // Index 5 in MATLAB is 4 in C++ (0-indexed)
    waves.b(6) = a_init_C11; // Index 7 in MATLAB is 6 in C++ (0-indexed)

    // Compute waves.a by multiplying with S_out matrix
    waves.a = S_out * waves.b;

    // Update values for a_init_C10 and a_init_C11 based on waves.a
    a_init_C10 = waves.a(4);  // Index 5 in MATLAB is 4 in C++ (0-indexed)
    a_init_C11 = waves.a(6);  // Index 7 in MATLAB is 6 in C++ (0-indexed)

    float outputSample = (waves.a(2) + waves.b(2)) / 2;
    return outputSample;
}


