/*
  ==============================================================================

    DistStage.h
    Created: 23 May 2025 2:24:08pm
    Author:  Stefano Polimeno

  ==============================================================================
*/

#pragma once
#include "../../eigen-3.4.0/Eigen/Dense"
#include <stdio.h>
#include <iostream>

using namespace Eigen;
using Vector11f = Eigen::Matrix<float, 11, 1>;
using Matrix11f = Eigen::Matrix<float, 11, 11>;


struct wavesDIST
{
    Vector11f a;
    Vector11f b;

    wavesDIST()
    {
        a.setZero();
        b.setZero();
    }
};


class DistStage {

public:

    DistStage();
    ~DistStage() {}
    void prepareDistStage(float sampleRate);
    
    float DistStageSample(const float DistSample, wavesDIST& waves);
   

private:
    float R9 = 10e3;
    float R10 = 10e3;
    float R12 = 220e3;
    float R11 = 39e3;
    float R13 = 150e3;

    float Z_C7 = 0;
    float Z_C8 = 0;
    float Z_C9 = 0;

    float Rin = 1e-6;
    float NL_adapt =493.986254297973;
    float R_L = 1000000;

    int i= 0;
    

    float a_init_C7 = 0.0f;
    float a_init_C8 = 0.0f;
    float a_init_C9 = 0.0f;
    float V_ref = 4.5f;
    
    // CPWL helper functions
    // For 1xN static row vectors:
    std::pair<float, int> CPWL_function(float a, const Eigen::Matrix<float, 1, 42>& aVect,
                                              float mu0, float mu1, const Eigen::Matrix<float, 1, 42>& etaVect);

    //Eigen::VectorXf Vvect, Ivect, Vvect_d, Ivect_d;
    //Eigen::VectorXf Z_mos, Z_d;
    Eigen::Matrix<float, 1, 42> Vvect;
    Eigen::Matrix<float, 1, 42> Ivect;
    Eigen::Matrix<float, 1, 42> aVect;
    Eigen::Matrix<float, 1, 42> bVect;
    Eigen::Matrix<float, 41, 1> Z_mos;
    Eigen::Matrix<float, 1, 42> etaVect;

    Matrix<float, 11, 11> Z_dist;


    Matrix<float, 11, 11> S_dist;
    Matrix<float, 11, 11> I;

    Eigen::Matrix<float, 6, 5> Fv;
    Eigen::Matrix<float, 6, 5> Fi;
    Eigen::Matrix<float, 6, 11> Bv_dist;

    Eigen::Matrix<float, 6, 11> Bi_dist;

    
    float mu0=-0.984741527402546f;
    float mu1=-0.995080545246249f;

};
