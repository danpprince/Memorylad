/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
*/

class MemoryLadLookAndFeel : public LookAndFeel_V3
{
public:
    void drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override {

        const float radius = jmin (width / 2, height / 2) - 4.0f;
        const float centreX = x + width * 0.5f;
        const float centreY = y + height * 0.5f;
        const float rx = centreX - radius;
        const float ry = centreY - radius;
        const float rw = radius * 2.0f;
        const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        g.setGradientFill(ColourGradient(Colour( 33,  33,  33), rx + radius * 2 / 5, y + height,
                                         Colour(179, 179, 179), rx + radius * 4 / 5, y, false));
        g.fillEllipse (rx, ry, rw, rw);
        
        g.setColour (Colour(208, 208, 208));
        g.fillEllipse (rx + radius * 2 / 5, ry + radius * 2 / 5, rw * 3 / 5, rw * 3 / 5);

        // outline
        g.setColour (Colour(141, 141, 141));
        g.drawEllipse (rx, ry, rw, rw, 1.0f);

        Path p;
        const float pointerLength = radius;
        const float pointerThickness = 9.0f;
        p.addRectangle (-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform (AffineTransform::rotation (angle).translated (centreX, centreY));

        // pointer
        g.setColour (Colour(242, 242, 242));
        g.fillPath (p);
    }
};

class MemoryLadAudioProcessorEditor  : public AudioProcessorEditor,
                                       private Slider::Listener,
                                       private Timer
{
public:
    MemoryLadAudioProcessorEditor (MemoryLadAudioProcessor&);
    ~MemoryLadAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    void timerCallback() override;

private:
    void sliderValueChanged (Slider* slider) override;
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MemoryLadAudioProcessor& mProcessor;
    
    Slider mDelayMixSlider;
    Slider mDelayTimeSlider;
    Slider mDelayFeedbackSlider;
    Label mDelayMixLabel;
    Label mDelayTimeLabel;
    Label mDelayFeedbackLabel;

    
    MemoryLadLookAndFeel lookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MemoryLadAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
