/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TheHorsePluginAudioProcessor::TheHorsePluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

TheHorsePluginAudioProcessor::~TheHorsePluginAudioProcessor()
{
}

//==============================================================================
const juce::String TheHorsePluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TheHorsePluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TheHorsePluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TheHorsePluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TheHorsePluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TheHorsePluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TheHorsePluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TheHorsePluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TheHorsePluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void TheHorsePluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void TheHorsePluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    smoothed_dry_wet.reset(sampleRate, 0.01);
    smoothed_delayt_time_ms.reset(sampleRate, 0.0025);

    // SUPER RESPONSIVE position smoothing (10ms = almost instant!)
    smoothed_posX.reset(sampleRate, 0.01);
    smoothed_posY.reset(sampleRate, 0.01);
    smoothed_posZ.reset(sampleRate, 0.01);

    // FAST light smoothing (20ms for quick changes)
    smoothed_lightX.reset(sampleRate, 0.02);
    smoothed_lightY.reset(sampleRate, 0.02);
    smoothed_lightZ.reset(sampleRate, 0.02);

    // Set initial values
    smoothed_posX.setCurrentAndTargetValue(positionX.load());
    smoothed_posY.setCurrentAndTargetValue(positionY.load());
    smoothed_posZ.setCurrentAndTargetValue(positionZ.load());

    smoothed_lightX.setCurrentAndTargetValue(lightX.load());
    smoothed_lightY.setCurrentAndTargetValue(lightY.load());
    smoothed_lightZ.setCurrentAndTargetValue(lightZ.load());

    verb.prepare_to_play(sampleRate);
}

void TheHorsePluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TheHorsePluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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

void TheHorsePluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if(buffer.getNumChannels() != 2){
        DBG("channel count mismatch.");
        return;
    }

    // Get smoothed spatial parameters
    float posX = smoothed_posX.getNextValue();
    float posY = smoothed_posY.getNextValue();
    float posZ = smoothed_posZ.getNextValue();

    float lightX = smoothed_lightX.getNextValue();
    float lightY = smoothed_lightY.getNextValue();
    float lightZ = smoothed_lightZ.getNextValue();

    // BASE REVERB SETTINGS - NUCLEAR MODE!
    float baseDelay = smoothed_delayt_time_ms.getNextValue();

    // === ABSOLUTELY INSANE SPATIAL REVERB MAPPING ===
    // Far away = echo chamber, close up = comb filter madness!

    // Z POSITION (W/S keys: DRAMATIC DEPTH CONTROL)
    // FAR AWAY (S key, posZ = -25): Echo chamber with minimal feedback (0.2)
    // CLOSE UP (W key, posZ = 5): Near-infinite feedback (0.995) = EXTREME COMB FILTER CHAOS!
    float feedback = juce::jmap(posZ, -25.0f, 5.0f, 0.2f, 0.995f);
    feedback = juce::jlimit(0.0f, 0.995f, feedback);

    // BONUS: When close, ALSO reduce delay time for SUPER TIGHT FLANGING
    float closenessFlange = juce::jmap(posZ, -25.0f, 5.0f, 1.0f, 0.15f);  // Close = much shorter delays!

    // X POSITION (A/D keys: ABSURD DELAY TIME CHANGES)
    // LEFT (A key, posX = -30): Tiny slapback (0.03x = ~9ms)
    // RIGHT (D key, posX = 30): MASSIVE cathedral delays (15x = ~4.5 seconds!)
    float delayMultiplier = juce::jmap(posX, -30.0f, 30.0f, 0.03f, 15.0f);

    // Combine X position with closeness for EXTREME flange when close + left
    float delayTime = baseDelay * delayMultiplier * closenessFlange;

    // Y POSITION (Q/E keys: EXTREME DIFFUSION)
    // DOWN (E key): Crystal clear discrete echoes (0.05)
    // UP (Q key): Total washout mud (0.99)
    float diffusion = juce::jmap(posY, -10.0f, 20.0f, 0.05f, 0.99f);
    diffusion = juce::jlimit(0.0f, 0.99f, diffusion);

    // Use very low damping for BRIGHT, PRESENT reverb
    float damping = 0.15f;  // Even brighter!

    // Set reverb parameters and process
    verb.set_parameters(delayTime, feedback, diffusion, damping);
    verb.process_block(buffer);

    // 100% WET - no dry/wet mixing for now
}

//==============================================================================
bool TheHorsePluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TheHorsePluginAudioProcessor::createEditor()
{
    return new TheHorsePluginAudioProcessorEditor (*this);
}

//==============================================================================
void TheHorsePluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void TheHorsePluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}
void TheHorsePluginAudioProcessor::update_dt(float in_time){
    smoothed_delayt_time_ms.setTargetValue(std::abs(in_time));
}

void TheHorsePluginAudioProcessor::setRotationParameters(float rotationX, float rotationY, float rotationZ)
{
    this->rotationX.store(rotationX);
    this->rotationY.store(rotationY);
    this->rotationZ.store(rotationZ);
}

void TheHorsePluginAudioProcessor::setPositionParameters(float x, float y, float z)
{
    positionX.store(x);
    positionY.store(y);
    positionZ.store(z);

    // Update smoothed values for audio processing
    smoothed_posX.setTargetValue(x);
    smoothed_posY.setTargetValue(y);
    smoothed_posZ.setTargetValue(z);
}

void TheHorsePluginAudioProcessor::setLightParameters(float x, float y, float z)
{
    lightX.store(x);
    lightY.store(y);
    lightZ.store(z);

    // Update smoothed values for reverb modulation
    smoothed_lightX.setTargetValue(x);
    smoothed_lightY.setTargetValue(y);
    smoothed_lightZ.setTargetValue(z);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TheHorsePluginAudioProcessor();
}
