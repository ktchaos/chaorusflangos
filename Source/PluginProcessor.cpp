/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ChaorusFlangosAudioProcessor::ChaorusFlangosAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    /* Construct and add parameters */
    addParameter(mDryWetParameter = new juce::AudioParameterFloat(juce::ParameterID{"dry wet", 1},
                                                                  "Dry Wet",
                                                                  0.0,
                                                                  1.0,
                                                                  0.5));
    addParameter(mDepthParameter = new juce::AudioParameterFloat(juce::ParameterID{"depth", 2},
                                                                    "Depth",
                                                                    0.0,
                                                                    1.0,
                                                                    0.5));
    addParameter(mRateParameter = new juce::AudioParameterFloat(juce::ParameterID{"rate", 3},
                                                                    "Rate",
                                                                    0.1f,
                                                                    20.f,
                                                                    10.f));
    addParameter(mPhaseOffsetParameter = new juce::AudioParameterFloat(juce::ParameterID{"phaseoffset", 4},
                                                                    "Phase Offset",
                                                                    0.0f,
                                                                    1.f,
                                                                    0.f));
    addParameter(mFeedbackParameter = new juce::AudioParameterFloat(juce::ParameterID{"feedback", 5},
                                                                    "Feedback",
                                                                    0.0,
                                                                    0.98,
                                                                    0.5));
    addParameter(mTypeParameter = new juce::AudioParameterInt(juce::ParameterID{"type", 5},
                                                                    "Type",
                                                                    0,
                                                                    1,
                                                                    0));


    /* Initialize our data to default values */
    mCircularBufferLeft = nullptr;
    delete [] mCircularBufferLeft;
    mCircularBufferRight = nullptr;
    delete [] mCircularBufferRight;

    mCircularBufferWriteHead = 0;
    mCircularBufferLength = 0;

    mFeedbackLeft = 0;
    mFeedbackRight = 0;
    mLFOPhase = 0;
}

ChaorusFlangosAudioProcessor::~ChaorusFlangosAudioProcessor()
{
    if (mCircularBufferLeft != nullptr) {
        delete [] mCircularBufferLeft;
        mCircularBufferLeft = nullptr;
    }

    if (mCircularBufferRight != nullptr) {
        delete [] mCircularBufferRight;
        mCircularBufferRight = nullptr;
    }
}

//==============================================================================
const juce::String ChaorusFlangosAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ChaorusFlangosAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ChaorusFlangosAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ChaorusFlangosAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ChaorusFlangosAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ChaorusFlangosAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ChaorusFlangosAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ChaorusFlangosAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ChaorusFlangosAudioProcessor::getProgramName (int index)
{
    return {};
}

void ChaorusFlangosAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ChaorusFlangosAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    /* Initialize data for the current sample rate and reset things such as phase and writeheads */
    mLFOPhase = 0;
    mCircularBufferLength = sampleRate * MAX_DELAY_TIME;

    // Clearing the buffer shit
    if (mCircularBufferLeft != nullptr ) {
        delete [] mCircularBufferLeft;
        mCircularBufferLeft = nullptr;
    }

    if (mCircularBufferRight != nullptr ) {
        delete [] mCircularBufferRight;
        mCircularBufferRight = nullptr;
    }

    if (mCircularBufferLeft == nullptr) {
        mCircularBufferLeft = new float[mCircularBufferLength];
    }

    juce::zeromem(mCircularBufferLeft, mCircularBufferLength * sizeof(float));

    if (mCircularBufferRight == nullptr) {
        mCircularBufferRight = new float[mCircularBufferLength];
    }

    juce::zeromem(mCircularBufferRight, mCircularBufferLength * sizeof(float));

    mCircularBufferWriteHead = 0;
}

void ChaorusFlangosAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ChaorusFlangosAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ChaorusFlangosAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    /* Obtain the left and right audio data pointers */
    float* leftChannel = buffer.getWritePointer(0);
    float* rightChannel = buffer.getWritePointer(1);

    /* Iterate through all the samples in the buffer */
    for (int i = 0; i < buffer.getNumSamples(); i++) {
        /* Write into the circular buffer */
        mCircularBufferLeft[mCircularBufferWriteHead] = leftChannel[i] + mFeedbackLeft;
        mCircularBufferRight[mCircularBufferWriteHead] = rightChannel[i] + mFeedbackRight;

        /* Generate the left LFO output */
        float lfoOutLeft = sin(2*M_PI * mLFOPhase);

        /* Calculate the right channel lfo phase */
        float lfoPhaseRight = mLFOPhase + *mPhaseOffsetParameter;
        if (lfoPhaseRight > 1) {
            lfoPhaseRight -= 1;
        }

        /* Generate right LFO output */
        float lfoOutRight = sin(2*M_PI * lfoPhaseRight);

        /* Moving LFO phase forward */
        mLFOPhase += *mRateParameter / getSampleRate();

        if (mLFOPhase > 1) {
            mLFOPhase -= 1;
        }

        /* Control the LFO Depth */
        lfoOutLeft *= *mDepthParameter;
        lfoOutRight *= *mDepthParameter;

        float lfoOutMappedLeft = 0;
        float lfoOutMappedRight = 0;


        /* Map the LFO output to the delay times */
        // chorus
        if(*mTypeParameter == 0) {
            lfoOutMappedLeft = juce::jmap(lfoOutLeft, -1.f, 1.f, 0.005f, 0.03f);
            lfoOutMappedRight = juce::jmap(lfoOutRight, -1.f, 1.f, 0.005f, 0.03f);

        // flanger
        } else {
            lfoOutMappedLeft = juce::jmap(lfoOutLeft, -1.f, 1.f, 0.001f, 0.005f);
            lfoOutMappedRight = juce::jmap(lfoOutRight, -1.f, 1.f, 0.001f, 0.005f);
        }

        /* Calculate the delay lenghts in samples */
        float delayTimeSamplesLeft = getSampleRate() * lfoOutMappedLeft;
        float delayTimeSamplesRight = getSampleRate() * lfoOutMappedRight;



        /* Calculate the left read head position */
        float delayReadHeadLeft = mCircularBufferWriteHead - delayTimeSamplesLeft;
        if (delayReadHeadLeft < 0) {
            delayReadHeadLeft += mCircularBufferLength;
        }

        /* Calculate the right read head position */
        float delayReadHeadRight = mCircularBufferWriteHead - delayTimeSamplesRight;
        if (delayReadHeadRight < 0) {
            delayReadHeadRight += mCircularBufferLength;
        }

        /* Calculate linear interpolation points for left channel */
        int readHeadLeft_x = (int)delayReadHeadLeft;
        int readHeadLeft_x1 = readHeadLeft_x + 1;
        float readHeadFloatLeft = delayReadHeadLeft - readHeadLeft_x;

        if (readHeadLeft_x1 >= mCircularBufferLength) {
            readHeadLeft_x1 -= mCircularBufferLength;
        }

        /* Calculate linear interpolation points for right channel */
        int readHeadRight_x = (int)delayReadHeadRight;
        int readHeadRight_x1 = readHeadRight_x + 1;
        float readHeadFloatRight = delayReadHeadRight - readHeadRight_x;

        if (readHeadRight_x1 >= mCircularBufferLength) {
            readHeadRight_x1 -= mCircularBufferLength;
        }


        /* generate the actual samples */
        float delay_sample_left = lin_interp(mCircularBufferLeft[readHeadLeft_x],
                                             mCircularBufferLeft[readHeadLeft_x1],
                                             readHeadFloatLeft);
        float delay_sample_right = lin_interp(mCircularBufferRight[readHeadRight_x],
                                              mCircularBufferRight[readHeadRight_x1],
                                              readHeadFloatRight);

        mFeedbackLeft = delay_sample_left * *mFeedbackParameter;
        mFeedbackRight = delay_sample_right * *mFeedbackParameter;


        mCircularBufferWriteHead++;


        if (mCircularBufferWriteHead >= mCircularBufferLength) {
            mCircularBufferWriteHead = 0;
        }

        float dryAmount = 1 - *mDryWetParameter;
        float wetAmount = *mDryWetParameter;

        buffer.setSample(0, i, buffer.getSample(0, i) * dryAmount + delay_sample_left * wetAmount);
        buffer.setSample(1, i, buffer.getSample(1, i) * dryAmount + delay_sample_right * wetAmount);
    }
}

//==============================================================================
bool ChaorusFlangosAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ChaorusFlangosAudioProcessor::createEditor()
{
    return new ChaorusFlangosAudioProcessorEditor (*this);
}

//==============================================================================
void ChaorusFlangosAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    std::unique_ptr<juce::XmlElement> xml(new juce::XmlElement("ChaorusFlangos"));

    xml->setAttribute("DryWet", *mDryWetParameter);
    xml->setAttribute("Depth", *mDepthParameter);
    xml->setAttribute("Rate", *mRateParameter);
    xml->setAttribute("PhaseOffset", *mPhaseOffsetParameter);
    xml->setAttribute("Feedback", *mFeedbackParameter);
    xml->setAttribute("Type", *mTypeParameter);

    copyXmlToBinary(*xml, destData);
}

void ChaorusFlangosAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));

    if (xml.get() != nullptr && xml->hasTagName("ChaorusFlangos")) {
        *mDryWetParameter = xml->getDoubleAttribute("DryWet");
        *mDepthParameter = xml->getDoubleAttribute("Depth");
        *mRateParameter = xml->getDoubleAttribute("Rate");
        *mPhaseOffsetParameter = xml->getDoubleAttribute("PhaseOffset");
        *mFeedbackParameter = xml->getDoubleAttribute("Feedback");

        *mTypeParameter = xml->getIntAttribute("Type");
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChaorusFlangosAudioProcessor();
}

float ChaorusFlangosAudioProcessor::lin_interp(float sample_x, float sample_x1, float inPhase) {
    return (1 - inPhase) * sample_x + inPhase * sample_x1;
}
