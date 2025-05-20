/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "./GUI/FilmStripSlider.h"
#include <stdio.h>

//==============================================================================
ProvaMXRAudioProcessorEditor::ProvaMXRAudioProcessorEditor (ProvaMXRAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    onOffSwitch("onOffSwitch"), ledOnOff("onOffLed"), toneKnob("toneKnob"), mixKnob("mixKnob"), driveKnob("driveKnob"), volumeKnob("volumeKnob")

{    //background
    backgroundImage = juce::ImageCache::getFromMemory(BinaryData::pedal_full_res_2k_cropped_scaled_png, BinaryData::pedal_full_res_2k_cropped_scaled_pngSize);

    //on off
    addAndMakeVisible(onOffSwitch);
    onOffSwitch.addListener(this);
    onOffSwitch.setImages(false, true, true,
        juce::ImageCache::getFromMemory(BinaryData::onoffswitchup_png, BinaryData::onoffswitchup_pngSize), 1.000f, juce::Colour(0x00000000),
        juce::Image(), 1.000f, juce::Colour(0x00000000),
        juce::ImageCache::getFromMemory(BinaryData::onoffswitchdown_png, BinaryData::onoffswitchdown_pngSize), 1.000f, juce::Colour(0x00000000));

    //on off led
    ledOnImage = juce::ImageCache::getFromMemory(BinaryData::onoffledon_png, BinaryData::onoffledon_pngSize);
    ledOffImage = juce::ImageCache::getFromMemory(BinaryData::onoffledoff_png, BinaryData::onoffledoff_pngSize);
    
    
    //hp lp
    addAndMakeVisible(HpLpSwitch);
    HpLpSwitch.addListener(this);
    HpLpSwitch.setImages(
        true,  // toggleable
        false, // no mouse-over image
        false, // no click-down image

        // OFF state (toggleState == false)
        juce::ImageCache::getFromMemory(BinaryData::HpLpswitchup_png, BinaryData::HpLpswitchup_pngSize),
        1.0f, juce::Colour(0x00000000),

        // Hover image (unused)
        juce::Image(), 1.0f, juce::Colour(0x00000000),

        // ON state (toggleState == true)
        juce::ImageCache::getFromMemory(BinaryData::HpLpswitchdown_png, BinaryData::HpLpswitchdown_pngSize),
        1.0f, juce::Colour(0x00000000)
    );

    HpLpSwitch.setClickingTogglesState(true); // Makes it toggle on click

    HpLpSwitch.onClick = [this]() {
        if (HpLpSwitch.getToggleState())
            DBG("Switched ON");
        else
            DBG("Switched OFF");
    };


    //tone knob
    toneKnobAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "tone", toneKnob);
    addAndMakeVisible(toneKnob);
    toneKnob.setLookAndFeel(&toneKnobLookAndFeel);
    mixKnob.setLookAndFeel(&mixKnobLookAndFeel);
    toneKnob.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    toneKnob.setPopupDisplayEnabled(true, true, nullptr);
    toneKnob.setTextValueSuffix(" Hz");
    toneKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    toneKnob.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);

    
    //drive knob
    driveKnobAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "drive", driveKnob);
    addAndMakeVisible(driveKnob);
    driveKnob.setLookAndFeel(&driveKnobLookAndFeel);
    driveKnob.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    driveKnob.setPopupDisplayEnabled(true, true, nullptr);
    driveKnob.setTextValueSuffix(" ohm");
    driveKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    driveKnob.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);

    //volume knob
    volumeKnobAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "volume", volumeKnob);
    addAndMakeVisible(volumeKnob);
    volumeKnob.setLookAndFeel(&volKnobLookAndFeel);
    volumeKnob.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    volumeKnob.setPopupDisplayEnabled(true, true, nullptr);
    volumeKnob.setTextValueSuffix(" dB");
    volumeKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    volumeKnob.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);


    //dry wet knob
    mixKnobAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "MIX", mixKnob);
    addAndMakeVisible(mixKnob);
    mixKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    mixKnob.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);
    mixKnob.setPopupDisplayEnabled(true, true, nullptr);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(windowWidth, windowHeight);

    startTimerHz(20);
}

ProvaMXRAudioProcessorEditor::~ProvaMXRAudioProcessorEditor()
{
}

//==============================================================================
void ProvaMXRAudioProcessorEditor::paint(juce::Graphics& g)
{
    FilmStripSlider fsstone(&toneKnobStripImage, 100);
    FilmStripSlider fssdrive(&driveKnobStripImage, 100);
    FilmStripSlider fssvolume(&volKnobStripImage, 100);

    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawImage(backgroundImage, 0, 0, backgroundImage.getWidth(),
        backgroundImage.getHeight(), 0, 0, backgroundImage.getWidth(),
        backgroundImage.getHeight(), false);
    int ledX = windowWidth / 2 - ledWidth / 2;
    juce::Image& image = audioProcessor.getOnOffState() ? ledOnImage : ledOffImage;
    g.drawImageAt(image, ledX , ledY);
}

void ProvaMXRAudioProcessorEditor::resized()
{
    HpLpSwitch.setBounds(windowWidth/2 - switchHpLpWidth / 2 , 25, switchHpLpWidth, switchHpLpHeight);
    onOffSwitch.setBounds(windowWidth / 2 - switchWidth / 2, switchY, switchWidth, switchHeight);
    ledOnOff.setBounds(windowWidth / 2 - ledWidth / 2, ledY, ledWidth, ledHeight);
    toneKnob.setBounds(windowWidth / 2 - knobWidth / 2, 155, knobWidth, knobHeight);
    driveKnob.setBounds(195, 100, knobWidth, knobHeight);
    volumeKnob.setBounds(95, 100, knobWidth, knobHeight);
    mixKnob.setBounds(windowWidth - 55, knobY, 50, 50);
}


void ProvaMXRAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &onOffSwitch)
    {
        if (!audioProcessor.getOnOffState()) {
            ledOnOff.setImages(false, true, true,
                juce::ImageCache::getFromMemory(BinaryData::onoffledon_png, BinaryData::onoffledon_pngSize), 2.000f, juce::Colour(0x00000000),
                juce::Image(), 1.000f, juce::Colour(0x00000000),
                juce::Image(), 1.000f, juce::Colour(0x00000000));
            audioProcessor.setOnOffState(true);
        }
        else {
            ledOnOff.setImages(false, true, true,
                juce::ImageCache::getFromMemory(BinaryData::onoffledoff_png, BinaryData::onoffledoff_pngSize), 2.000f, juce::Colour(0x00000000),
                juce::Image(), 1.000f, juce::Colour(0x00000000),
                juce::Image(), 1.000f, juce::Colour(0x00000000));
            audioProcessor.setOnOffState(false);
        }
    }
}

void ProvaMXRAudioProcessorEditor::timerCallback()
{
    if (previousState != audioProcessor.getOnOffState())
    {
        repaint();
        previousState = audioProcessor.getOnOffState();
    }
}
