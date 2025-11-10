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
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TheHorsePluginAudioProcessorEditor::TheHorsePluginAudioProcessorEditor (TheHorsePluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    gl = std::make_unique<OpenGLOut>();

    // all of the lambda callbacks...
    // again, don't do this.
    gl->update_callback = [&](float val){p.update_dt(std::abs(val));};
    gl->position_callback = [&](float x, float y, float z){
        p.setPositionParameters(x, y, z);
    };
    gl->light_callback = [&](float x, float y, float z){
        p.setLightParameters(x, y, z);
    };

    gl->toBack();
    addAndMakeVisible(gl.get());
    setSize (500, 300);
}

TheHorsePluginAudioProcessorEditor::~TheHorsePluginAudioProcessorEditor()
{
}

//==============================================================================
void TheHorsePluginAudioProcessorEditor::paint (juce::Graphics& g)
{
}

void TheHorsePluginAudioProcessorEditor::resized()
{
    gl->setBounds(getLocalBounds());
}
