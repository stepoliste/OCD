/*
  ==============================================================================

    outputStage.cpp
    Created: 1 May 2025 2:37:54pm
    Author:  Stefano Polimeno

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

void OutputStage::prepareOutputStage(float sampleRate)
{
    
    Ts = 1/sampleRate;
    Z_C10 = Ts / (2 * 10e-6f);
    Z_C11 = Ts / (2 * 47e-9f);
    
    // Precompute with initial values
    computeScatteringMatrix(true, X4, X5);
    
    // Initialize last parameter values to force recalculation on first sample
    lastR = -1;
    lastTone = -1.0f;
    lastVolume = -1.0f;
    

}

/*
float OutputStage::outputStageSample(float inputWet, int R, float tone, float volume)
{
    // Check if any parameter has changed, and only recompute if necessary
    if (R != lastR || tone != lastTone || volume != lastVolume)
    {
        computeScatteringMatrix(R, tone, volume);
        
        // Update last parameter values
        lastR = R;
        lastTone = tone;
        lastVolume = volume;
    }
    
    waves.b(2) = inputWet;      // Index 4 in MATLAB is 3 in C++ (0-indexed)
    waves.b(3) = a_init_C10; // Index 5 in MATLAB is 4 in C++ (0-indexed)
    waves.b(5) = a_init_C11; // Index 7 in MATLAB is 6 in C++ (0-indexed)


    
    // Compute waves.a by multiplying with S_out matrix
    waves.a = S_out * waves.b;

    // Update values for a_init_C10 and a_init_C11 based on waves.a
    a_init_C10 = waves.a(3);  // Index 5 in MATLAB is 4 in C++ (0-indexed)
    a_init_C11 = waves.a(5);  // Index 7 in MATLAB is 6 in C++ (0-indexed)

    float outputSample = (waves.a(1) + waves.b(1)) / 2;
    
    return outputSample; // Return the processed sample
}
*/

float OutputStage::outputStageSample(float inputWet, int R, float tone, float volume)
{
    // Check if parameters changed
    if (R != lastR || tone != lastTone || volume != lastVolume)
    {
        computeScatteringMatrix(R, tone, volume);
        lastR = R;
        lastTone = tone;
        lastVolume = volume;
    }
    
    
    // Set active inputs
    waves.b(2) = inputWet;
    waves.b(3) = a_init_C10;
    waves.b(5) = a_init_C11;
    
    // Set active inputs
    waves.b(2) = inputWet;
    waves.b(3) = a_init_C10;
    waves.b(5) = a_init_C11;
    
    // Calculate only the rows we need using cached values
    // Row 1 - output
    waves.a(1) = S_cache.S12 * inputWet + 
                 S_cache.S13 * a_init_C10 +
                 S_cache.S15 * a_init_C11;
    
    // Row 3 - C10 state
    waves.a(3) = S_cache.S32 * inputWet + 
                 S_cache.S33 * a_init_C10 +
                 S_cache.S35 * a_init_C11;
    
    // Row 5 - C11 state
    waves.a(5) = S_cache.S52 * inputWet + 
                 S_cache.S53 * a_init_C10 +
                 S_cache.S55 * a_init_C11;
    
    // Update capacitor states
    a_init_C10 = waves.a(3);
    a_init_C11 = waves.a(5);
    
    // Since waves.b(1) is 0, we can simplify
    float outputSample = waves.a(1) / 2.0f;
    
    return outputSample;
}
void OutputStage::computeScatteringMatrix(bool R, int X4, int X5)
{
    if (R == true) {
        R15 = 13200;
    } else {
        R15 = 33e3f;
    }

    // Common denominator for all elements
    float denom = X4 + X5 + Z_C11 + 1000000.0f * R15 * X4 + 1000000.0f * R15 * X5 +
                  1000000.0f * R15 * Z_C11 + 1000000.0f * X4 * X5 + 1000000.0f * X4 * Z_C10 +
                  1000000.0f * X5 * Z_C10 + 1000000.0f * X5 * Z_C11 + 1000000.0f * Z_C10 * Z_C11;
    
    // Compute only the elements we need directly into the cache
    
    // For row 1 (indices 1,2 1,3 1,5)
    float term_X4_Z_C11 = X4 + Z_C11;
    float term_R15_Z_C10_1 = 1000000.0f * R15 + 1000000.0f * Z_C10 + 1.0f;
    
    S_cache.S12 = (2000000.0f * X5 * term_X4_Z_C11) / denom;
    S_cache.S13 = -(2000000.0f * X5 * term_X4_Z_C11) / denom;
    S_cache.S15 = (2.0f * X5 * term_R15_Z_C10_1) / denom;
    
    // For row 3 (indices 3,2 3,3 3,5)
    float term_X4_X5_Z_C11 = X4 + X5 + Z_C11;
    
    S_cache.S32 = (2000000.0f * Z_C10 * term_X4_X5_Z_C11) / denom;
    S_cache.S33 = (X4 + X5 + Z_C11 + 1000000.0f * R15 * X4 + 1000000.0f * R15 * X5 +
               1000000.0f * R15 * Z_C11 + 1000000.0f * X4 * X5 - 1000000.0f * X4 * Z_C10 -
               1000000.0f * X5 * Z_C10 + 1000000.0f * X5 * Z_C11 - 1000000.0f * Z_C10 * Z_C11) / denom;
    S_cache.S35 = -(2000000.0f * X5 * Z_C10) / denom;
    
    // For row 5 (indices 5,2 5,3 5,5)
    float term_R15_X5_Z_C10_1 = 1000000.0f * R15 + 1000000.0f * X5 + 1000000.0f * Z_C10 + 1.0f;
    
    S_cache.S52 = (2000000.0f * X5 * Z_C11) / denom;
    S_cache.S53 = -(2000000.0f * X5 * Z_C11) / denom;
    S_cache.S55 = 1.0f - (2.0f * Z_C11 * term_R15_X5_Z_C10_1) / denom;
}