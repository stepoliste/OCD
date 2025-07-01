//
//  inputStage.cpp
//  Sfaser25 - VST3
//
//  Created by Stefano Polimeno 25/06/2025
//

#include "InputStage.h"
#include <JuceHeader.h>

InputStage::InputStage()
{
    
   
    S_in.fill(0);
}
void InputStage::prepareInputStage(float sampleRate)
{
    float Ts = 1.0f / sampleRate;
    
    Z_C1 = Ts / (2.0f * 22e-9f);
    Z_C4 = Ts / (2.0f * 100e-9f);
    Z_C6 = Ts / (2.0f * 220e-12f);
        
    // Initialize with default drive value
    computeScatteringMatrix(drive);
    
    // Initialize to -1 to force recomputation on first sample
    lastDrive = -1;
}


float InputStage::inputStageSample(const float inputSample, float drive)
{
    // Convert drive to int as required by computeScatteringMatrix
    int driveInt = static_cast<int>(drive);
    
    // Check if drive parameter has changed, and only recompute if necessary
    if (driveInt != lastDrive)
    {
        computeScatteringMatrix(driveInt);
        lastDrive = driveInt;
    }
    
    // Set active inputs
    const float b0 = inputSample;
    const float b3 = a_init_C4;
    const float b5 = a_init_C1;
    const float b7 = V_ref;
    const float b9 = a_init_C6;
    
    // Calculate only the rows we need using cached values
    // Row 3 - C4 state
    waves.a(3) = S_cache.S30 * b0 + 
                 S_cache.S33 * b3 + 
                 S_cache.S35 * b5 + 
                 S_cache.S37 * b7;
    
    // Row 4 - Output
    waves.a(4) = S_cache.S40 * b0 + 
                 S_cache.S43 * b3 + 
                 S_cache.S45 * b5 + 
                 S_cache.S47 * b7 + 
                 S_cache.S49 * b9;
    
    // Row 5 - C1 state
    waves.a(5) = S_cache.S50 * b0 + 
                 S_cache.S55 * b5 + 
                 S_cache.S57 * b7;
    
    // Row 9 - C6 state
    waves.a(9) = S_cache.S90 * b0 + 
                 S_cache.S93 * b3 + 
                 S_cache.S95 * b5 + 
                 S_cache.S97 * b7 + 
                 S_cache.S99 * b9;
    
    // Update state variables
    a_init_C1 = waves.a(5);
    a_init_C4 = waves.a(3);
    a_init_C6 = waves.a(9);
    
    // Calculate output sample
    float outputSample = waves.a(4) / 2.0f; // waves.b(4) is 0, so simplified
    
    return outputSample;
}


void InputStage::computeScatteringMatrix(int X2){
    // Common denominators for efficiency
    float denom_base = 1000000000001.0f * Z_C1 + 480000000001480000.0f;
    float denom_C4 = Z_C4 + 2200.0f;
    float denom_X2 = X2 + Z_C6 + 18000.0f;
    float denom_full = denom_C4 * denom_base * denom_X2;
    
    // Common terms
    float term_940M = 940000000000000000.0f;
    float term_940K = 940000.0f;
    float term_2M = 2000000.0f;
    float term_20B = 20000000000000000.0f;
    float term_Z_C1_base = 1000000000001.0f * Z_C1 + 10000000001010000.0f;
    float term_X2_18k = X2 + 18000.0f;
    
    // Only compute the cache values we need
    
    // Row 3 - C4 state
    S_cache.S30 = (term_940M * Z_C4) / (denom_C4 * denom_base);
    S_cache.S31 = -(term_940M * Z_C4 + term_940K * Z_C4) / (denom_C4 * denom_base);
    S_cache.S33 = -(Z_C4 - 2200.0f) / denom_C4;
    S_cache.S35 = -(term_940M * Z_C4 + term_940K * Z_C4) / (denom_C4 * denom_base);
    S_cache.S36 = (term_940K * Z_C4) / (denom_C4 * denom_base);
    S_cache.S37 = (2.0f * Z_C4 * term_Z_C1_base) / (denom_C4 * denom_base);
    S_cache.S38 = -(2.0f * Z_C4) / denom_C4;
    
    // Row 4 - Output (Complex term dependent on X2, Z_C4, Z_C6)
    float complex_term = 2200.0f * X2 + 18000.0f * Z_C4 + 20200.0f * Z_C6 +
                       X2 * Z_C4 + X2 * Z_C6 + Z_C4 * Z_C6 + 39600000.0f;
    
    S_cache.S40 = (term_940M * complex_term) / denom_full;
    S_cache.S41 = -(term_940M * complex_term + term_940K * complex_term) / denom_full;
    S_cache.S42 = -(2.0f * Z_C6) / denom_X2;
    S_cache.S43 = -(2.0f * Z_C6 * term_X2_18k) / (denom_C4 * denom_X2);
    S_cache.S45 = -(term_940M * complex_term + term_940K * complex_term) / denom_full;
    S_cache.S46 = (term_940K * complex_term) / denom_full;
    S_cache.S47 = (2.0f * term_Z_C1_base * complex_term) / denom_full;
    S_cache.S48 = -(2.0f * Z_C6 * term_X2_18k) / (denom_C4 * denom_X2);
    S_cache.S49 = (2.0f * term_X2_18k) / denom_X2;
    S_cache.S410 = -(2.0f * Z_C6) / denom_X2;
    
    // Row 5 - C1 state
    S_cache.S50 = (2000000000000.0f * Z_C1) / denom_base;
    S_cache.S51 = -(2000000000002.0f * Z_C1) / denom_base;
    S_cache.S55 = 1.0f - (2000000000002.0f * Z_C1) / denom_base;
    S_cache.S56 = (2.0f * Z_C1) / denom_base;
    S_cache.S57 = -(2000000000002.0f * Z_C1) / denom_base;
    
    // Row 9 - C6 state
    S_cache.S90 = (term_940M * Z_C6 * term_X2_18k) / denom_full;
    S_cache.S91 = -(term_940M * Z_C6 * term_X2_18k + term_940K * Z_C6 * term_X2_18k) / denom_full;
    S_cache.S92 = -(2.0f * Z_C6) / denom_X2;
    S_cache.S93 = -(2.0f * Z_C6 * term_X2_18k) / (denom_C4 * denom_X2);
    S_cache.S95 = -(term_940M * Z_C6 * term_X2_18k + term_940K * Z_C6 * term_X2_18k) / denom_full;
    S_cache.S96 = (term_940K * Z_C6 * term_X2_18k) / denom_full;
    S_cache.S97 = (2.0f * Z_C6 * term_Z_C1_base * term_X2_18k) / denom_full;
    S_cache.S98 = -(2.0f * Z_C6 * term_X2_18k) / (denom_C4 * denom_X2);
    S_cache.S99 = 1.0f - (2.0f * Z_C6) / denom_X2;
    S_cache.S910 = -(2.0f * Z_C6) / denom_X2;
}