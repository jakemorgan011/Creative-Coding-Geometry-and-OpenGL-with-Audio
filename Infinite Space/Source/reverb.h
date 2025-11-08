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
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

namespace termite{

// not including indexes but this could be a better solution with the indeces.
static float interpolate_sample(float in_sample, float last_sample, float next_sample, int in_index, int last_index
    ){
    //memory wasting. could do it in one line.
    float return_sample = 0.f;
    return_sample = last_sample + next_sample / 2;
    return_sample += (return_sample * in_sample);
    return return_sample;
}

class delay{
public:
    delay();
    virtual ~delay();
protected:
    double ms_to_samps(double sample_rate, float in_ms){
        return (in_ms * sample_rate)/ 1000;
    }
    juce::LinearSmoothedValue<float> smoothed_delay_time;
    juce::LinearSmoothedValue<float> smoothed_feedback;
    juce::AudioBuffer<float> circular_buffer;
    std::vector<int> readheads{0,0};
    std::vector<int> writeheads{0,0};
    double f_sample_rate;
};

class allpass_filter : public delay{
public:
    allpass_filter();
    ~allpass_filter();
    void prepare_to_play(double sample_rate);
    void process_block(juce::AudioBuffer<float>& in_buffer);
    void set_parameters(const float delay_time_ms, const float fb_gain);
private:
    std::vector<float> last_delayed_sample{0.0f, 0.0f};
};

class comb_filter{
public:
    comb_filter();
    ~comb_filter();
    void prepare_to_play(double sample_rate);
    void process_block(juce::AudioBuffer<float>& in_buffer);
    void set_parameters(const float delay_time_ms, const float fb_gain);
private:
    double ms_to_samps(double sample_rate, float in_ms){
        return (in_ms * sample_rate)/ 1000;
    }
    juce::LinearSmoothedValue<float> smoothed_delay_time;
    juce::LinearSmoothedValue<float> smoothed_feedback;
    juce::AudioBuffer<float> circular_buffer;
    std::vector<int> readheads{0,0};
    std::vector<int> writeheads{0,0};
    std::vector<float> last_delayed_sample{0.0f, 0.0f};
    double f_sample_rate;
};

class delay_matrix{
public:
    delay_matrix();
    ~delay_matrix();
    void prepare_to_play(double sample_rate);
    void process_block(juce::AudioBuffer<float>& in_buffer);
    void set_parameters(const float in_delay_time);
    void set_parameters(const float in_delay_time, const float feedback, const float diffusion, const float damping);
    void set_bandpass_frequency(float frequency);  // Control bandpass from rotation
    void set_rotation_modulation(float rotation);  // Modulate allpass timing for tape warble effect
    void comb_matrix(juce::AudioBuffer<float>& in_buffer);
    delay_matrix* get_self();
private:
    juce::LinearSmoothedValue<float> smoothed_delay_time;
    juce::LinearSmoothedValue<float> smoothed_feedback;
    juce::LinearSmoothedValue<float> smoothed_diffusion;
    juce::LinearSmoothedValue<float> smoothed_damping;
    std::vector<comb_filter> comb_filters;
    std::vector<allpass_filter> allpass_filters;
    std::vector<juce::AudioBuffer<float>> comb_buffers;
    const int num_allpass = 2;
    const int num_comb = 4;
    
    // these values will BLOW UP the processor so go crazy and change them a bunch.
    float comb_gain = 0.7f;
    float allpass_gain = 0.85f;

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> bandpassFilter;
    juce::LinearSmoothedValue<float> smoothed_bandpass_freq;
    double current_sample_rate = 44100.0;

    float current_rotation = 0.0f;
    float base_allpass_times[2] = {5.0f, 1.7f}; 
    float current_delay_time = 300.0f; 
    float current_feedback = 0.85f;
    float current_diffusion = 0.8f;
    float current_damping = 0.2f;
};

}
