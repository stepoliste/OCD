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
    S_dist.fill(0);
}

void DistStage::prepareDistStage(float sampleRate)
{
    float Ts=1/sampleRate;
    
    Z_C7 = Ts/(2 * 10e-9);
    Z_C8 = Ts/(2 * 100e-9);
    Z_C9 = Ts/(2 * 220e-12);
    
    Vvect <<
        -4.99856859293187, -4.90274089691738, -4.69744905685494, -4.40280036708721, -3.85515808873174,
        -3.25408103322061, -2.61949827203105, -2.18570481661994, -2.10629009685546, -2.09999999999945,
        -2.09988539747068, -2.09988539746983, -2.09988538806159, -2.09988538806041, -2.09988530056458,
        -2.09988530056338, -2.09988525291046, -2.09988525291013, -2.09988515584195, -2.09988515584180,
        -2.09986727857888, -1.98003958026245, -1.11899067331055, -0.334082570721260, 0.459237577074292,
        1.58463888486200, 2.09999690923582, 2.10000000000000, 2.10000000000000, 2.12501354909501,
        2.19964498588063, 2.42676854845128, 2.65840946779551, 2.88929036336938, 3.15077575236894,
        3.46084794733325, 3.77960162514239, 4.09440492158127, 4.39952684628453, 4.64321833572751,
        4.86287224142961, 4.99886111010489;
    
    Ivect <<
        -0.183278353939797, -0.143524149268422, -0.0831825265532629, -0.0363132528927298, -0.00697267589008764,
        -0.00101906309990301, -0.000108957524771731, -7.86012713816546e-06, -4.88199480023920e-07, 1.90286676216477e-30,
        -9.31127287009592e-37, 9.27552971044144e-09, -6.51691896195360e-36, 9.27628796227923e-09, -3.05618824139073e-36,
        9.28336652194673e-09, -4.69130289452809e-36, 9.28721967415956e-09, 9.29506909927909e-09, -5.53242738058275e-36,
        2.18952885050753e-47, -2.13667758435138e-30, -5.29508708107299e-31, 1.67556413856870e-30, 9.68331092150178e-31,
        -7.42712063768852e-31, 2.56131858883214e-26, -2.38637868109526e-31, 0.000168838907132336, 0.00326134553752407,
        0.0518296706643104, 0.559800893867905, 1.64200049403328, 3.29489319090481, 5.86853491171060, 9.90045257975064,
        15.1715570039531, 21.5168663356194, 28.7655108239571, 35.3430123318941, 41.8793706561026, 46.2177194919832;

    Z_mos <<
            2.41050467005058, 3.40215975018622, 6.28660669891802, 18.6650139261458, 100.960051769249,
            697.262799536054, 4290.84690174192, 10772.5853324651, 12884.2760193901, 516123905859.004,
            9.13742139983245e-05, -1.01430828460333, 0.000126505959464683, -9.43220381106409, 0.000128657841796976,
            -5.13315099319694, 3.55521169105161e-05, 12365.9289418431, -1.70802276581551e-05, 80512034589.0000,
            539655977486562, 3.87781953649632e+15, 3.53491183834118e+15, 3.57279632199757e+15, 5.06835691039501e+15,
            2.32096620626461e+15, 13919564424.6057, 1.18361429735879e-11, 8.08843830735436, 1.53662776286379,
            0.447118955161014, 0.214046391542586, 0.139682930423049, 0.101601317264040, 0.0769043964915685,
            0.0604718958603008, 0.0496119700371188, 0.0420936528469795, 0.0370492487381299, 0.0336049363892093,
            0.0313457662856823;

    Z_dist = Matrix<float, 11, 11>::Zero();
    Z_dist.diagonal() << Rin, NL_adapt, R12, Z_C8, Z_C9, R_L, Z_C7, R10, R9, R11, R13;
    
    // Kirchoff to Wave conversion
    aVect = (Vvect + Z_dist(1,1) * Ivect);
    bVect = (Vvect - Z_dist(1,1) * Ivect);

   
    S_dist = I - 2 * Z_dist * Bi_dist.transpose() *((Bv_dist * Z_dist * Bi_dist.transpose()).inverse() * Bv_dist);

   

    // Initialize wave variables
    a_init_C7 = 0.0f;
    a_init_C8 = 0.0f;
    a_init_C9 = 0.0f;
    V_ref = 0.0f;
    waves.a.setZero();
    waves.b.setZero();

    // Initialize nonlinear function parameters
    mu0 = 0.0f;
    mu1 = 0.0f;
    etaVect = Eigen::Matrix<float, 1, 42>::Zero();

    // Calculate eta values for CPWL function
    for (int i = 0; i < 42-1; i++) {
        if (aVect(i+1) != aVect(i)) {
            etaVect(i) = ((bVect(i+1) - bVect(i)) / (aVect(i+1) - aVect(i)) - mu1) * 0.5f;
        }
    }




}

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
    int idx;
    std::tie(waves.b(1), idx) = CPWL_function(waves.a(1), aVect, mu0, mu1, etaVect);

    
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

    // distput (a_dist(6) + b_dist(6)) / 2 in MATLAB
    float outputSample = (waves.a(5) + waves.b(5)) / 2.0f;  // MATLAB index 6 is C++ index 5


    return outputSample;
}

/*

std::pair<float, int> DistStage::CPWL_function(float a, const Eigen::Matrix<float, 1, 42>& aVect,
                                              float mu0, float mu1, const Eigen::Matrix<float, 1, 42>& etaVect)
{

    Eigen::Matrix<float, 1, 42> absDiff = (aVect.array() - a).cwiseAbs();
    float b = mu0 + mu1 * a + etaVect.dot(absDiff);

    // Find interval index
    int index = 0;
    int N = static_cast<int>(aVect.size());
    for (int j = 0; j < N - 1; ++j) {
        if (std::min(aVect(j + 1), aVect(j)) <= a && a <= std::max(aVect(j + 1), aVect(j))) {
            index = j;
            break;
        } else if (a <= aVect(0)) {
            index = 0;
            break;
        } else if (a >= aVect(N - 1)) {
            index = N - 1;
            break;
        }
    }

    return {b, index};
}
    
*/

std::pair<float, int> DistStage::CPWL_function(float a, const Eigen::Matrix<float, 1, 42>& aVect,
                                              float mu0, float mu1, const Eigen::Matrix<float, 1, 42>& etaVect)
{
    // Optimize the calculation of b
    float b = mu0 + mu1 * a;
    
    // Use simple loop to avoid creating temporary vector
    for (int i = 0; i < aVect.size(); ++i) {
        b += etaVect(i) * std::abs(aVect(i) - a);
    }

    // Binary search for interval (much faster than linear search)
    int index;
    if (a <= aVect(0)) {
        index = 0;
    } else if (a >= aVect(aVect.size() - 1)) {
        index = static_cast<int>(aVect.size()) - 2;
    } else {
        // Binary search
        int low = 0;
        int high = static_cast<int>(aVect.size()) - 2;
        
        while (low <= high) {
            int mid = low + (high - low) / 2;
            
            if (a > std::max(aVect(mid), aVect(mid + 1))) {
                low = mid + 1;
            } else if (a < std::min(aVect(mid), aVect(mid + 1))) {
                high = mid - 1;
            } else {
                index = mid;
                break;
            }
        }
    }

    return {b, index};
}
