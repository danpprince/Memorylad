/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
MemoryLadAudioProcessor::MemoryLadAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
    // MemoryBoy has 550ms of delay time, use the same value here
    : mDelayBuffer(),
      mDelayBufferDur(550)
{
    mDelayBufferIdx = 0;
    addParameter (mDelayTime = new AudioParameterFloat (
                                           "delay_time", // Parameter ID
                                           "Delay Time", // Parameter name
                                           NormalisableRange<float> (10.0f, 550.0f), // Param range
                                           0.5f));       // Default value

    addParameter (mDelayFeedback = new AudioParameterFloat (
                                           "feedback", // Parameter ID
                                           "Feedback", // Parameter name
                                           0.0f,       // Min value
                                           1.0f,       // Max value
                                           0.2f));     // Default value

    addParameter (mDelayMix = new AudioParameterFloat (
                                           "mix",  // Parameter ID
                                           "Mix",  // Parameter name
                                           0.0f,   // Min value
                                           1.0f,   // Max value
                                           0.3f)); // Default value
}

MemoryLadAudioProcessor::~MemoryLadAudioProcessor()
{
}

//==============================================================================
const String MemoryLadAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MemoryLadAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MemoryLadAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

double MemoryLadAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MemoryLadAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MemoryLadAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MemoryLadAudioProcessor::setCurrentProgram (int index)
{
}

const String MemoryLadAudioProcessor::getProgramName (int index)
{
    return String();
}

void MemoryLadAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void MemoryLadAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Initialize the delay buffer. Convert mDelayBufferDur from ms to seconds.
    mDelayBufferLen = (int) (mDelayBufferDur / 1000.0 * sampleRate);
    mDelayBuffer.setSize(getTotalNumOutputChannels(), mDelayBufferLen);
    mSampleRate = sampleRate;
}

void MemoryLadAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MemoryLadAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void MemoryLadAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    const int totalNumInputChannels  = getTotalNumInputChannels();

    // Read the input channels. Assumes that there are an equal number of 
    // input and output channels.

    // Calculate the number of samples to delay based on the parameter value
    int delaySamples = (*mDelayTime) / 1000.0 * mSampleRate;

    int iDelayBuffer;
    for (int iChannel = 0; iChannel < totalNumInputChannels; ++iChannel)
    {
        iDelayBuffer = mDelayBufferIdx;
        float* delayBuffer = mDelayBuffer.getWritePointer(iChannel);
        float* channelBuffer = buffer.getWritePointer(iChannel);

        for (int iSample = 0; iSample < buffer.getNumSamples(); ++iSample)
        {
            // Get the index in the delay buffer for the current output sample and
            // wrap around the beginning of the buffer if the index is negative
            int iDelayOutput = iDelayBuffer - delaySamples;
            if (iDelayOutput < 0.0) 
            {
                iDelayOutput = mDelayBufferLen + iDelayOutput;
            }

            // Read sample from the input buffer
            float inSample = channelBuffer[iSample];

            // Write sample to the output buffer from the delay buffer
            channelBuffer[iSample] 
                = (*mDelayMix) * delayBuffer[iDelayOutput] 
                + (1 - *mDelayMix) * inSample;

            // Write sample from the input buffer to the delay buffer plus its
            // current value scaled by the feedback parameter
            delayBuffer[iDelayBuffer] 
                = inSample + (*mDelayFeedback) * delayBuffer[iDelayOutput];

            // Advance the delay buffer index
            iDelayBuffer = (iDelayBuffer+1) % (mDelayBufferLen-1);
        }
    }
    mDelayBufferIdx = iDelayBuffer;
}

//==============================================================================
bool MemoryLadAudioProcessor::hasEditor() const
{
    return false; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* MemoryLadAudioProcessor::createEditor()
{
    return NULL;
    // return new MemoryLadAudioProcessorEditor (*this);
}

//==============================================================================
void MemoryLadAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MemoryLadAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MemoryLadAudioProcessor();
}
