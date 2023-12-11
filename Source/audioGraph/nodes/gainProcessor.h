#include <JuceHeader.h>
// #include <juce_audio_processors/juce_audio_processors.h>

class GainProcessor : public juce::AudioProcessor
{
public:
    GainProcessor() : juce::AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo()).withOutput("Output", juce::AudioChannelSet::stereo())) {}

    void prepareToPlay(double sampleRate, int samplesPerBlock) override {}
    void releaseResources() override {}

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override
    {
        for (int channel = 0; channel < getTotalNumOutputChannels(); ++channel)
        {
            buffer.applyGain(channel, 0, buffer.getNumSamples(), gainFactor);
        }
    }

    const juce::String getName() const override { return "GainProcessor"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int index) override {}
    const juce::String getProgramName(int index) override { return {}; }
    void changeProgramName(int index, const juce::String& newName) override {}

    bool hasEditor() const override { return false; }
    juce::AudioProcessorEditor* createEditor() override { return nullptr; }

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override
    {
        // Only mono/stereo and input/output must have same layout
        const int numChannels = layouts.getMainOutputChannelSet().size();
        return numChannels == layouts.getMainInputChannelSet().size() &&
               (numChannels == 1 || numChannels == 2);
    }

    void setGain(float newGain) { gainFactor = newGain; }
    void getStateInformation (juce::MemoryBlock& destData) override {}
    void setStateInformation (const void* data, int sizeInBytes) override {}
private:
    float gainFactor = 1.0f;
};