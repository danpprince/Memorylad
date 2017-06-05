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
    setSize (400, 200);

    setLookAndFeel(&lookAndFeel);

    // Define the parameters of the sliders
    mDelayMixSlider.setRange (0.0f, 1.0f);
    mDelayMixSlider.setTextBoxStyle (Slider::TextBoxBelow, false, 100, 20);
    mDelayMixSlider.setValue (0.3f);
    mDelayMixSlider.setSliderStyle (Slider::RotaryVerticalDrag);
    
    mDelayMixLabel.setText ("Mix", dontSendNotification);
    mDelayMixLabel.attachToComponent (&mDelayMixSlider, false);
    mDelayMixLabel.setJustificationType(Justification::centred);
    
    mDelayTimeSlider.setRange (10.0f, 550.0f);
    mDelayTimeSlider.setTextValueSuffix (" ms");
    mDelayTimeSlider.setTextBoxStyle (Slider::TextBoxBelow, false, 100, 20);
    mDelayTimeSlider.setValue (200.0f);
    mDelayTimeSlider.setSliderStyle (Slider::RotaryVerticalDrag);

    mDelayTimeLabel.setText ("Delay Time", dontSendNotification);
    mDelayTimeLabel.attachToComponent (&mDelayTimeSlider, false);
    mDelayTimeLabel.setJustificationType(Justification::centred);
    
    mDelayFeedbackSlider.setRange (0.0f, 1.0f);
    mDelayFeedbackSlider.setTextBoxStyle (Slider::TextBoxBelow, false, 100, 20);
    mDelayFeedbackSlider.setValue (0.2f);
    mDelayFeedbackSlider.setSliderStyle (Slider::RotaryVerticalDrag);

    mDelayFeedbackLabel.setText ("Feedback", dontSendNotification);
    mDelayFeedbackLabel.attachToComponent (&mDelayFeedbackSlider, false);
    mDelayFeedbackLabel.setJustificationType(Justification::centred);
    
    // Add the slider and label to the editor
    addAndMakeVisible (&mDelayMixSlider);
    addAndMakeVisible (&mDelayMixLabel);
    addAndMakeVisible (&mDelayTimeSlider);
    addAndMakeVisible (&mDelayTimeLabel);
    addAndMakeVisible (&mDelayFeedbackSlider);
    addAndMakeVisible (&mDelayFeedbackLabel);

    // Place the sliders in the UI window
    mDelayMixSlider     .setBounds ( 50, 50, 100, 100);
    mDelayTimeSlider    .setBounds (150, 50, 100, 100);
    mDelayFeedbackSlider.setBounds (250, 50, 100, 100);
    mDelayMixSlider     .addListener (this);
    mDelayTimeSlider    .addListener (this);
    mDelayFeedbackSlider.addListener (this);

    // Update the UI with parameter information this often:
    startTimer(40);
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
    if (slider == &mDelayMixSlider) {
        *mProcessor.mDelayMix      = mDelayMixSlider.getValue();
    } else if (slider == &mDelayTimeSlider) {
        *mProcessor.mDelayTime     = mDelayTimeSlider.getValue();
    } else if (slider == &mDelayFeedbackSlider) {
        *mProcessor.mDelayFeedback = mDelayFeedbackSlider.getValue();
    }
}

void MemoryLadAudioProcessorEditor::timerCallback()
{
    // Callback for the Timer that is run periodically. Update Sliders here
    // based on the AudioParameter values so that the UI reacts to automation.
    mDelayMixSlider     .setValue(*mProcessor.mDelayMix,      dontSendNotification);
    mDelayTimeSlider    .setValue(*mProcessor.mDelayTime,     dontSendNotification);
    mDelayFeedbackSlider.setValue(*mProcessor.mDelayFeedback, dontSendNotification);
}
