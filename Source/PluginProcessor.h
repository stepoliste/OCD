/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <JuceHeader.h>
#include "Stages/InputStage.h"
#include "Stages/OutputStage.h"
#include "Stages/DistStage.h"
#include <cmath>


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
    //==============================================================================
  
    float getTone();
    float getDrive();
    float getVolume();
    float getMix();
    bool getOnOffState() { return onOffState; };
    void setOnOffState(bool onOffState) { this->onOffState = onOffState; };
    bool getHpLpState() { return HpLpState; };
    void setHpLpState(bool HpLpState) { this->HpLpState = HpLpState; };
     
   //==============================================================================
    //========================================================================
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts;

    //========================================================================
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProvaMXRAudioProcessor)
    float inputStageOutput = 0;
    float distStageOutput=0;
    /*
    float shiftStageOutput1 = 0;
    float shiftStageOutput2 = 0;
    float shiftStageOutput3 = 0;
    float shiftStageOutput4 = 0;
     */
    float outputL = 0;
    float outputR = 0;
    int rounded;
    float input_sample = 0;
    float makeupGain = 1;
    
    bool onOffState = true;
    bool HpLpState = true;
    


    // dry/wet
    float drySampleL;
    float drySampleR;
    float dryWetParam;
    float dry;
    float wet;
    float wetSampleL;
    float wetSampleR;

    // input stage
    InputStage inputStage;
    wavesIN initIN;
    Matrix11f S_in;

    // shifting stage
    /*
    ShiftStage shiftStage;
    wavesSTAGE initSTAGE1L, initSTAGE2L, initSTAGE3L, initSTAGE4L, initSTAGE1R, initSTAGE2R, initSTAGE3R, initSTAGE4R;
    Matrix8d S_stage;
     */
    wavesDIST initDIST;
    DistStage distStage;

    // output stage
    OutputStage outputStage;
    wavesOUT initOUT;
    Matrix6f S_out;
    
    //Output_Data O_data;
    juce::AudioFormatManager formatManager;
    int sample_rate;



};
