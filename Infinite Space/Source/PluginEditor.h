/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

//#include <JuceHeader.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>  // if you're using DSP
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class TheHorsePluginAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    TheHorsePluginAudioProcessorEditor (TheHorsePluginAudioProcessor&);
    ~TheHorsePluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    std::unique_ptr<OpenGLOut> gl;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    TheHorsePluginAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TheHorsePluginAudioProcessorEditor)
};
