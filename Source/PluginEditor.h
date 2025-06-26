/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/Sfaser25LookAndFeel.h"

//==============================================================================
/**
*/
class ProvaMXRAudioProcessorEditor : public juce::AudioProcessorEditor,
    juce::Button::Listener,
    juce::Timer
{
public:
    ProvaMXRAudioProcessorEditor (ProvaMXRAudioProcessor&);
    ~ProvaMXRAudioProcessorEditor() override;
    void buttonClicked(juce::Button* btn) override;
    void timerCallback() override;
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    float previoustone = 0;
    bool previousState = true;
    bool repaintFlag = false;

    toneKnobLookAndFeel toneKnobLookAndFeel;
    driveKnobLookAndFeel driveKnobLookAndFeel;
    volKnobLookAndFeel volKnobLookAndFeel;
    MixKnobLookAndFeel mixKnobLookAndFeel;
    juce::Image toneKnobStripImage,driveKnobStripImage,volKnobStripImage, backgroundImage, ledOnImage, ledOffImage;

    juce::Slider toneKnob;
    juce::Slider driveKnob;
    juce::Slider volumeKnob;
    juce::Slider mixKnob;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixKnobAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> toneKnobAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveKnobAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeKnobAttachment;

    juce::ImageButton onOffSwitch;

    juce::ImageButton ledOnOff;
    
    juce::ImageButton HpLpSwitch;

    const int windowWidth = 360;
    const int windowHeight = 497;

    const int knobY = 29;
    //const int knobWidth = 118;
    //const int knobHeight = 135;
    const int knobWidth = 70;
    const int knobHeight = 65;

    const int ledY = 120;
    const int ledWidth = 30;
    const int ledHeight = 28;

    const int switchY = 300;
    const int switchWidth = 80;
    const int switchHeight = 70;
    
    const int switchHpLpWidth = 30;
    const int switchHpLpHeight = 30;

    ProvaMXRAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProvaMXRAudioProcessorEditor)
};
