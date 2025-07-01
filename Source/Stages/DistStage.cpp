/*
  ==============================================================================

    DistStage.cpp
    Created: 23 May 2025 2:23:53pm
    Author:  Stefano Polimeno

  ==============================================================================
*/

#include "DistStage.h"
#include <JuceHeader.h>

DistStage::DistStage()
{
    
    S_dist.fill(0);
    

    // Initialize variables
    a_init_C7 = 0.0f;
    a_init_C8 = 0.0f;
    a_init_C9 = 0.0f;
    V_ref = 4.5f;
    waves.a.setZero();
    waves.b.setZero();
    absDiff.setZero();

    etaVect = Eigen::Matrix<float, 1, 42>::Zero();
}

void DistStage::prepareDistStage(float sampleRate)
{
    float Ts=1/sampleRate;
    
    Z_C7 = Ts/(2 * 10e-9);
    Z_C8 = Ts/(2 * 100e-9);
    Z_C9 = Ts/(2 * 220e-12);
    

    
    
    aVect << -0.0096, -0.0076, -0.0046, -0.0022,
             -0.0007, -0.0004, -0.0003, -0.0002,
             -0.0002, -0.0002, -0.0002, -0.0002,
             -0.0002, -0.0002, -0.0002, -0.0002,
             -0.0002, -0.0002, -0.0002, -0.0002,
             -0.0002, -0.0002, -0.0001, -0.0000,
              0.0000,  0.0002,  0.0002,  0.0002,
              0.0002,  0.0004,  0.0028,  0.0279,
              0.0814,  0.1631,  0.2902,  0.4894,
              0.7498,  1.0633,  1.4214,  1.7464,
              2.0693,  2.2836;
    aVect *= 1.0e+04;



   


    computeScatteringMatrix(Z_C7, Z_C8, Z_C9);

 
    
    

    etaVect <<
        0, 0.0020, 0.0057, 0.0238,
        0.1333, 0.4156, 0.3114, 0.0594,
        0.0069, 0.0369, -1.0000, -0.0021,
        0.0021, -0.0195, 0.0195, -0.0105,
        0.0105, 0.9616, -0.9616, 1.0000,
        0, 0, 0, 0,
        0, 0, 0, -1.0000,
        0.0161, -0.0130, -0.0022, -0.0005,
        -0.0002, -0.0001, -0.0000, -0.0000,
        -0.0000, -0.0000, -0.0000, -0.0000,
        -0.0000, 0;



}


float DistStage::DistStageSample(const float DistSample)
{
    // Set up input values that change each sample
    waves.b(0) = V_ref;           // constant reference voltage 
    waves.b(3) = a_init_C8;       // previous capacitor states
    waves.b(4) = a_init_C9;
    waves.b(6) = a_init_C7;
    waves.b(8) = DistSample;      // current input sample
    
    // Pre-compute parts that only depend on constant/previous values using cached struct
    float staticSum = S_cache.S10 * V_ref + 
                    S_cache.S13 * a_init_C8 + 
                    S_cache.S14 * a_init_C9 + 
                    S_cache.S16 * a_init_C7;
    
    // Add the input sample contribution
    waves.a(1) = staticSum + S_cache.S18 * DistSample;
    
    // Compute nonlinear reflected wave 
    waves.b(1) = CPWL_function(waves.a(1));
    
    // Calculate only needed outputs with optimized expressions using cached struct
    waves.a(3) = S_cache.S30 * V_ref + S_cache.S31 * waves.b(1) + 
                S_cache.S33 * a_init_C8 + S_cache.S36 * a_init_C7 +
                S_cache.S38 * DistSample;
                
    waves.a(4) = S_cache.S40 * V_ref + S_cache.S41 * waves.b(1) + 
                S_cache.S43 * a_init_C8 + S_cache.S44 * a_init_C9 + 
                S_cache.S46 * a_init_C7 + S_cache.S48 * DistSample;
                
    waves.a(5) = S_cache.S50 * V_ref + S_cache.S51 * waves.b(1) + 
                S_cache.S53 * a_init_C8 + S_cache.S54 * a_init_C9 + 
                S_cache.S56 * a_init_C7 + S_cache.S58 * DistSample;
                
    waves.a(6) = S_cache.S60 * V_ref + S_cache.S61 * waves.b(1) + 
                S_cache.S62 * waves.b(2) + S_cache.S66 * a_init_C7 + 
                S_cache.S68 * DistSample;
    
    // Save capacitor states for next sample
    a_init_C8 = waves.a(3);
    a_init_C9 = waves.a(4);
    a_init_C7 = waves.a(6);

    // Calculate output sample (voltage)
    float outputSample = (waves.a(5) + waves.b(5)) / 2.0f;
    
    return outputSample;
}

