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
                       ), apvts(*this, nullptr, "Parameters", createParameterLayout()), dist()
#endif
{
    auto* gain = apvts.getParameter("gain");
    gainParam = dynamic_cast<juce::AudioParameterFloat*>(gain);

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
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    dist.computeScatteringMatrices((int)sampleRate);
    


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

    float makeupGain = juce::Decibels::decibelsToGain(gainParam->get());
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
        
        //dry wet blending
        dryWetParam = 1;
        dry = 1.0f - dryWetParam;
        wet = dryWetParam;

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
           
           
            drySampleL = inputBufferL[sample] * dry;

            input_sample = inputBufferL[sample];

            MXRStageOutputL = dist.process(input_sample);
            /*
            if (!input_sample) {
                MXRStageOutputL = 0;
            }

            else {
                MXRStageOutputL = dist.process(input_sample);
            }

            */
            drySampleR = inputBufferR[sample] * dry;

            input_sample = inputBufferR[sample];
            
            MXRStageOutputR = dist.process(input_sample);

            /*
            if (!input_sample) {
                MXRStageOutputR = 0;
            }
            else {

                MXRStageOutputR = dist.process(input_sample);
            }

             
            
            wetSampleL = MXRStageOutputL * wet;
            wetSampleR = MXRStageOutputR * wet;
             */
            //output: sum of dry/wet signals
            //channelDataL[sample] = (drySampleL + wetSampleL * makeupGain);
           // channelDataR[sample] = (drySampleR + wetSampleR * makeupGain);

            channelDataL[sample] = MXRStageOutputL;
            channelDataR[sample] = MXRStageOutputR;

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
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(std::make_unique<juce::AudioParameterFloat>( juce::ParameterID { "gain", 1 },"Output Gain",juce::NormalisableRange<float> { -12.0f, 12.0f }, 0.0f));
    
    return layout;
}
