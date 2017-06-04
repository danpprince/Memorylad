/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
MemoryLadAudioProcessorEditor::MemoryLadAudioProcessorEditor (MemoryLadAudioProcessor& p)
    : AudioProcessorEditor (&p), mProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    // Define the parameters of the slider
    mDelayTimeSlider.setRange (10.0f, 550.0f);
    mDelayTimeSlider.setTextValueSuffix(" ms");
    mDelayTimeSlider.setValue (200.0f);

    mDelayTimeLabel.setText ("Delay Time", dontSendNotification);
    mDelayTimeLabel.attachToComponent (&mDelayTimeSlider, true);
    
    // Add the slider and label to the editor
    addAndMakeVisible (&mDelayTimeSlider);
    addAndMakeVisible (&mDelayTimeLabel);

    // Place the delay time slider in the UI window
    mDelayTimeSlider.setBounds(100, 50, 250, 20);

    mDelayTimeSlider.addListener (this);

    startTimer(100);
}

MemoryLadAudioProcessorEditor::~MemoryLadAudioProcessorEditor()
{
}

//==============================================================================
void MemoryLadAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::white);

    g.setColour (Colours::black);
    g.setFont (15.0f);
}

void MemoryLadAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void MemoryLadAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    // Callback for when a slider is changed in the UI. AudioParameters that
    // correspond to each slider should be updated here.
    if (slider == &mDelayTimeSlider) {
        *mProcessor.mDelayTime = mDelayTimeSlider.getValue();
    }
}

void MemoryLadAudioProcessorEditor::timerCallback()
{
    // Callback for the Timer that is run periodically. Update Sliders here
    // based on the AudioParameter values so that the UI reacts to automation.
    mDelayTimeSlider.setValue(*mProcessor.mDelayTime, dontSendNotification);
}
