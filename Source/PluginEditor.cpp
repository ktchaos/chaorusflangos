/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ChaorusFlangosAudioProcessorEditor::ChaorusFlangosAudioProcessorEditor (ChaorusFlangosAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 150);

    auto& params = processor.getParameters();
//    std::unique_ptr<CustomLookAndFeel> 
    customLookAndFeel = std::make_unique<CustomLookAndFeel>();

    // Define layout constants
    const int knobSize = 80;
    const int knobSpacing = 100;
    const int startX = 50;
    const int knobY = 50;
    const int comboY = 80;
    const int comboWidth = 120;
    const int comboHeight = 30;

    // Initializing the dry wet slider
    if (params.size() < 7) return; // Safety check
    juce::AudioParameterFloat* dryWetParameter = dynamic_cast<juce::AudioParameterFloat*>(params.getUnchecked(0));
    if (!dryWetParameter) return;

    mDryWetSlider.setBounds(startX, knobY, knobSize, knobSize);
    mDryWetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mDryWetSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    mDryWetSlider.setRange(dryWetParameter->range.start, dryWetParameter->range.end);
    mDryWetSlider.setValue(*dryWetParameter);
    mDryWetSlider.setLookAndFeel(customLookAndFeel.get());
    addAndMakeVisible(mDryWetSlider);

    mDryWetSlider.onValueChange = [this, dryWetParameter] { *dryWetParameter = mDryWetSlider.getValue(); };
    mDryWetSlider.onDragStart = [dryWetParameter] { dryWetParameter->beginChangeGesture(); };
    mDryWetSlider.onDragEnd = [dryWetParameter] { dryWetParameter->endChangeGesture(); };

    // Initializing the depth slider
    juce::AudioParameterFloat* depthParameter = dynamic_cast<juce::AudioParameterFloat*>(params.getUnchecked(1));
    if (!depthParameter) return;

    mDepthSlider.setBounds(startX + knobSpacing, knobY, knobSize, knobSize);
    mDepthSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mDepthSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    mDepthSlider.setRange(depthParameter->range.start, depthParameter->range.end);
    mDepthSlider.setValue(*depthParameter);
    mDepthSlider.setLookAndFeel(customLookAndFeel.get());
    addAndMakeVisible(mDepthSlider);

    mDepthSlider.onValueChange = [this, depthParameter] { *depthParameter = mDepthSlider.getValue(); };
    mDepthSlider.onDragStart = [depthParameter] { depthParameter->beginChangeGesture(); };
    mDepthSlider.onDragEnd = [depthParameter] { depthParameter->endChangeGesture(); };

    // Initializing the rate slider
    juce::AudioParameterFloat* rateParameter = dynamic_cast<juce::AudioParameterFloat*>(params.getUnchecked(2));
    if (!rateParameter) return;

    mRateSlider.setBounds(startX + 2 * knobSpacing, knobY, knobSize, knobSize);
    mRateSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mRateSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    mRateSlider.setRange(rateParameter->range.start, rateParameter->range.end);
    mRateSlider.setValue(*rateParameter);
    mRateSlider.setLookAndFeel(customLookAndFeel.get());
    addAndMakeVisible(mRateSlider);

    mRateSlider.onValueChange = [this, rateParameter] { *rateParameter = mRateSlider.getValue(); };
    mRateSlider.onDragStart = [rateParameter] { rateParameter->beginChangeGesture(); };
    mRateSlider.onDragEnd = [rateParameter] { rateParameter->endChangeGesture(); };

    // Initializing the phase slider
    juce::AudioParameterFloat* phaseParameter = dynamic_cast<juce::AudioParameterFloat*>(params.getUnchecked(3));
    if (!phaseParameter) return;

    mPhaseOffsetSlider.setBounds(startX + 3 * knobSpacing, knobY, knobSize, knobSize);
    mPhaseOffsetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mPhaseOffsetSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    mPhaseOffsetSlider.setRange(phaseParameter->range.start, phaseParameter->range.end);
    mPhaseOffsetSlider.setValue(*phaseParameter);
    mPhaseOffsetSlider.setLookAndFeel(customLookAndFeel.get());
    addAndMakeVisible(mPhaseOffsetSlider);

    mPhaseOffsetSlider.onValueChange = [this, phaseParameter] { *phaseParameter = mPhaseOffsetSlider.getValue(); };
    mPhaseOffsetSlider.onDragStart = [phaseParameter] { phaseParameter->beginChangeGesture(); };
    mPhaseOffsetSlider.onDragEnd = [phaseParameter] { phaseParameter->endChangeGesture(); };

    // Initializing the feedback slider
    juce::AudioParameterFloat* feedbackParameter = dynamic_cast<juce::AudioParameterFloat*>(params.getUnchecked(4));
    if (!feedbackParameter) return;
    mFeedbackSlider.setBounds(startX + 4 * knobSpacing, knobY, knobSize, knobSize);
    mFeedbackSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mFeedbackSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    mFeedbackSlider.setRange(feedbackParameter->range.start, feedbackParameter->range.end);
    mFeedbackSlider.setValue(*feedbackParameter);
    mFeedbackSlider.setLookAndFeel(customLookAndFeel.get());
    addAndMakeVisible(mFeedbackSlider);

    mFeedbackSlider.onValueChange = [this, feedbackParameter] { *feedbackParameter = mFeedbackSlider.getValue(); };
    mFeedbackSlider.onDragStart = [feedbackParameter] { feedbackParameter->beginChangeGesture(); };
    mFeedbackSlider.onDragEnd = [feedbackParameter] { feedbackParameter->endChangeGesture(); };

    // Initializing the distortion slider
    juce::AudioParameterFloat* distortionParameter = dynamic_cast<juce::AudioParameterFloat*>(params.getUnchecked(5));
    if (!distortionParameter) return;
    mDistortionSlider.setBounds(startX + 5 * knobSpacing, knobY, knobSize, knobSize);
    mDistortionSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mDistortionSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    mDistortionSlider.setRange(distortionParameter->range.start, distortionParameter->range.end);
    mDistortionSlider.setValue(*distortionParameter);
    mDistortionSlider.setLookAndFeel(customLookAndFeel.get());
    addAndMakeVisible(mDistortionSlider);

    mDistortionSlider.onValueChange = [this, distortionParameter] { *distortionParameter = mDistortionSlider.getValue(); };
    mDistortionSlider.onDragStart = [distortionParameter] { distortionParameter->beginChangeGesture(); };
    mDistortionSlider.onDragEnd = [distortionParameter] { distortionParameter->endChangeGesture(); };

    // Type selector
    juce::AudioParameterInt* typeParameter = dynamic_cast<juce::AudioParameterInt*>(params.getUnchecked(6));
    if (!typeParameter) return;

    mType.setBounds(startX + 6 * knobSpacing, comboY, comboWidth, comboHeight);
    mType.setColour(juce::ComboBox::backgroundColourId, juce::Colours::brown);
    mType.addItem("Jello", 1);
    mType.addItem("Wavy", 2);
    mType.addItem("Tormentrix", 3);
    addAndMakeVisible(mType);

    mType.onChange = [this, typeParameter] {
        typeParameter->beginChangeGesture();
        *typeParameter = mType.getSelectedItemIndex();
        typeParameter->endChangeGesture();
        updateDistortionKnobVisibility();
    };

    mType.setSelectedItemIndex(*typeParameter);
    mType.setLookAndFeel(customLookAndFeel.get());
    
    // Set initial visibility of distortion knob
    updateDistortionKnobVisibility();
}

ChaorusFlangosAudioProcessorEditor::~ChaorusFlangosAudioProcessorEditor()
{
}

//==============================================================================
void ChaorusFlangosAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Create a gradient background
    juce::ColourGradient gradient(juce::Colour(0xff2c2c2c), 0, 0,
                                  juce::Colour(0xff1a1a1a), 0, getHeight(), false);
    g.setGradientFill(gradient);
    g.fillAll();
    
    // Add a subtle border
    g.setColour(juce::Colour(0xff404040));
    g.drawRect(getLocalBounds(), 1);
    
    // Add plugin title
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(20.0f, juce::Font::bold));
    g.drawText("?", 0, 5, getWidth(), 20, juce::Justification::centred);
}

void ChaorusFlangosAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void ChaorusFlangosAudioProcessorEditor::updateDistortionKnobVisibility()
{
    // Show distortion knob only for Tormentrix mode (index 2)
    bool showDistortion = (mType.getSelectedItemIndex() == 2);
    mDistortionSlider.setVisible(showDistortion);
}

