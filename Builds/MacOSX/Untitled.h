// volume Parameter
juce::NormalisableRange<float> volumeRange(0.1f, 10.f, 0.005f);
volumeRange.setSkewForCentre(2.5f);
params.push_back(std::make_unique<juce::AudioParameterFloat>(
    juce::ParameterID { "volume", 1 }, // ID and version
    "volume",                          // Label
    volumeRange,
    0.1f,
    "dB"
));
