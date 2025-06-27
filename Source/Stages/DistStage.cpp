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
    /*
    Fv <<  1,  0, -1,  0,  0,
           1,  0,  0,  0,  0,
          -1, -1,  0,  0,  0,
           0, -1, -1,  1,  0,
           0,  0,  0,  0, -1,
           0, -1, -1,  0,  1;

    Fi <<  1,  0, -1,  0,  0,
           1,  0,  0,  0,  0,
          -1, -1,  0,  0,  0,
           0,  0,  0,  1, -1,
           0,  0,  0,  0, -1,
           0,  0,  0,  0,  0;


    Bv_dist.setZero(6, 11);
    Bv_dist.block<6,6>(0,0) = Matrix<float,6,6>::Identity();
    Bv_dist.block<6,5>(0,6) = Fv;

    Bi_dist.setZero(6, 11);
    Bi_dist.block<6,6>(0,0) = Matrix<float,6,6>::Identity();
    Bi_dist.block<6,5>(0,6) = Fi;

    I = Matrix<float, 11, 11>::Identity();
    Z_dist.fill(0);
    
    */
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
    

    //Z_dist.diagonal() << Rin, NL_adapt, R12, Z_C8, Z_C9, R_L, Z_C7, R10, R9, R11, R13;
    
    
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



   
    //S_dist = I - 2 * Z_dist * Bi_dist.transpose() *((Bv_dist * Z_dist * Bi_dist.transpose()).inverse() * Bv_dist);


    //computeScatteringMatrix(Z_C7, Z_C8, Z_C9);

 
    S_dist <<
        1.0000,    0.0000,   -0.0000,         0,         0,         0,   -0.0000,    0.0000,    0.0000,         0,         0,
        0.0648,    0.3108,   -0.0028,         0,         0,         0,   -0.6216,    0.0028,   -0.0648,         0,         0,
       -0.0619,   -1.2538,   -0.9104,         0,         0,         0,    0.5946,    1.9104,    0.0619,         0,         0,
        0.0052,   -0.0033,    0.0002,    0.9947,    0.0000,         0,    0.0016,    0.0051,    0.0002,   -0.0053,   -0.0000,
       -1.7836,    1.1539,   -0.0825,    1.8406,    0.5202,         0,   -0.5472,   -1.7581,   -0.0570,    1.8406,    0.4798,
        3.7217,   -2.4077,    0.1722,   -1.8406,   -1.5202,   -1.0000,    1.1418,    3.6685,    0.1189,   -1.8406,   -0.4798,
       -0.0648,   -1.3108,    0.0028,         0,         0,         0,   -0.3784,   -0.0028,    0.0648,         0,         0,
        0.0028,    0.0570,    0.0868,         0,         0,         0,   -0.0270,    0.9132,   -0.0028,         0,         0,
        1.9352,   -1.3108,    0.0028,         0,         0,         0,    0.6216,   -0.0028,   -0.9352,         0,         0,
        1.9329,   -1.2505,    0.0894,   -1.9947,    0.0000,         0,    0.5930,    1.9053,    0.0618,   -0.9947,   -0.0000,
       -1.7836,    1.1539,   -0.0825,    1.8406,    1.5202,         0,   -0.5472,   -1.7581,   -0.0570,    1.8406,   -0.5202;

    /*
    DBG("S_dist matrix:");
    for (int i = 0; i < 11; ++i) {
        juce::String rowStr = "Row " + juce::String(i) + ": ";
        for (int j = 0; j < 11; ++j) {
            rowStr += juce::String(S_dist(i, j), 6) + " ";
        }
        DBG(rowStr);
    }
        */

    // For waves.a(1) calculation
    S_cache.S10 = S_dist(1,0);
    S_cache.S13 = S_dist(1,3);
    S_cache.S14 = S_dist(1,4);
    S_cache.S16 = S_dist(1,6);
    S_cache.S18 = S_dist(1,8);
    
    // For waves.a(3) calculation
    S_cache.S30 = S_dist(3,0);
    S_cache.S31 = S_dist(3,1);
    S_cache.S33 = S_dist(3,3);
    S_cache.S36 = S_dist(3,6);
    S_cache.S38 = S_dist(3,8);
    
    // For waves.a(4) calculation
    S_cache.S40 = S_dist(4,0);
    S_cache.S41 = S_dist(4,1);
    S_cache.S43 = S_dist(4,3);
    S_cache.S44 = S_dist(4,4);
    S_cache.S46 = S_dist(4,6);
    S_cache.S48 = S_dist(4,8);
    
    // For waves.a(5) calculation
    S_cache.S50 = S_dist(5,0);
    S_cache.S51 = S_dist(5,1);
    S_cache.S53 = S_dist(5,3);
    S_cache.S54 = S_dist(5,4);
    S_cache.S56 = S_dist(5,6);
    S_cache.S58 = S_dist(5,8);
    
    // For waves.a(6) calculation
    S_cache.S60 = S_dist(6,0);
    S_cache.S61 = S_dist(6,1);
    S_cache.S62 = S_dist(6,2);
    S_cache.S66 = S_dist(6,6);
    S_cache.S68 = S_dist(6,8);


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

