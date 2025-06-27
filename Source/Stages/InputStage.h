#ifndef inputStage_h
#define inputStage_h
#endif

#include "../../eigen-3.4.0/Eigen/Dense"
#include <stdio.h>
#include <iostream>

using namespace Eigen;
using Vector11f = Eigen::Matrix<float, 11, 1>;
using Matrix11f = Eigen::Matrix<float, 11, 11>;


class InputStage {

public:

    InputStage();
    ~InputStage() {}
    void prepareInputStage(float sampleRate);
    
    float inputStageSample(const float inputSample,  float drive);
    void computeScatteringMatrix(int drive);

private:

    struct wavesIN {
        Vector11f a;
        Vector11f b;

        wavesIN() {
            a.setZero();
            b.setZero();
        }
    };
    wavesIN waves;

    // In InputStage.h
    struct S_Cache {
        // Row 3 - C4 state
        float S30, S31, S33, S35, S36, S37, S38;
        
        // Row 4 - Output
        float S40, S41, S42, S43, S45, S46, S47, S48, S49, S410;
        
        // Row 5 - C1 state
        float S50, S51, S55, S56, S57;
        
        // Row 9 - C6 state
        float S90, S91, S92, S93, S95, S96, S97, S98, S99, S910;
    };
    S_Cache S_cache;

    float Rin = 1e-6;
    float R2 = 1e6;
    float R3 = 10e3;
    float R6 = 470e3;
    float R5 = 2.2e3;
    float R8 = 18e3;
    float X2 = 500e3;//1e6;
    int drive= 1e6;
    float R_L = 1e6;

    float a_init_C1  = 0.0f;
    float a_init_C4  = 0.0f;
    float a_init_C6  = 0.0f;
    float a_init_C10 = 0.0f;
    float a_init_C11 = 0.0f;
    float V_ref = 4.5;
    int i= 0;
    float Z_C1 = 0.0f;
    float Z_C4 = 0.0f;
    float Z_C6 = 0.0f;
    
    // Track last drive value to avoid unnecessary recalculation
    int lastDrive = -1;
    
    Matrix<float, 11, 11> S_in;
};

