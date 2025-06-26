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
        
    // Initialize the scattering matrix S_in
    computeScatteringMatrix(drive);

    
}

float InputStage::inputStageSample(const float inputSample, float drive)
{
    // Convert drive to int as required by computeScatteringMatrix
    int driveInt = static_cast<int>(drive);
    
    // Check if drive parameter has changed, and only recompute if necessary
    if (driveInt != lastDrive)
    {
        computeScatteringMatrix(driveInt);
        
        // Update last drive value
        lastDrive = driveInt;
    }
    
    // Input stage
    waves.b(0) = inputSample;           // Vin(n) --> waves.b(0)
    waves.b(3) = a_init_C4;
    waves.b(5) = a_init_C1;
    waves.b(7) = V_ref;
    waves.b(9) = a_init_C6;

    /*
    if (i>10000){
        
        computeScatteringMatrix(drive);
        i=0;
    }

    i++;
     */
    
    waves.a = S_in * waves.b;

    a_init_C1 = waves.a(5);
    a_init_C4 = waves.a(3);
    a_init_C6 = waves.a(9);

    
    float outputSample = (waves.a(4) + waves.b(4)) / 2;
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
    float term_20K = 20000.0f;
    
    // Row 0
    S_in(0, 0) = 1.0f - (2.0f * (Z_C1 + 1480000.0f)) / denom_base;
    S_in(0, 1) = term_2M / denom_base;
    S_in(0, 2) = 0.0f;
    S_in(0, 3) = 0.0f;
    S_in(0, 4) = 0.0f;
    S_in(0, 5) = term_2M / denom_base;
    S_in(0, 6) = (2.0f * (Z_C1 + 480000.0f)) / denom_base;
    S_in(0, 7) = term_2M / denom_base;
    S_in(0, 8) = 0.0f;
    S_in(0, 9) = 0.0f;
    S_in(0, 10) = 0.0f;
    
    // Row 1
    S_in(1, 0) = term_20B / denom_base;
    S_in(1, 1) = 1.0f - (term_20B + term_20K) / denom_base;
    S_in(1, 2) = 0.0f;
    S_in(1, 3) = 0.0f;
    S_in(1, 4) = 0.0f;
    S_in(1, 5) = -(term_20B + term_20K) / denom_base;
    S_in(1, 6) = term_20K / denom_base;
    S_in(1, 7) = -(term_20B + term_20K) / denom_base;
    S_in(1, 8) = 0.0f;
    S_in(1, 9) = 0.0f;
    S_in(1, 10) = 0.0f;
    
    // Row 2 (X2, Z_C4, Z_C6 dependent)
    S_in(2, 0) = -(term_940M * X2 * Z_C6) / denom_full;
    S_in(2, 1) = (term_940M * X2 * Z_C6 + term_940K * X2 * Z_C6) / denom_full;
    S_in(2, 2) = 1.0f - (2.0f * X2) / denom_X2;
    S_in(2, 3) = (2.0f * X2 * Z_C6) / (denom_C4 * denom_X2);
    S_in(2, 4) = 0.0f;
    S_in(2, 5) = (term_940M * X2 * Z_C6 + term_940K * X2 * Z_C6) / denom_full;
    S_in(2, 6) = -(term_940K * X2 * Z_C6) / denom_full;
    S_in(2, 7) = -(2.0f * X2 * Z_C6 * (1000000000001.0f * Z_C1 + 10000000001010000.0f)) / denom_full;
    S_in(2, 8) = (2.0f * X2 * Z_C6) / (denom_C4 * denom_X2);
    S_in(2, 9) = -(2.0f * X2) / denom_X2;
    S_in(2, 10) = -(2.0f * X2) / denom_X2;
    
    // Row 3 (Z_C4 dependent)
    S_in(3, 0) = (term_940M * Z_C4) / (denom_C4 * denom_base);
    S_in(3, 1) = -(term_940M * Z_C4 + term_940K * Z_C4) / (denom_C4 * denom_base);
    S_in(3, 2) = 0.0f;
    S_in(3, 3) = -(Z_C4 - 2200.0f) / denom_C4;
    S_in(3, 4) = 0.0f;
    S_in(3, 5) = -(term_940M * Z_C4 + term_940K * Z_C4) / (denom_C4 * denom_base);
    S_in(3, 6) = (term_940K * Z_C4) / (denom_C4 * denom_base);
    S_in(3, 7) = (2.0f * Z_C4 * (1000000000001.0f * Z_C1 + 10000000001010000.0f)) / (denom_C4 * denom_base);
    S_in(3, 8) = -(2.0f * Z_C4) / denom_C4;
    S_in(3, 9) = 0.0f;
    S_in(3, 10) = 0.0f;
    
    // Row 4 (Complex term dependent on X2, Z_C4, Z_C6)
    float complex_term = 2200.0f * X2 + 18000.0f * Z_C4 + 20200.0f * Z_C6 +
                        X2 * Z_C4 + X2 * Z_C6 + Z_C4 * Z_C6 + 39600000.0f;
    
    S_in(4, 0) = (term_940M * complex_term) / denom_full;
    S_in(4, 1) = -(term_940M * complex_term + term_940K * complex_term) / denom_full;
    S_in(4, 2) = -(2.0f * Z_C6) / denom_X2;
    S_in(4, 3) = -(2.0f * Z_C6 * (X2 + 18000.0f)) / (denom_C4 * denom_X2);
    S_in(4, 4) = -1.0f;
    S_in(4, 5) = -(term_940M * complex_term + term_940K * complex_term) / denom_full;
    S_in(4, 6) = (term_940K * complex_term) / denom_full;
    S_in(4, 7) = (2.0f * (1000000000001.0f * Z_C1 + 10000000001010000.0f) * complex_term) / denom_full;
    S_in(4, 8) = -(2.0f * Z_C6 * (X2 + 18000.0f)) / (denom_C4 * denom_X2);
    S_in(4, 9) = (2.0f * (X2 + 18000.0f)) / denom_X2;
    S_in(4, 10) = -(2.0f * Z_C6) / denom_X2;
    
    // Row 5 (Z_C1 dependent)
    S_in(5, 0) = (2000000000000.0f * Z_C1) / denom_base;
    S_in(5, 1) = -(2000000000002.0f * Z_C1) / denom_base;
    S_in(5, 2) = 0.0f;
    S_in(5, 3) = 0.0f;
    S_in(5, 4) = 0.0f;
    S_in(5, 5) = 1.0f - (2000000000002.0f * Z_C1) / denom_base;
    S_in(5, 6) = (2.0f * Z_C1) / denom_base;
    S_in(5, 7) = -(2000000000002.0f * Z_C1) / denom_base;
    S_in(5, 8) = 0.0f;
    S_in(5, 9) = 0.0f;
    S_in(5, 10) = 0.0f;
    
    // Row 6
    S_in(6, 0) = (2000000000000.0f * (Z_C1 + 480000.0f)) / denom_base;
    S_in(6, 1) = term_2M / denom_base;
    S_in(6, 2) = 0.0f;
    S_in(6, 3) = 0.0f;
    S_in(6, 4) = 0.0f;
    S_in(6, 5) = term_2M / denom_base;
    S_in(6, 6) = -(999999999999.0f * Z_C1 + 480000000000520000.0f) / denom_base;
    S_in(6, 7) = term_2M / denom_base;
    S_in(6, 8) = 0.0f;
    S_in(6, 9) = 0.0f;
    S_in(6, 10) = 0.0f;
    
    // Row 7
    S_in(7, 0) = term_940M / denom_base;
    S_in(7, 1) = -(term_940M + term_940K) / denom_base;
    S_in(7, 2) = 0.0f;
    S_in(7, 3) = 0.0f;
    S_in(7, 4) = 0.0f;
    S_in(7, 5) = -(term_940M + term_940K) / denom_base;
    S_in(7, 6) = term_940K / denom_base;
    S_in(7, 7) = 1.0f - (term_940M + term_940K) / denom_base;
    S_in(7, 8) = 0.0f;
    S_in(7, 9) = 0.0f;
    S_in(7, 10) = 0.0f;
    
    // Row 8
    float term_2068B = 2068000000000000000000.0f;
    float term_2068M = 2068000000.0f;
    
    S_in(8, 0) = term_2068B / (denom_C4 * denom_base);
    S_in(8, 1) = -(term_2068B + 2068000000000.0f) / (denom_C4 * denom_base);
    S_in(8, 2) = 0.0f;
    S_in(8, 3) = -4400.0f / denom_C4;
    S_in(8, 4) = 0.0f;
    S_in(8, 5) = -(term_2068B + 2068000000000.0f) / (denom_C4 * denom_base);
    S_in(8, 6) = term_2068M / (denom_C4 * denom_base);
    S_in(8, 7) = (4400.0f * (1000000000001.0f * Z_C1 + 10000000001010000.0f)) / (denom_C4 * denom_base);
    S_in(8, 8) = 1.0f - 4400.0f / denom_C4;
    S_in(8, 9) = 0.0f;
    S_in(8, 10) = 0.0f;
    
    // Row 9 (Z_C6, X2 dependent)
    float term_X2_18k = X2 + 18000.0f;
    
    S_in(9, 0) = (term_940M * Z_C6 * term_X2_18k) / denom_full;
    S_in(9, 1) = -(term_940M * Z_C6 * term_X2_18k + term_940K * Z_C6 * term_X2_18k) / denom_full;
    S_in(9, 2) = -(2.0f * Z_C6) / denom_X2;
    S_in(9, 3) = -(2.0f * Z_C6 * term_X2_18k) / (denom_C4 * denom_X2);
    S_in(9, 4) = 0.0f;
    S_in(9, 5) = -(term_940M * Z_C6 * term_X2_18k + term_940K * Z_C6 * term_X2_18k) / denom_full;
    S_in(9, 6) = (term_940K * Z_C6 * term_X2_18k) / denom_full;
    S_in(9, 7) = (2.0f * Z_C6 * (1000000000001.0f * Z_C1 + 10000000001010000.0f) * term_X2_18k) / denom_full;
    S_in(9, 8) = -(2.0f * Z_C6 * term_X2_18k) / (denom_C4 * denom_X2);
    S_in(9, 9) = 1.0f - (2.0f * Z_C6) / denom_X2;
    S_in(9, 10) = -(2.0f * Z_C6) / denom_X2;
    
    // Row 10 (Final row with Z_C6 dependencies)
    float term_16920B = 16920000000000000000000.0f;
    float term_16920M = 16920000000.0f;
    
    S_in(10, 0) = -(term_16920B * Z_C6) / denom_full;
    S_in(10, 1) = (term_16920B * Z_C6 + term_16920M * Z_C6) / denom_full;
    S_in(10, 2) = -36000.0f / denom_X2;
    S_in(10, 3) = (36000.0f * Z_C6) / (denom_C4 * denom_X2);
    S_in(10, 4) = 0.0f;
    S_in(10, 5) = (term_16920B * Z_C6 + term_16920M * Z_C6) / denom_full;
    S_in(10, 6) = -(term_16920M * Z_C6) / denom_full;
    S_in(10, 7) = -(36000.0f * Z_C6 * (1000000000001.0f * Z_C1 + 10000000001010000.0f)) / denom_full;
    S_in(10, 8) = (36000.0f * Z_C6) / (denom_C4 * denom_X2);
    S_in(10, 9) = -36000.0f / denom_X2;
    S_in(10, 10) = 1.0f - 36000.0f / denom_X2;

}
