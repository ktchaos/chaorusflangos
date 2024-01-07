/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class ChaorusFlangosAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ChaorusFlangosAudioProcessorEditor (ChaorusFlangosAudioProcessor&);
    ~ChaorusFlangosAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ChaorusFlangosAudioProcessor& audioProcessor;

    juce::Slider mDryWetSlider;
    juce::Slider mDepthSlider;
    juce::Slider mRateSlider;
    juce::Slider mPhaseOffsetSlider;
    juce::Slider mFeedbackSlider;
    juce::ComboBox mType;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChaorusFlangosAudioProcessorEditor)
};
