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
// -----
class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel()
    {
        // Set default background color for the ComboBox
        setColour(juce::ComboBox::backgroundColourId, juce::Colours::brown);
    }

    void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area, const bool isSeparator, const bool isActive, const bool isHighlighted, const bool isTicked, const bool hasSubMenu, const juce::String& text, const juce::String& shortcutKeyText, const juce::Drawable* icon, const juce::Colour* const textColourToUse) override 
    {
        g.fillAll(findColour(juce::ComboBox::backgroundColourId));

        if (isHighlighted) {
            g.fillAll(findColour(juce::ComboBox::backgroundColourId));
        }

        LookAndFeel_V4::drawPopupMenuItem(g, area, isSeparator, isActive, isHighlighted, isTicked, hasSubMenu, text, shortcutKeyText, icon, textColourToUse);
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                          float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override 
    {
        setColour(juce::Slider::thumbColourId, juce::Colours::ivory);
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::brown);
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::antiquewhite);

        LookAndFeel_V4::drawRotarySlider(g, x, y, width, height, sliderPos, rotaryStartAngle, rotaryEndAngle, slider);
    }
};

// -----


class ChaorusFlangosAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ChaorusFlangosAudioProcessorEditor (ChaorusFlangosAudioProcessor&);
    ~ChaorusFlangosAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void updateDistortionKnobVisibility();

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ChaorusFlangosAudioProcessor& audioProcessor;

    juce::Slider mDryWetSlider;
    juce::Slider mDepthSlider;
    juce::Slider mRateSlider;
    juce::Slider mPhaseOffsetSlider;
    juce::Slider mFeedbackSlider;
    juce::Slider mDistortionSlider;
    juce::ComboBox mType;

    std::unique_ptr<CustomLookAndFeel> customLookAndFeel;
//    CustomLookAndFeel customLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChaorusFlangosAudioProcessorEditor)
};
