
#include "OCDLookAndFeel.h"

toneKnobLookAndFeel::toneKnobLookAndFeel()
{
    toneKnobStripImage = ImageCache::getFromMemory(BinaryData::toneknob_strip_png, BinaryData::toneknob_strip_pngSize);
    toneKnobStripFrames = toneKnobStripImage.getHeight() / knobHeight;
}

toneKnobLookAndFeel::~toneKnobLookAndFeel()
{
}


volKnobLookAndFeel::volKnobLookAndFeel()
{
    volKnobStripImage = ImageCache::getFromMemory(BinaryData::volknob_strip_png, BinaryData::volknob_strip_pngSize);
    volKnobStripFrames = volKnobStripImage.getHeight() / knobHeight;
}

volKnobLookAndFeel::~volKnobLookAndFeel()
{
}

driveKnobLookAndFeel::driveKnobLookAndFeel()
{
    driveKnobStripImage = ImageCache::getFromMemory(BinaryData::driveknob_strip_png, BinaryData::driveknob_strip_pngSize);
    driveKnobStripFrames = driveKnobStripImage.getHeight() / knobHeight;
}

driveKnobLookAndFeel::~driveKnobLookAndFeel()
{
}

//==================================================================================================================

void toneKnobLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
{
    if (toneKnobStripImage.isValid()) {

        auto imageIndex = int(sliderPos * toneKnobStripFrames);
        const int frameId = jlimit(0, toneKnobStripFrames - 1, imageIndex);
        const float radiusWidth = width / 2.0f;
        const float radiusHeight = height / 2.0f;
        const float centerX = x + width * 0.5f;
        const float centerY = y + height * 0.5f;
        const float rx = centerX - radiusWidth;
        const float ry = centerY - radiusHeight;

        g.drawImage(toneKnobStripImage, rx, ry, knobWidth, knobHeight, 0, frameId * (toneKnobStripImage.getHeight() / toneKnobStripFrames), knobWidth, knobHeight);
    }
    else {
        static const float textPpercent = 0.35f;
        juce::Rectangle<float> text_bounds(1.0f + width * (1.0f - textPpercent) / 2.0f,
            0.5f * height, width * textPpercent, 0.5f * height);

        g.setColour(juce::Colours::white);

        g.drawFittedText(juce::String("No Image"), text_bounds.getSmallestIntegerContainer(),
            juce::Justification::horizontallyCentred | juce::Justification::centred, 1);
    }
}


//==========================================================================================================================

void volKnobLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
{
    if (volKnobStripImage.isValid()) {

        auto imageIndex = int(sliderPos * volKnobStripFrames);
        const int frameId = jlimit(0, volKnobStripFrames - 1, imageIndex);
        const float radiusWidth = width / 2.0f;
        const float radiusHeight = height / 2.0f;
        const float centerX = x + width * 0.5f;
        const float centerY = y + height * 0.5f;
        const float rx = centerX - radiusWidth;
        const float ry = centerY - radiusHeight;

        g.drawImage(volKnobStripImage, rx, ry, knobWidth, knobHeight, 0, frameId * (volKnobStripImage.getHeight() / volKnobStripFrames), knobWidth, knobHeight);
    }
    else {
        static const float textPpercent = 0.35f;
        juce::Rectangle<float> text_bounds(1.0f + width * (1.0f - textPpercent) / 2.0f,
            0.5f * height, width * textPpercent, 0.5f * height);

        g.setColour(juce::Colours::white);

        g.drawFittedText(juce::String("No Image"), text_bounds.getSmallestIntegerContainer(),
            juce::Justification::horizontallyCentred | juce::Justification::centred, 1);
    }
}


//==========================================================================================================================

void driveKnobLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
{
    if (driveKnobStripImage.isValid()) {

        auto imageIndex = int(sliderPos * driveKnobStripFrames);
        const int frameId = jlimit(0, driveKnobStripFrames - 1, imageIndex);
        const float radiusWidth = width / 2.0f;
        const float radiusHeight = height / 2.0f;
        const float centerX = x + width * 0.5f;
        const float centerY = y + height * 0.5f;
        const float rx = centerX - radiusWidth;
        const float ry = centerY - radiusHeight;

        g.drawImage(driveKnobStripImage, rx, ry, knobWidth, knobHeight, 0, frameId * (driveKnobStripImage.getHeight() / driveKnobStripFrames), knobWidth, knobHeight);
    }
    else {
        static const float textPpercent = 0.35f;
        juce::Rectangle<float> text_bounds(1.0f + width * (1.0f - textPpercent) / 2.0f,
            0.5f * height, width * textPpercent, 0.5f * height);

        g.setColour(juce::Colours::white);

        g.drawFittedText(juce::String("No Image"), text_bounds.getSmallestIntegerContainer(),
            juce::Justification::horizontallyCentred | juce::Justification::centred, 1);
    }
}


//================================================================================================================
MixKnobLookAndFeel::MixKnobLookAndFeel()
{
   mixKnobStripImage = ImageCache::getFromMemory(BinaryData::dry_wet_knob_png, BinaryData::dry_wet_knob_pngSize);
}

MixKnobLookAndFeel::~MixKnobLookAndFeel()
{
}
void MixKnobLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, 
    float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
{
    const double rotation = (slider.getValue()
        - slider.getMinimum())
        / (slider.getMaximum()
            - slider.getMinimum());

    const int frames = mixKnobStripImage.getHeight() / mixKnobStripImage.getWidth();
    const int frameId = (int)ceil(rotation * ((double)frames - 1.0));
    const float radius = juce::jmin(width / 2.0f, height / 2.0f);
    const float centerX = x + width * 0.5f;
    const float centerY = y + height * 0.5f;
    const float rx = centerX - radius - 1.0f;
    const float ry = centerY - radius;

    g.drawImage(mixKnobStripImage, (int)rx, (int)ry, 2 * (int)radius, 2 * (int)radius, 0, frameId * mixKnobStripImage.getWidth(),
        mixKnobStripImage.getWidth(),
        mixKnobStripImage.getWidth());
}