float DistStage::CPWL_function(float a)
{
   // Start with the linear part (fast calculation)
    float b = mu0 + mu1 * a;
    
    // Only calculate terms for non-zero eta values
    // Manually unrolled based on your etaVect initialization
    
    // eta[1] = 0.0020
    b += 0.0020f * std::abs(aVect(1) - a);
    
    // eta[2] = 0.0057
    b += 0.0057f * std::abs(aVect(2) - a);
    
    // eta[3] = 0.0238
    b += 0.0238f * std::abs(aVect(3) - a);
    
    // eta[4] = 0.1333
    b += 0.1333f * std::abs(aVect(4) - a);
    
    // eta[5] = 0.4156
    b += 0.4156f * std::abs(aVect(5) - a);
    
    // eta[6] = 0.3114
    b += 0.3114f * std::abs(aVect(6) - a);
    
    // eta[7] = 0.0594
    b += 0.0594f * std::abs(aVect(7) - a);
    
    // eta[8] = 0.0069
    b += 0.0069f * std::abs(aVect(8) - a);
    
    // eta[9] = 0.0369
    b += 0.0369f * std::abs(aVect(9) - a);
    
    // eta[10] = -1.0000
    b -= 1.0000f * std::abs(aVect(10) - a);
    
    // eta[11] = -0.0021
    b -= 0.0021f * std::abs(aVect(11) - a);
    
    // eta[12] = 0.0021
    b += 0.0021f * std::abs(aVect(12) - a);
    
    // eta[13] = -0.0195
    b -= 0.0195f * std::abs(aVect(13) - a);
    
    // eta[14] = 0.0195
    b += 0.0195f * std::abs(aVect(14) - a);
    
    // eta[15] = -0.0105
    b -= 0.0105f * std::abs(aVect(15) - a);
    
    // eta[16] = 0.0105
    b += 0.0105f * std::abs(aVect(16) - a);
    
    // eta[17] = 0.9616
    b += 0.9616f * std::abs(aVect(17) - a);
    
    // eta[18] = -0.9616
    b -= 0.9616f * std::abs(aVect(18) - a);
    
    // eta[19] = 1.0000
    b += 1.0000f * std::abs(aVect(19) - a);
    
    // eta[23] = -1.0000
    b -= 1.0000f * std::abs(aVect(23) - a);
    
    // eta[27] = 0.0161
    b += 0.0161f * std::abs(aVect(27) - a);
    
    // eta[28] = -0.0130
    b -= 0.0130f * std::abs(aVect(28) - a);
    
    // eta[29] = -0.0022
    b -= 0.0022f * std::abs(aVect(29) - a);
    
    // eta[30] = -0.0005
    b -= 0.0005f * std::abs(aVect(30) - a);
    
    // eta[31] = -0.0002
    b -= 0.0002f * std::abs(aVect(31) - a);
    
    // eta[32] = -0.0001
    b -= 0.0001f * std::abs(aVect(32) - a);
    return b;
}
    
void DistStage::computeScatteringMatrix(float Z_C7, float Z_C8, float Z_C9)
{
    // Common denominators and terms used in multiple calculations
    double denom_main = 21273849723785142624152337067.0 * Z_C7 + 9993842803342638380234325410000.0;
    double denom_C8 = Z_C8 + 39000.0;
    double denom_C9 = Z_C9 + 150000.0;
    double denom_C8C9 = denom_C8 * denom_C9;
    
    double term1 = 1015895846598417067.0 * Z_C7 + 499692140117162705000.0;
    
    double term4 = 955932791816801284152337067.0 * Z_C7 + 999384280234325410000.0;
    double term5 = 150000.0 * Z_C8 + 189000.0 * Z_C9 + Z_C8 * Z_C9 + 5850000000.0;
    
    // For waves.a(1) calculation
    S_cache.S10 = (1998768560468650820000000000.0 * Z_C7) / denom_main;
    S_cache.S13 = 0.0;
    S_cache.S14 = 0.0;
    S_cache.S16 = -19987685606685276760468650820000.0 / denom_main;
    S_cache.S18 = -(1998768560468650820000000000.0 * Z_C7) / denom_main;
    
    // For waves.a(3) calculation
    S_cache.S30 = (40000000000.0 * Z_C8 * term1) / (denom_C8 * denom_main);
    S_cache.S31 = -(38702809297539278139555840000.0 * Z_C7 * Z_C8) / (denom_C8 * denom_main);
    S_cache.S33 = -(Z_C8 - 39000.0) / denom_C8;
    S_cache.S36 = (19118655795700191819110058660000.0 * Z_C8) / (denom_C8 * denom_main);
    S_cache.S38 = (2.0 * Z_C8 * term4) / (denom_C8 * denom_main);
    
    // For waves.a(4) calculation
    S_cache.S40 = -(6000000000000000.0 * Z_C9 * term1) / (denom_C8C9 * denom_main);
    S_cache.S41 = (5805421394630891720933376000000000.0 * Z_C7 * Z_C9) / (denom_C8C9 * denom_main);
    S_cache.S43 = (300000.0 * Z_C9) / denom_C8C9;
    S_cache.S44 = -(Z_C9 - 150000.0) / denom_C9;
    S_cache.S46 = -(2867798369355028772866508799000000000.0 * Z_C9) / (denom_C8C9 * denom_main);
    S_cache.S48 = -(300000.0 * Z_C9 * term4) / (denom_C8C9 * denom_main);
    
    // For waves.a(5) calculation
    S_cache.S50 = (40000000000.0 * term1 * term5) / (denom_C8C9 * denom_main);
    S_cache.S51 = -(38702809297539278139555840000.0 * Z_C7 * term5) / (denom_C8C9 * denom_main);
    S_cache.S53 = -(300000.0 * Z_C9) / denom_C8C9;
    S_cache.S54 = -300000.0 / denom_C9;
    S_cache.S56 = (19118655795700191819110058660000.0 * term5) / (denom_C8C9 * denom_main);
    S_cache.S58 = (2.0 * term4 * term5) / (denom_C8C9 * denom_main);
    
    // For waves.a(6) calculation
    S_cache.S60 = -(1998768560468650820000000000.0 * Z_C7) / denom_main;
    S_cache.S61 = -(40462027906203002790215680000.0 * Z_C7) / denom_main;
    S_cache.S62 = (86902980898631638088994134.0 * Z_C7) / denom_main;
    S_cache.S66 = 1.0 - (42547699447570285248304674134.0 * Z_C7) / denom_main;
    S_cache.S68 = (1998768560468650820000000000.0 * Z_C7) / denom_main;
}
