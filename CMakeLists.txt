
cmake_minimum_required(VERSION 3.22)

project(KLUTCH11VOXSHIFT VERSION 1.0.0)

include(FetchContent)

FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.10
)

FetchContent_MakeAvailable(JUCE)

juce_add_plugin(KLUTCH11VOXSHIFT
    COMPANY_NAME "Klutch 11"
    BUNDLE_ID com.klutch11.voxshift
    PLUGIN_MANUFACTURER_CODE KL11
    PLUGIN_CODE VxSh
    PRODUCT_NAME "KLUTCH 11 VOXSHIFT"
    FORMATS VST3
    VST3_CATEGORIES Fx Stereo
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
)

juce_generate_juce_header(KLUTCH11VOXSHIFT)

target_sources(KLUTCH11VOXSHIFT PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
)

target_compile_definitions(KLUTCH11VOXSHIFT PRIVATE
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(KLUTCH11VOXSHIFT PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)
