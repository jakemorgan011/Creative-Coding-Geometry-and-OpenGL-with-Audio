// Copyright (c) 2025, Jake Morgan, termite
// This code contains all the basic steps necessary to start understanding audio sculpting
// This code is free to use for any purpose.
/*
  _        __ _       _ _                                  
 (_)      / _(_)     (_) |                                 
  _ _ __ | |_ _ _ __  _| |_ ___   ___ _ __   __ _  ___ ___ 
 | | '_ \|  _| | '_ \| | __/ _ \ / __| '_ \ / _` |/ __/ _ \
 | | | | | | | | | | | | ||  __/ \__ \ |_) | (_| | (_|  __/
 |_|_| |_|_| |_|_| |_|_|\__\___| |___/ .__/ \__,_|\___\___|
                                     | |                   
                                     |_|                   
*/

#pragma once

//#include <JuceHeader.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>  // if you're using DSP
#include "PluginProcessor.h"

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
    TheHorsePluginAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TheHorsePluginAudioProcessorEditor)
};
