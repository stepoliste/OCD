/*
  ==============================================================================

    Sfaser25LookAndFeel.h
    Created: 9 May 2023 12:22:52pm
    Author:  claud

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
using namespace juce;

class driveKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    driveKnobLookAndFeel();
    ~driveKnobLookAndFeel();

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width,
        int height, float sliderPos, float rotaryStartAngle,
        float rotaryEndAngle, juce::Slider& slider) override;

private:

    const int knobY = 29;
    const int knobWidth = 70;
    const int knobHeight = 65;

    const int windowWidth = 360;
    const int windowHeight = 497;

    int driveKnobStripFrames;
    Image driveKnobStripImage;
};

#pragma once
#include <JuceHeader.h>
using namespace juce;

class volKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    volKnobLookAndFeel();
    ~volKnobLookAndFeel();

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width,
        int height, float sliderPos, float rotaryStartAngle,
        float rotaryEndAngle, juce::Slider& slider) override;

private:

    const int knobY = 29;
    const int knobWidth = 70;
    const int knobHeight = 65;

    const int windowWidth = 360;
    const int windowHeight = 497;

    int volKnobStripFrames;
    Image volKnobStripImage;
};


#pragma once
#include <JuceHeader.h>
using namespace juce;

class toneKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    toneKnobLookAndFeel();
    ~toneKnobLookAndFeel();

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width,
        int height, float sliderPos, float rotaryStartAngle,
        float rotaryEndAngle, juce::Slider& slider) override;

private:

    const int knobY = 29;
    const int knobWidth = 70;
    const int knobHeight = 65;

    const int windowWidth = 360;
    const int windowHeight = 497;

    int toneKnobStripFrames;
    Image toneKnobStripImage;
};

#pragma once
#include <JuceHeader.h>
using namespace juce;

class MixKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    MixKnobLookAndFeel();
    ~MixKnobLookAndFeel();

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width,
        int height, float sliderPos, float rotaryStartAngle,
        float rotaryEndAngle, juce::Slider& slider) override;

private:

    const int knobY = 29;
    const int knobWidth = 50;
    const int knobHeight = 50;

    int mixKnobStripFrames;
    Image mixKnobStripImage;
};
