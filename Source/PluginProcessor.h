/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"


//==============================================================================
/**
*/
class MemoryLadAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    MemoryLadAudioProcessor();
    ~MemoryLadAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //==============================================================================
    AudioParameterFloat* mDelayTime;
    AudioParameterFloat* mDelayFeedback;
    AudioParameterFloat* mDelayMix;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MemoryLadAudioProcessor)

    AudioSampleBuffer mDelayBuffer;
    double mSampleRate;
    const double mDelayBufferDur;
    int mDelayBufferIdx;
    int mDelayBufferLen;

    const float mDelayTimeFilterCoeff;
    float mDelayTimeSmoothed;
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
