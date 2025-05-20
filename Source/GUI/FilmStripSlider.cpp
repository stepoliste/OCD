/*
  ==============================================================================

	FilmStripSlider.cpp
	Created: 17 Apr 2023 16:17pm
	Author:  MSC

  ==============================================================================
*/

#include "FilmStripSlider.h"
FilmStripSlider::FilmStripSlider(juce::Image* _knobStrip, unsigned int numOfPositions) : knobStrip(_knobStrip), frameCount(numOfPositions)
{
	frameSize = knobStrip->getHeight() / frameCount;
	isVerticalStrip = true;
}


void FilmStripSlider::drawFrame(juce::Graphics& g, int x, int y, int width, int height, juce::Slider& slider, double position)
{
	const double div = (slider.getMaximum() + slider.getInterval()) / frameCount;
	double pos = (int)(slider.getValue() / div);

	g.drawImage(*knobStrip, x, y, width, height, 0, (int)(pos * frameSize), width, height, false);
}