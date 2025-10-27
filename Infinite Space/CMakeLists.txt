cmake_minimum_required(VERSION 3.15)

project(InfiniteSpace VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(JUCE)

juce_add_binary_data(Assets
  SOURCES
    Source/assets/horse.obj
    Source/assets/rndy.obj
    Source/assets/spiky.obj
    Source/assets/hud.png
    Source/assets/lines.png
    Source/assets/turbine.png
    Source/assets/texture.png
)

juce_add_plugin(${PROJECT_NAME}
  COMPANY_NAME "TERMITE"
  IS_SYNTH FALSE
  NEED_MIDI_INPUT TRUE
  NEEDS_MIDI_OUTPUT FALSE
  IS_MIDI_EFFECT FALSE
  EDITOR_WANTS_KEYBOARD_FOCUS FALSE
  COPY_PLUGIN_AFTER_BUILD TRUE
  PLUGIN_MANUFACTURER_CODE Term
  PLUGIN_CODE insp
  FORMATS VST3 AU Standalone
  PRODUCT_NAME "Infinite Space"
)

target_sources(${PROJECT_NAME}
  PRIVATE
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/WavefrontObjParser.h
    Source/OpenGL.h
    Source/reverb.cpp
    Source/reverb.h
)

target_compile_definitions(${PROJECT_NAME}
  PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    # need for anything in binary data.
    Assets

    #core modules
    juce::juce_core
    juce::juce_events
    juce::juce_graphics
    juce::juce_data_structures

    # GUI modules
    juce::juce_gui_basics
    juce::juce_gui_extra

    # audio modules
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_utils

    # DSP module
    juce::juce_dsp

    # advanced modules
    juce::juce_opengl
    #juce::juce_video
    #juce::juce_cryptography
    #juce::juce_osc
    #juce::juce_product_unlocking
    #juce::juce_analytics

  PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

set_target_properties(${PROJECT_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
