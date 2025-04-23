/*
  ==============================================================================

    FilmStripSlider.h
	Created: 17 Apr 2023 16:17pm
	Author:  MSC

  ==============================================================================
*/
#include <JuceHeader.h>
#pragma once
class FilmStripSlider
{
	public:
		FilmStripSlider (juce::Image *_knobStrip, unsigned int numOfPositions);
		void drawFrame (juce::Graphics &g, int x, int y, int width, int height, juce::Slider &slider, double position);
		juce::Image *knobStrip;

	private:
		int frameCount;
		int frameSize;
		bool isVerticalStrip;
};