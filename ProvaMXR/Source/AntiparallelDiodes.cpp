/*
  ==============================================================================

    AntiparallelDiodes.cpp
    Created: 15 Mar 2025 4:58:21pm
    Author:  Stefano Polimeno

  ==============================================================================
*/

#include "AntiparallelDiodes.h"
#include <cmath>

AntiparallelDiodes::AntiparallelDiodes() {}
AntiparallelDiodes::~AntiparallelDiodes() {}

float AntiparallelDiodes::LambertW(float x) {
    // Error threshold
    const float thresh = 1e-12;
    
    // Case when x == 0
    if (x == 0) {
        return 0.0f;
    }

    float w;

    // Initial guess based on x
    if (x < 0.14546954290661823f) {
        // First case: when x < 0.14546954290661823
        float num = 1 + 5.931375839364438f * x + 11.39220550532913f * x * x + 7.33888339911111f * x * x * x + 0.653449016991959f * x * x * x * x;
        float den = 1 + 6.931373689597704f * x + 16.82349461388016f * x * x + 16.43072324143226f * x * x * x + 5.115235195211697f * x * x * x * x;
        w = x * num / den;
    }
    else if (x < 8.706658967856612f) {
        // Second case: when x < 8.706658967856612
        float num = 1 + 2.4450530707265568f * x + 1.3436642259582265f * x * x + 0.14844005539759195f * x * x * x + 0.0008047501729130f * x * x * x * x;
        float den = 1 + 3.4447089864860025f * x + 3.2924898573719523f * x * x + 0.9164600188031222f * x * x * x + 0.05306864044833221f * x * x * x * x;
        w = x * num / den;
    }
    else {
        // Third case: when x >= 8.706658967856612
        float a = log(x);
        float b = log(a);
        float ia = 1.0f / a;
        w = a - b + (b * ia) + 0.5f * b * (b - 2) * (ia * ia) + (1.0f / 6.0f) * (2 * b * b - 9 * b + 6) * (ia * ia * ia);
    }

    // Lambert W (iterative approach)
    for (int m = 0; m < 20; ++m) {
        float w1 = w + 1;
        float z = log(x) - log(w) - w;
        float q = 2 * w1 * (w1 + (2.0f / 3.0f) * z);
        float e = (z / w1) * ((q - z) / (q - 2 * z));
        w = w * (1 + e);

        // Check convergence
        if (fabs(e) < thresh) {
            break;
        }
    }

    return w;
}

float AntiparallelDiodes::process(float a, float Z_diode) {
    const float Vt = 25.864e-3;   // Thermal Voltage (assuming 27°C in operation condition)
    const float Is = 2.52e-9;     // Saturation current of 1N914
    const float eta = 1.752;      // Ideality Factor 1N914

    // Compute the Lambert W value using the `LambertW` function
    float W_x = LambertW((Z_diode * Is / (eta * Vt)) * exp((Z_diode * Is + fabs(a)) / (eta * Vt)));

    // Return the final result based on the input `a` and computed `W_x`
    return copysign(fabs(a) + 2 * Z_diode * Is - 2 * (eta * Vt) * W_x, a);
}
