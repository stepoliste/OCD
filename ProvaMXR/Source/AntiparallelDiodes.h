/*
  ==============================================================================

    AntiparallelDiodes.h
    Created: 15 Mar 2025 4:56:46pm
    Author:  Stefano Polimeno

  ==============================================================================
*/

#pragma once

#ifndef AntiparallelDiodes_h
#define AntiparallelDiodes_h
#endif

#include <stdio.h>
#include <iostream>


class AntiparallelDiodes {
public:
    AntiparallelDiodes();
    ~AntiparallelDiodes();

    float process(float a, float Z_diode);

private:
    float LambertW(float x);
};