/*
float DistStage::DistStageSample(const float DistSample)
{
    
    // Set up b_dist (waves.b) for the distortion stage
    waves.b(3) = a_init_C8;
    waves.b(4) = a_init_C9;
    waves.b(0) = V_ref;
    waves.b(8) = DistSample;
    waves.b(6) = a_init_C7;

    // Compute wave at the NL port (a_dist(2) in MATLAB)
    waves.a(1) = S_dist.row(1) * waves.b;


    // Compute nonlinear reflected wave at port 2
    
    waves.b(1) = CPWL_function(waves.a(1));
     
     
    // Compute final a_dist
    // waves.a = S_dist * waves.b;

     
    // Calculate only the output rows we need instead of full matrix multiplication
    waves.a(3) = S_dist.row(3) * waves.b; // C8
    waves.a(4) = S_dist.row(4) * waves.b; // C9
    waves.a(5) = S_dist.row(5) * waves.b; // Output
    waves.a(6) = S_dist.row(6) * waves.b; // C7
    
 

    // Save capacitor states
    a_init_C9 = waves.a(4);
    a_init_C8 = waves.a(3);
    a_init_C7 = waves.a(6);

    float outputSample = (waves.a(5) + waves.b(5)) / 2.0f;  // MATLAB index 6 is C++ index 5

    return outputSample;
   

}



float DistStage::DistStageSample(const float DistSample)
{
    // Set up input values that change each sample
    waves.b(0) = V_ref;           // constant reference voltage 
    waves.b(3) = a_init_C8;       // previous capacitor states
    waves.b(4) = a_init_C9;
    waves.b(6) = a_init_C7;
    waves.b(8) = DistSample;      // current input sample
    
    // Pre-compute parts that only depend on constant/previous values
    float staticSum = S_dist(1,0) * V_ref + 
                      S_dist(1,3) * a_init_C8 + 
                      S_dist(1,4) * a_init_C9 + 
                      S_dist(1,6) * a_init_C7;
    
    // Add the input sample contribution
    waves.a(1) = staticSum + S_dist(1,8) * DistSample;
    
    // Compute nonlinear reflected wave 
    waves.b(1) = CPWL_function(waves.a(1));
    
    // Calculate only needed outputs with optimized expressions
    waves.a(3) = S_dist(3,0) * V_ref + S_dist(3,1) * waves.b(1) + 
                 S_dist(3,3) * a_init_C8 + S_dist(3,6) * a_init_C7 +
                 S_dist(3,8) * DistSample;
                 
    waves.a(4) = S_dist(4,0) * V_ref + S_dist(4,1) * waves.b(1) + 
                 S_dist(4,3) * a_init_C8 + S_dist(4,4) * a_init_C9 + 
                 S_dist(4,6) * a_init_C7 + S_dist(4,8) * DistSample;
                 
    waves.a(5) = S_dist(5,0) * V_ref + S_dist(5,1) * waves.b(1) + 
                 S_dist(5,3) * a_init_C8 + S_dist(5,4) * a_init_C9 + 
                 S_dist(5,6) * a_init_C7 + S_dist(5,8) * DistSample;
                 
    waves.a(6) = S_dist(6,0) * V_ref + S_dist(6,1) * waves.b(1) + 
                 S_dist(6,2) * waves.b(2) + S_dist(6,6) * a_init_C7 + 
                 S_dist(6,8) * DistSample;
    
    // Save capacitor states for next sample
    a_init_C8 = waves.a(3);
    a_init_C9 = waves.a(4);
    a_init_C7 = waves.a(6);

    // Calculate output sample (voltage)
    float outputSample = (waves.a(5) + waves.b(5)) / 2.0f;
    
    return outputSample;
}
    */

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
/*
    absDiff = (aVect.array() - a).cwiseAbs();
    float b = mu0 + mu1 * a + etaVect.dot(absDiff);

    // Find interval index
    
    
    for (int j = 0; j < N - 1; ++j) {
        if (std::min(aVect(j + 1), aVect(j)) <= a && a <= std::max(aVect(j + 1), aVect(j))) {
            idx = j;
            break;
        } else if (a <= aVect(0)) {
            idx = 0;
            break;
        } else if (a >= aVect(N - 1)) {
            idx = N - 1;
            break;
        }
    }

    */
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
    
