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
    : mDelayBuffer(),
      // MemoryBoy has 550ms of delay time, use the same value here
      mDelayBufferDur(550),
      // 1 pole IIR LPF coefficient selected to provide an exponential ramp
      // on abrupt delay time parameter changes
      mDelayTimeFilterCoeff(0.999f),
      mDelayTimeSmoothed(200.0f)
{
    mDelayBufferIdx = 0;
    addParameter (mDelayTime = new AudioParameterFloat (
                                           "delay_time", // Parameter ID
                                           "Delay Time", // Parameter name
                                           NormalisableRange<float> (10.0f, 550.0f), // Param range
                                           mDelayTimeSmoothed));       // Default value

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
    mSampleRate = sampleRate;
    int nOutputChannels = getTotalNumOutputChannels();

    // Initialize the delay buffer. Convert mDelayBufferDur from ms to seconds.
    mDelayBufferLen = (int) (mDelayBufferDur / 1000.0 * sampleRate);
    mDelayBuffer.setSize(nOutputChannels, mDelayBufferLen);
    for (int iChannel = 0; iChannel < nOutputChannels; ++iChannel)
    {
        float* channelBuffer = mDelayBuffer.getWritePointer(iChannel);
        for (int iSample = 0; iSample < mDelayBuffer.getNumSamples(); ++iSample)
        {
            channelBuffer[iSample] = 0.0f;
        }
    }
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
    // Read the input channels. Assumes that there are an equal number of 
    // input and output channels.
    const int totalNumInputChannels = getTotalNumInputChannels();

    int iDelayBuffer;
    float smoothedDelayTime;
    float paramDelayTime = *mDelayTime;
    for (int iChannel = 0; iChannel < totalNumInputChannels; ++iChannel)
    {
        iDelayBuffer = mDelayBufferIdx;
        smoothedDelayTime = mDelayTimeSmoothed;
        float* delayBuffer = mDelayBuffer.getWritePointer(iChannel);
        float* channelBuffer = buffer.getWritePointer(iChannel);

        for (int iSample = 0; iSample < buffer.getNumSamples(); ++iSample)
        {
            // Calculate the number of samples to delay based on the parameter value
            float delaySamples = smoothedDelayTime / 1000.0 * mSampleRate;

            // Get the index in the delay buffer for the current output sample and
            // wrap around the beginning of the buffer if the index is negative
            float iCurrentDelay = iDelayBuffer - delaySamples;
            if (iCurrentDelay < 0.0f) 
            {
                iCurrentDelay = mDelayBufferLen + iCurrentDelay;
            }

            // Get the two samples adjacent to the index for use in interpolation
            float y[2];
            y[0] = delayBuffer[ (int) iCurrentDelay];
            y[1] = delayBuffer[((int)(iCurrentDelay+1)) % (mDelayBufferLen-1)];
            float interpSample = y[0] + (y[1] - y[0]) * (iCurrentDelay - floor(iCurrentDelay));

            float driveSample = tubeDistortion(interpSample);

            // Read sample from the input buffer
            float inputSample = channelBuffer[iSample];
            float outputSample 
                = (     *mDelayMix)  * driveSample
                + (1 - (*mDelayMix)) * inputSample;

            // Write sample from the input buffer to the delay buffer plus its
            // current value scaled by the feedback parameter
            delayBuffer[iDelayBuffer] = inputSample + (*mDelayFeedback) * driveSample;

            // Write sample to the output buffer from the delay buffer
            channelBuffer[iSample] = outputSample;

            // Advance the delay buffer index
            iDelayBuffer = (iDelayBuffer+1) % mDelayBufferLen;

            // Update the delay time using a one pole IIR filter to smooth
            // any abrupt parameter changes
            smoothedDelayTime 
                = mDelayTimeFilterCoeff * smoothedDelayTime 
                + (1 - mDelayTimeFilterCoeff) * paramDelayTime;
        }
    }
    mDelayBufferIdx = iDelayBuffer;
    mDelayTimeSmoothed = smoothedDelayTime;
}

//==============================================================================
bool MemoryLadAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* MemoryLadAudioProcessor::createEditor()
{
    return new MemoryLadAudioProcessorEditor (*this);
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

//==============================================================================
double MemoryLadAudioProcessor::tubeDistortion(double input) 
{
    float bias  = -0.1;
    float drive =  2.0;

    return (input - bias) / (1 - exp(-1 * drive * (input - bias)))
           + bias / (1 - exp(drive * bias));
}

