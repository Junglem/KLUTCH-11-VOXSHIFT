#include "PluginProcessor.h"
#include "PluginEditor.h"

VoxShiftAudioProcessor::VoxShiftAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

VoxShiftAudioProcessor::APVTS::ParameterLayout
VoxShiftAudioProcessor::createParameterLayout()
{
    APVTS::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "PITCH", "Pitch",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.01f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "FORMANT", "Formant Color",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 1.0f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "REVERB", "Reverb",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 20.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "LOWPASS", "Low Pass",
        juce::NormalisableRange<float>(1000.0f, 20000.0f, 1.0f), 20000.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "HIGHPASS", "High Pass",
        juce::NormalisableRange<float>(20.0f, 5000.0f, 1.0f), 20.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "BANDPASS", "Band Pass",
        juce::NormalisableRange<float>(100.0f, 10000.0f, 1.0f), 1000.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "WIDTH", "Stereo Width",
        juce::NormalisableRange<float>(0.0f, 200.0f, 0.1f), 100.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "DELAY", "Delay",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "DRIVE", "Drive",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "MIX", "Wet Dry",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 100.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "OUTPUT", "Output",
        juce::NormalisableRange<float>(-24.0f, 12.0f, 0.1f), 0.0f));

    return layout;
}

void VoxShiftAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    filterLeft.prepare(spec);
    filterRight.prepare(spec);

    filterLeft.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    filterRight.setType(juce::dsp::StateVariableTPTFilterType::lowpass);

    filterLeft.setCutoffFrequency(20000.0f);
    filterRight.setCutoffFrequency(20000.0f);

    reverb.reset();

    juce::dsp::Reverb::Parameters reverbParams;
    reverbParams.roomSize = 0.5f;
    reverbParams.damping = 0.5f;
    reverbParams.wetLevel = 0.2f;
    reverbParams.dryLevel = 0.8f;
    reverbParams.width = 1.0f;
    reverb.setParameters(reverbParams);

    echoDelay.setMaximumDelayInSamples(static_cast<int>(sampleRate * 1.5));
    echoDelay.prepare(spec);
    echoDelay.reset();

    pitchDelay.prepare(spec);
    pitchDelay.reset();
}

void VoxShiftAudioProcessor::releaseResources()
{
    reverb.reset();
    echoDelay.reset();
    pitchDelay.reset();
    filterLeft.reset();
    filterRight.reset();
}

juce::AudioProcessorEditor* VoxShiftAudioProcessor::createEditor()
{
    return new VoxShiftAudioProcessorEditor(*this);
}

void VoxShiftAudioProcessor::processBlock(
    juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;

    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    juce::AudioBuffer<float> dryBuffer;
    dryBuffer.makeCopyOf(buffer);

    const float reverbAmount = parameters.getRawParameterValue("REVERB")->load() / 100.0f;
    const float lowPass = parameters.getRawParameterValue("LOWPASS")->load();
    const float highPass = parameters.getRawParameterValue("HIGHPASS")->load();
    const float bandPass = parameters.getRawParameterValue("BANDPASS")->load();
    const float drive = parameters.getRawParameterValue("DRIVE")->load() / 100.0f;
    const float mix = parameters.getRawParameterValue("MIX")->load() / 100.0f;
    const float outputGain = juce::Decibels::decibelsToGain(
        parameters.getRawParameterValue("OUTPUT")->load());

    const float delayAmount = parameters.getRawParameterValue("DELAY")->load() / 100.0f;
    const float width = parameters.getRawParameterValue("WIDTH")->load() / 100.0f;

    juce::dsp::Reverb::Parameters reverbParams;
    reverbParams.roomSize = 0.5f;
    reverbParams.damping = 0.4f;
    reverbParams.wetLevel = reverbAmount;
    reverbParams.dryLevel = 1.0f - reverbAmount;
    reverbParams.width = 1.0f;
    reverb.setParameters(reverbParams);

    filterLeft.setCutoffFrequency(juce::jlimit(20.0f, 20000.0f, lowPass));
    filterRight.setCutoffFrequency(juce::jlimit(20.0f, 20000.0f, lowPass));

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* data = buffer.getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float value = data[sample];

            value = channel == 0
                ? filterLeft.processSample(0, value)
                : filterRight.processSample(0, value);

            if (drive > 0.001f)
            {
                const float amount = 1.0f + drive * 12.0f;
                value = std::tanh(value * amount);
            }

            data[sample] = value;
        }
    }

    if (numChannels >= 2 && width != 1.0f)
    {
        auto* left = buffer.getWritePointer(0);
        auto* right = buffer.getWritePointer(1);

        for (int i = 0; i < numSamples; ++i)
        {
            const float mid = (left[i] + right[i]) * 0.5f;
            const float side = (left[i] - right[i]) * 0.5f * width;

            left[i] = mid + side;
            right[i] = mid - side;
        }
    }

    if (delayAmount > 0.001f)
    {
        const int delaySamples = juce::jlimit(
            1,
            static_cast<int>(spec.sampleRate * 1.4),
            static_cast<int>(spec.sampleRate * 0.35));

        echoDelay.setDelay(static_cast<float>(delaySamples));

        for (int channel = 0; channel < numChannels; ++channel)
        {
            auto* data = buffer.getWritePointer(channel);

            for (int i = 0; i < numSamples; ++i)
            {
                const float input = data[i];
                const float delayed = echoDelay.popSample(channel);
                echoDelay.pushSample(channel, input);

                data[i] = input + delayed * delayAmount * 0.6f;
            }
        }
    }

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    reverb.process(context);

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* wet = buffer.getWritePointer(channel);
        const auto* dry = dryBuffer.getReadPointer(channel);

        for (int i = 0; i < numSamples; ++i)
        {
            wet[i] = ((dry[i] * (1.0f - mix)) + (wet[i] * mix)) * outputGain;
        }
    }

    juce::ignoreUnused(highPass, bandPass);
}

void VoxShiftAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void VoxShiftAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));

    if (xml != nullptr && xml->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VoxShiftAudioProcessor();
}
