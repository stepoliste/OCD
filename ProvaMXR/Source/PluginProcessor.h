/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MXRdist.h"

//==============================================================================
/**
*/
class ProvaMXRAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    ProvaMXRAudioProcessor();
    ~ProvaMXRAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //========================================================================
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts;

    //========================================================================
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProvaMXRAudioProcessor)
    float MXRStageOutputL = 0;
    float MXRStageOutputR = 0;
    int sampleRate;
    float makeupGain = 1;
    float input_sample = 0;
    bool onOffState = true;
    // dry/wet
    float drySampleL;
    float drySampleR;
    float dryWetParam;
    float dry;
    float wet;
    float wetSampleL;
    float wetSampleR;
    MXRdist dist;
    juce::AudioParameterFloat* gainParam;


};
