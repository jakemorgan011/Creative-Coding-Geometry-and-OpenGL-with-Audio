/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TheHorsePluginAudioProcessorEditor::TheHorsePluginAudioProcessorEditor (TheHorsePluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    gl = std::make_unique<OpenGLOut>();

    // Original rotation callback
    gl->update_callback = [&](float val){p.update_dt(std::abs(val));};

    // WASD position control callback - updates reverb based on 3D position
    gl->position_callback = [&](float x, float y, float z){
        p.setPositionParameters(x, y, z);
    };

    // Light position callback - updates reverb room characteristics
    gl->light_callback = [&](float x, float y, float z){
        p.setLightParameters(x, y, z);
    };

    gl->toBack();
    addAndMakeVisible(gl.get());
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
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
    // might have a pointer issue here.
    gl->setBounds(getLocalBounds());
}
