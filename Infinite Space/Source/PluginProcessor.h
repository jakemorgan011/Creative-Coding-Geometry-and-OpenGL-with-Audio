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

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
//#include <JuceHeader.h>
#include "OpenGL.h"
#include "reverb.h"

class TheHorsePluginAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    TheHorsePluginAudioProcessor();
    ~TheHorsePluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    juce::ValueTree& getVT(){
        return *ValueTree;
    }
    
    void setRotationParameters(float rotationX, float rotationY, float rotationZ);
    std::atomic<float>& getRotationX() { return rotationX; }
    std::atomic<float>& getRotationY() { return rotationY; }
    std::atomic<float>& getRotationZ() { return rotationZ; }

    void setPositionParameters(float x, float y, float z);
    std::atomic<float>& getPositionX() { return positionX; }
    std::atomic<float>& getPositionY() { return positionY; }
    std::atomic<float>& getPositionZ() { return positionZ; }

    void setLightParameters(float x, float y, float z);
    std::atomic<float>& getLightX() { return lightX; }
    std::atomic<float>& getLightY() { return lightY; }
    std::atomic<float>& getLightZ() { return lightZ; }

    void update_dt(float in_time);

private:
    termite::delay_matrix verb;
    
    std::atomic<float>* delay_time = nullptr;
    
    std::atomic<float> rotationX{0.0f};
    std::atomic<float> rotationY{0.0f};
    std::atomic<float> rotationZ{0.0f};

    std::atomic<float> positionX{0.0f};
    std::atomic<float> positionY{0.0f};  // Centered
    std::atomic<float> positionZ{-10.0f};

    std::atomic<float> lightX{0.0f};
    std::atomic<float> lightY{10.0f};
    std::atomic<float> lightZ{15.0f};

    // static juce::Identifier
    // eventually implement value tree for automation. even though i don't like that idea.
    std::unique_ptr<juce::ValueTree> ValueTree;
    juce::LinearSmoothedValue<float> smoothed_delayt_time_ms;
    juce::LinearSmoothedValue<float> smoothed_dry_wet;

    juce::LinearSmoothedValue<float> smoothed_posX;
    juce::LinearSmoothedValue<float> smoothed_posY;
    juce::LinearSmoothedValue<float> smoothed_posZ;

    juce::LinearSmoothedValue<float> smoothed_lightX;
    juce::LinearSmoothedValue<float> smoothed_lightY;
    juce::LinearSmoothedValue<float> smoothed_lightZ;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TheHorsePluginAudioProcessor)
};
