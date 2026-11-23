#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class VoxShiftAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit VoxShiftAudioProcessorEditor(VoxShiftAudioProcessor&);
    ~VoxShiftAudioProcessorEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    VoxShiftAudioProcessor& audioProcessor;

    juce::Slider pitchSlider;
    juce::Slider formantSlider;
    juce::Slider reverbSlider;
    juce::Slider lowPassSlider;
    juce::Slider highPassSlider;
    juce::Slider bandPassSlider;
    juce::Slider widthSlider;
    juce::Slider delaySlider;
    juce::Slider driveSlider;
    juce::Slider mixSlider;
    juce::Slider outputSlider;

    juce::Label pitchLabel;
    juce::Label formantLabel;
    juce::Label reverbLabel;
    juce::Label lowPassLabel;
    juce::Label highPassLabel;
    juce::Label bandPassLabel;
    juce::Label widthLabel;
    juce::Label delayLabel;
    juce::Label driveLabel;
    juce::Label mixLabel;
    juce::Label outputLabel;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    std::unique_ptr<SliderAttachment> pitchAttachment;
    std::unique_ptr<SliderAttachment> formantAttachment;
    std::unique_ptr<SliderAttachment> reverbAttachment;
    std::unique_ptr<SliderAttachment> lowPassAttachment;
    std::unique_ptr<SliderAttachment> highPassAttachment;
    std::unique_ptr<SliderAttachment> bandPassAttachment;
    std::unique_ptr<SliderAttachment> widthAttachment;
    std::unique_ptr<SliderAttachment> delayAttachment;
    std::unique_ptr<SliderAttachment> driveAttachment;
    std::unique_ptr<SliderAttachment> mixAttachment;
    std::unique_ptr<SliderAttachment> outputAttachment;

    void setupSlider(juce::Slider&, juce::Label&, const juce::String&);
    void setupAttachment(
        std::unique_ptr<SliderAttachment>&,
        juce::Slider&,
        const juce::String&);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VoxShiftAudioProcessorEditor)
};
