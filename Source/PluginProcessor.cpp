/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <iostream>
//==============================================================================
ProvaMXRAudioProcessor::ProvaMXRAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParameterLayout()), inputStage(), outputStage(), distStage()
#endif
{
    
}

ProvaMXRAudioProcessor::~ProvaMXRAudioProcessor()
{
}

//==============================================================================
const juce::String ProvaMXRAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ProvaMXRAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ProvaMXRAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ProvaMXRAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ProvaMXRAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ProvaMXRAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ProvaMXRAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ProvaMXRAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ProvaMXRAudioProcessor::getProgramName (int index)
{
    return {};
}

void ProvaMXRAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ProvaMXRAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (samplesPerBlock);
    sample_rate = (int)sampleRate;
    
    //Compute S for each stage

    inputStage.prepareInputStage(sample_rate);
    distStage.prepareDistStage(sample_rate);
    outputStage.prepareOutputStage(sample_rate);



}

void ProvaMXRAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ProvaMXRAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif



void ProvaMXRAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    float tone = getTone();
    float drive = getDrive();
    float volume = getVolume();
    
    

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

        auto* channelDataL = buffer.getWritePointer (0);
        auto* channelDataR = buffer.getWritePointer (1);
    
        auto* inputBufferL = buffer.getReadPointer(0);
        auto* inputBufferR = buffer.getReadPointer(1);
        // This is the place where you'd normally do the guts of your plugin's
        // audio processing...
        // Make sure to reset the state if your inner loop is processing
        // the samples and the outer loop is handling the channels.
        // Alternatively, you can process the samples with the channels
        // interleaved by keeping the same state.
        
    //if the effect is off, dry signal output
        if (!getOnOffState()) {

            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                 channelDataL[sample] = inputBufferL[sample];
                 channelDataR[sample] = inputBufferR[sample];
            }
        }

        else {
            
            //dry wet blending
            dryWetParam = getMix();
            dry = 1.0f - dryWetParam;
            wet = dryWetParam;
            
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                
                drySampleL = inputBufferL[sample];
                
                input_sample = inputBufferL[sample];
                
                if (!input_sample) {
                    outputL = 0;
                }
                
                else {
                    inputStageOutput = inputStage.inputStageSample(input_sample, drive);
                    distStageOutput = distStage.DistStageSample(inputStageOutput);
                    outputL = (outputStage.outputStageSample( distStageOutput, HpLpState,tone,volume));

                    //outputL = outputStage.outputStageSample( inputStageOutput, HpLpState,tone,volume);
                    //outputL= inputStageOutput;
                    //outputL=distStageOutput;
                }
                
                
                drySampleR = inputBufferR[sample];
                
                input_sample = inputBufferR[sample];
                //DBG("inpsample = " << input_sample);
                
                if (!input_sample) {
                    outputR = 0;
                }
                else {
                    
                    inputStageOutput = inputStage.inputStageSample(input_sample,  drive);
                    distStageOutput=distStage.DistStageSample(inputStageOutput);
                    outputR = (outputStage.outputStageSample( distStageOutput, HpLpState,tone,volume));
                    
                    
                    //outputR= inputStageOutput;
                    //outputR = outputStage.outputStageSample( inputStageOutput,HpLpState,tone,volume);
                    
                    //DBG("outputsample = " << outputR);
                   // outputR=distStageOutput;
                    
                }
                wetSampleL = outputL * wet;
                wetSampleR = outputR * wet;
                
                //makeupGain = juce::Decibels::gainToDecibels(volume);
                
                //output: sum of dry/wet signals
                channelDataL[sample] = (drySampleL + wetSampleL);
                channelDataR[sample] = (drySampleR + wetSampleR);
                
                //channelDataL[sample] = inputStageOutput;
                //channelDataR[sample] = inputStageOutput;
                
            }
            
        }
    
}

//==============================================================================
bool ProvaMXRAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ProvaMXRAudioProcessor::createEditor()
{
    return new ProvaMXRAudioProcessorEditor (*this);
}

//==============================================================================
void ProvaMXRAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    copyXmlToBinary(*apvts.copyState().createXml(), destData);
}

void ProvaMXRAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes)); if (xml.get() != nullptr && xml->hasTagName(apvts.state.getType())) {
            apvts.replaceState(juce::ValueTree::fromXml(*xml));
        }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ProvaMXRAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout ProvaMXRAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // tone Parameter
    juce::NormalisableRange<float> toneRange(1.0f, 10000.f);
    toneRange.interval = 1.0f;
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "tone", 1 }, // ID and version
        "tone",                          // Label
        toneRange,
        1.0f,
        "Hz"
    ));
    
    // drive Parameter
    juce::NormalisableRange<float> driveRange(1000.0f, 500000.0f);
    driveRange.interval = 10.0f;
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "drive", 1 },  // Parameter ID
        "Drive",                           // Display name
        driveRange,
        1000.0f,                              // Default value
        "ohm"                                // Unit suffix
    ));
     
    

    
    // volume Parameter
    juce::NormalisableRange<float> volumeRange(1.0f, 500000.0f);
    volumeRange.interval = 1.0f;
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "volume", 1 }, // ID and version
        "volume",                          // Label
        volumeRange,
        1000.0f,
        "dB"
    ));


    // Dry/Wet Mix Parameter
    juce::NormalisableRange<float> mixRange(0.f, 1.f, 0.0005f);
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "MIX", 1 },
        "Mix",
        mixRange,
        1.f
    ));

    return { params.begin(), params.end() };
}

float ProvaMXRAudioProcessor::getTone()
{
    auto& toneValue = *apvts.getRawParameterValue("tone");
    return toneValue;
}

float ProvaMXRAudioProcessor::getDrive()
{
    auto& driveValue = *apvts.getRawParameterValue("drive");
    return driveValue;
}

float ProvaMXRAudioProcessor::getVolume()
{
    auto& volumeValue = *apvts.getRawParameterValue("volume");
    return volumeValue;
}


float ProvaMXRAudioProcessor::getMix()
{
    auto& toneValue = *apvts.getRawParameterValue("MIX");
    return toneValue;
}
