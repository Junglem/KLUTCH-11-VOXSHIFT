#include "PluginEditor.h"

VoxShiftAudioProcessorEditor::VoxShiftAudioProcessorEditor(
    VoxShiftAudioProcessor& processor)
    : AudioProcessorEditor(&processor),
      audioProcessor(processor)
{
    setSize(760, 560);

    setupSlider(pitchSlider, pitchLabel, "PITCH");
    setupSlider(formantSlider, formantLabel, "FORMANT");
    setupSlider(reverbSlider, reverbLabel, "REVERB");
    setupSlider(lowPassSlider, lowPassLabel, "LOW PASS");
    setupSlider(highPassSlider, highPassLabel, "HIGH PASS");
    setupSlider(bandPassSlider, bandPassLabel, "BAND PASS");
    setupSlider(widthSlider, widthLabel, "WIDTH");
    setupSlider(delaySlider, delayLabel, "DELAY");
    setupSlider(driveSlider, driveLabel, "DRIVE");
    setupSlider(mixSlider, mixLabel, "MIX");
    setupSlider(outputSlider, outputLabel, "OUTPUT");

    setupAttachment(pitchAttachment, pitchSlider, "PITCH");
    setupAttachment(formantAttachment, formantSlider, "FORMANT");
    setupAttachment(reverbAttachment, reverbSlider, "REVERB");
    setupAttachment(lowPassAttachment, lowPassSlider, "LOWPASS");
    setupAttachment(highPassAttachment, highPassSlider, "HIGHPASS");
    setupAttachment(bandPassAttachment, bandPassSlider, "BANDPASS");
    setupAttachment(widthAttachment, widthSlider, "WIDTH");
    setupAttachment(delayAttachment, delaySlider, "DELAY");
    setupAttachment(driveAttachment, driveSlider, "DRIVE");
    setupAttachment(mixAttachment, mixSlider, "MIX");
    setupAttachment(outputAttachment, outputSlider, "OUTPUT");
}

void VoxShiftAudioProcessorEditor::setupSlider(
    juce::Slider& slider,
    juce::Label& label,
    const juce::String& name)
{
    addAndMakeVisible(slider);
    addAndMakeVisible(label);

    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 75, 20);
    slider.setColour(juce::Slider::rotarySliderFillColourId,
                     juce::Colour(0xffa855f7));
    slider.setColour(juce::Slider::rotarySliderOutlineColourId,
                     juce::Colour(0xff30243d));
    slider.setColour(juce::Slider::thumbColourId,
                     juce::Colour(0xffe9d5ff));
    slider.setColour(juce::Slider::textBoxTextColourId,
                     juce::Colours::white);
    slider.setColour(juce::Slider::textBoxBackgroundColourId,
                     juce::Colour(0xff17121e));
    slider.setColour(juce::Slider::textBoxOutlineColourId,
                     juce::Colour(0xff49345c));

    label.setText(name, juce::dontSendNotification);
    label.setFont(juce::Font(12.0f, juce::Font::bold));
    label.setColour(juce::Label::textColourId,
                    juce::Colour(0xffd8b4fe));
    label.setJustificationType(juce::Justification::centred);
}

void VoxShiftAudioProcessorEditor::setupAttachment(
    std::unique_ptr<SliderAttachment>& attachment,
    juce::Slider& slider,
    const juce::String& parameterID)
{
    attachment = std::make_unique<SliderAttachment>(
        audioProcessor.parameters, parameterID, slider);
}

void VoxShiftAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff0b0810));

    auto bounds = getLocalBounds().toFloat();

    g.setColour(juce::Colour(0xff171020));
    g.fillRoundedRectangle(bounds.reduced(10.0f), 16.0f);

    g.setColour(juce::Colour(0xffa855f7));
    g.drawRoundedRectangle(bounds.reduced(10.0f), 16.0f, 1.5f);

    g.setColour(juce::Colour(0xffe9d5ff));
    g.setFont(juce::Font(27.0f, juce::Font::bold));
    g.drawText("KLUTCH 11", 28, 18, 250, 34,
               juce::Justification::centredLeft);

    g.setColour(juce::Colour(0xffa855f7));
    g.setFont(juce::Font(17.0f, juce::Font::bold));
    g.drawText("VOXSHIFT", 28, 49, 250, 26,
               juce::Justification::centredLeft);

    g.setColour(juce::Colour(0xff887995));
    g.setFont(juce::Font(10.0f));
    g.drawText("TRANSFORM YOUR VOICE. OWN YOUR SOUND.",
               30, 77, 400, 18, juce::Justification::centredLeft);

    g.setColour(juce::Colour(0xff49345c));
    g.drawLine(28.0f, 105.0f, 732.0f, 105.0f, 1.0f);

    g.setColour(juce::Colour(0xffd8b4fe));
    g.setFont(juce::Font(11.0f, juce::Font::bold));
    g.drawText("VOICE TRANSFORMATION", 30, 116, 300, 20,
               juce::Justification::centredLeft);

    g.setColour(juce::Colour(0xffd8b4fe));
    g.drawText("FILTERS & EFFECTS", 30, 272, 300, 20,
               juce::Justification::centredLeft);

    g.setColour(juce::Colour(0xff49345c));
    g.drawLine(28.0f, 265.0f, 732.0f, 265.0f, 1.0f);

    g.setColour(juce::Colour(0xff887995));
    g.setFont(juce::Font(10.0f));
    g.drawText("KLUTCH 11 AUDIO • VST3",
               30, 525, 300, 16,
               juce::Justification::centredLeft);
}

void VoxShiftAudioProcessorEditor::resized()
{
    const int sliderWidth = 116;
    const int sliderHeight = 115;
    const int gap = 9;

    const int startX = 30;
    const int topY = 135;

    juce::Slider* topSliders[] = {
        &pitchSlider, &formantSlider, &reverbSlider
    };

    juce::Label* topLabels[] = {
        &pitchLabel, &formantLabel, &reverbLabel
    };

    for (int i = 0; i < 3; ++i)
    {
        const int x = startX + i * (sliderWidth + gap);
        topLabels[i]->setBounds(x, topY, sliderWidth, 20);
        topSliders[i]->setBounds(x, topY + 18, sliderWidth, sliderHeight);
    }

    juce::Slider* effectSliders[] = {
        &lowPassSlider, &highPassSlider, &bandPassSlider,
        &widthSlider, &delaySlider, &driveSlider,
        &mixSlider, &outputSlider
    };

    juce::Label* effectLabels[] = {
        &lowPassLabel, &highPassLabel, &bandPassLabel,
        &widthLabel, &delayLabel, &driveLabel,
        &mixLabel, &outputLabel
    };

    const int effectWidth = 82;
    const int effectGap = 6;
    const int effectY = 300;

    for (int i = 0; i < 8; ++i)
    {
        const int x = 30 + i * (effectWidth + effectGap);
        effectLabels[i]->setBounds(x, effectY, effectWidth, 20);
        effectSliders[i]->setBounds(x, effectY + 18, effectWidth, 112);
    }
}