/*
float DistStage::CPWL_function(float a)
{
    // Linear part of calculation (much faster than vector operations)
    float b = mu0 + mu1 * a;
    
    // Use binary search instead of linear search for interval location
    // This changes O(N) to O(log N) complexity
    if (a <= aVect(0)) {
        idx = 0;
        // Only compute the abs diff for elements that have non-zero eta values
        for (int i = 0; i < N; ++i) {
            if (etaVect(i) != 0.0f) {
                b += etaVect(i) * std::abs(aVect(i) - a);
            }
        }
        return b;
    }
    
    if (a >= aVect(N-1)) {
        idx = N-1;
        // Only compute the abs diff for elements that have non-zero eta values
        for (int i = 0; i < N; ++i) {
            if (etaVect(i) != 0.0f) {
                b += etaVect(i) * std::abs(aVect(i) - a);
            }
        }
        return b;
    }
    
    // Binary search for interval (much faster than linear search)
    int low = 0;
    int high = N - 2;
    
    while (low <= high) {
        int mid = (low + high) / 2;
        float minVal = std::min(aVect(mid), aVect(mid+1));
        float maxVal = std::max(aVect(mid), aVect(mid+1));
        
        if (a < minVal) {
            high = mid - 1;
        } else if (a > maxVal) {
            low = mid + 1;
        } else {
            // Found the interval
            idx = mid;
            break;
        }
    }
    
    // Manual calculation of dot product, only considering non-zero eta values
    for (int i = 0; i < N; ++i) {
        if (etaVect(i) != 0.0f) {
            b += etaVect(i) * std::abs(aVect(i) - a);
        }
    }
    
    return b;
}
*/
void DistStage::computeScatteringMatrix(float Z_C7, float Z_C8, float Z_C9)
{
        // Common denominators and terms
    float denom1 = 21273849723785142624152337067.0f * Z_C7 + 9993842803342638380234325410000.0f;
    float denom2 = (Z_C8 + 39000.0f);
    float denom3 = (Z_C9 + 150000.0f);
    float denom12 = denom1 * denom2;
    float denom123 = denom1 * denom2 * denom3;
    
    // Common recurring terms
    float term1 = 1015895846598417067.0f * Z_C7 + 499692140117162705000.0f;
    float term2 = 96651228319870227022208.0f * Z_C7 + 43451490449315819044497067.0f;
    float term3 = 20307337440586440353930257067.0f * Z_C7 + 9559327898849480189789354740000.0f;
    float term4 = 955932791816801284152337067.0f * Z_C7 + 999384280234325410000.0f;
    float term5 = 150000.0f * Z_C8 + 189000.0f * Z_C9 + Z_C8 * Z_C9 + 5850000000.0f;
    
    // Row 0
    S_dist(0, 0) = (21273849719730249535847662933.0f * Z_C7 + 9993842801343869819765674590000.0f) / denom1;
    S_dist(0, 1) = (4046202790215680000.0f * Z_C7) / denom1;
    S_dist(0, 2) = -(8690298088994134.0f * Z_C7) / denom1;
    S_dist(0, 3) = 0.0f;
    S_dist(0, 4) = 0.0f;
    S_dist(0, 5) = 0.0f;
    S_dist(0, 6) = -1998768560468650820000.0f / denom1;
    S_dist(0, 7) = (8690298088994134.0f * Z_C7) / denom1;
    S_dist(0, 8) = (2.0f * (2027446544152337067.0f * Z_C7 + 999384280234325410000.0f)) / denom1;
    S_dist(0, 9) = 0.0f;
    S_dist(0, 10) = 0.0f;
    
    // Row 1
    S_dist(1, 0) = (1998768560468650820000000000.0f * Z_C7) / denom1;
    S_dist(1, 1) = (19188178182417860166063342933.0f * Z_C7 - 9993842803342638380234325410000.0f) / denom1;
    S_dist(1, 2) = -(86902980898631638088994134.0f * Z_C7) / denom1;
    S_dist(1, 3) = 0.0f;
    S_dist(1, 4) = 0.0f;
    S_dist(1, 5) = 0.0f;
    S_dist(1, 6) = -19987685606685276760468650820000.0f / denom1;
    S_dist(1, 7) = (86902980898631638088994134.0f * Z_C7) / denom1;
    S_dist(1, 8) = -(1998768560468650820000000000.0f * Z_C7) / denom1;
    S_dist(1, 9) = 0.0f;
    S_dist(1, 10) = 0.0f;
    
    // Row 2
    S_dist(2, 0) = -(1911865579578709480000000000.0f * Z_C7) / denom1;
    S_dist(2, 1) = -(38702809301585480929771520000.0f * Z_C7) / denom1;
    S_dist(2, 2) = -(19340825157379047785619182933.0f * Z_C7 + 9124812994356321999344384070000.0f) / denom1;
    S_dist(2, 3) = 0.0f;
    S_dist(2, 4) = 0.0f;
    S_dist(2, 5) = 0.0f;
    S_dist(2, 6) = 19118655797698960379578709480000.0f / denom1;
    S_dist(2, 7) = (440000.0f * term2) / denom1;
    S_dist(2, 8) = (1911865579578709480000000000.0f * Z_C7) / denom1;
    S_dist(2, 9) = 0.0f;
    S_dist(2, 10) = 0.0f;
    
    // Row 3
    S_dist(3, 0) = (40000000000.0f * Z_C8 * term1) / denom12;
    S_dist(3, 1) = -(38702809297539278139555840000.0f * Z_C7 * Z_C8) / denom12;
    S_dist(3, 2) = (20000.0f * Z_C8 * term2) / denom12;
    S_dist(3, 3) = -(Z_C8 - 39000.0f) / denom2;
    S_dist(3, 4) = 0.0f;
    S_dist(3, 5) = 0.0f;
    S_dist(3, 6) = (19118655795700191819110058660000.0f * Z_C8) / denom12;
    S_dist(3, 7) = (2.0f * Z_C8 * term3) / denom12;
    S_dist(3, 8) = (2.0f * Z_C8 * term4) / denom12;
    S_dist(3, 9) = -(2.0f * Z_C8) / denom2;
    S_dist(3, 10) = 0.0f;
    
    // Row 4
    S_dist(4, 0) = -(6000000000000000.0f * Z_C9 * term1) / denom123;
    S_dist(4, 1) = (5805421394630891720933376000000000.0f * Z_C7 * Z_C9) / denom123;
    S_dist(4, 2) = -(3000000000.0f * Z_C9 * term2) / denom123;
    S_dist(4, 3) = (300000.0f * Z_C9) / (denom2 * denom3);
    S_dist(4, 4) = -(Z_C9 - 150000.0f) / denom3;
    S_dist(4, 5) = 0.0f;
    S_dist(4, 6) = -(2867798369355028772866508799000000000.0f * Z_C9) / denom123;
    S_dist(4, 7) = -(300000.0f * Z_C9 * term3) / denom123;
    S_dist(4, 8) = -(300000.0f * Z_C9 * term4) / denom123;
    S_dist(4, 9) = (300000.0f * Z_C9) / (denom2 * denom3);
    S_dist(4, 10) = (2.0f * Z_C9) / denom3;
    
    // Row 5
    S_dist(5, 0) = (40000000000.0f * term1 * term5) / denom123;
    S_dist(5, 1) = -(38702809297539278139555840000.0f * Z_C7 * term5) / denom123;
    S_dist(5, 2) = (20000.0f * term2 * term5) / denom123;
    S_dist(5, 3) = -(300000.0f * Z_C9) / (denom2 * denom3);
    S_dist(5, 4) = -300000.0f / denom3;
    S_dist(5, 5) = -1.0f;
    S_dist(5, 6) = (19118655795700191819110058660000.0f * term5) / denom123;
    S_dist(5, 7) = (2.0f * term3 * term5) / denom123;
    S_dist(5, 8) = (2.0f * term4 * term5) / denom123;
    S_dist(5, 9) = -(300000.0f * Z_C9) / (denom2 * denom3);
    S_dist(5, 10) = -(2.0f * Z_C9) / denom3;
    
    // Row 6
    S_dist(6, 0) = -(1998768560468650820000000000.0f * Z_C7) / denom1;
    S_dist(6, 1) = -(40462027906203002790215680000.0f * Z_C7) / denom1;
    S_dist(6, 2) = (86902980898631638088994134.0f * Z_C7) / denom1;
    S_dist(6, 3) = 0.0f;
    S_dist(6, 4) = 0.0f;
    S_dist(6, 5) = 0.0f;
    S_dist(6, 6) = 1.0f - (42547699447570285248304674134.0f * Z_C7) / denom1;
    S_dist(6, 7) = -(86902980898631638088994134.0f * Z_C7) / denom1;
    S_dist(6, 8) = (1998768560468650820000000000.0f * Z_C7) / denom1;
    S_dist(6, 9) = 0.0f;
    S_dist(6, 10) = 0.0f;
    
    // Row 7
    S_dist(7, 0) = (86902980889941340000000000.0f * Z_C7) / denom1;
    S_dist(7, 1) = (1759218604617521860444160000.0f * Z_C7) / denom1;
    S_dist(7, 2) = (20000.0f * term2) / denom1;
    S_dist(7, 3) = 0.0f;
    S_dist(7, 4) = 0.0f;
    S_dist(7, 5) = 0.0f;
    S_dist(7, 6) = -869029808986316380889941340000.0f / denom1;
    S_dist(7, 7) = (19427728138277679423708177067.0f * Z_C7 + 9124812994356321999344384070000.0f) / denom1;
    S_dist(7, 8) = -(86902980889941340000000000.0f * Z_C7) / denom1;
    S_dist(7, 9) = 0.0f;
    S_dist(7, 10) = 0.0f;
    
    // Row 8
    S_dist(8, 0) = (20000000000.0f * term4) / denom1;
    S_dist(8, 1) = -(40462027902156800000000000000.0f * Z_C7) / denom1;
    S_dist(8, 2) = (86902980889941340000000000.0f * Z_C7) / denom1;
    S_dist(8, 3) = 0.0f;
    S_dist(8, 4) = 0.0f;
    S_dist(8, 5) = 0.0f;
    S_dist(8, 6) = 19987685604686508200000000000000.0f / denom1;
    S_dist(8, 7) = -(86902980889941340000000000.0f * Z_C7) / denom1;
    S_dist(8, 8) = -(19275081159261598715847662933.0f * Z_C7 + 9993842801343869819765674590000.0f) / denom1;
    S_dist(8, 9) = 0.0f;
    S_dist(8, 10) = 0.0f;
    
    // Row 9
    S_dist(9, 0) = (1560000000000000.0f * term1) / denom12;
    S_dist(9, 1) = -(1509409562604031847442677760000000.0f * Z_C7) / denom12;
    S_dist(9, 2) = (780000000.0f * term2) / denom12;
    S_dist(9, 3) = -78000.0f / denom2;
    S_dist(9, 4) = 0.0f;
    S_dist(9, 5) = 0.0f;
    S_dist(9, 6) = 745627576032307480945292287740000000.0f / denom12;
    S_dist(9, 7) = (78000.0f * term3) / denom12;
    S_dist(9, 8) = (78000.0f * term4) / denom12;
    S_dist(9, 9) = 1.0f - 78000.0f / denom2;
    S_dist(9, 10) = 0.0f;
    
    // Row 10
    S_dist(10, 0) = -(6000000000000000.0f * Z_C9 * term1) / denom123;
    S_dist(10, 1) = (5805421394630891720933376000000000.0f * Z_C7 * Z_C9) / denom123;
    S_dist(10, 2) = -(3000000000.0f * Z_C9 * term2) / denom123;
    S_dist(10, 3) = (300000.0f * Z_C9) / (denom2 * denom3);
    S_dist(10, 4) = 300000.0f / denom3;
    S_dist(10, 5) = 0.0f;
    S_dist(10, 6) = -(2867798369355028772866508799000000000.0f * Z_C9) / denom123;
    S_dist(10, 7) = -(300000.0f * Z_C9 * term3) / denom123;
    S_dist(10, 8) = -(300000.0f * Z_C9 * term4) / denom123;
    S_dist(10, 9) = (300000.0f * Z_C9) / (denom2 * denom3);
    S_dist(10, 10) = 1.0f - 300000.0f / denom3;
    
    return;
}

