
#pragma once

#include <JuceHeader.h>

class VoxShiftAudioProcessor : public juce::AudioProcessor
{
public:
    VoxShiftAudioProcessor();
    ~VoxShiftAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "KLUTCH 11 VOXSHIFT"; }

    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 5.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    using APVTS = juce::AudioProcessorValueTreeState;

    APVTS parameters;

    static APVTS::ParameterLayout createParameterLayout();

private:
    juce::dsp::ProcessSpec spec;

    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> pitchDelay { 8192 };
    juce::dsp::DelayLine<float> echoDelay { 96000 };

    juce::dsp::Reverb reverb;

    juce::dsp::StateVariableTPTFilter<float> filterLeft;
    juce::dsp::StateVariableTPTFilter<float> filterRight;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VoxShiftAudioProcessor)
};
