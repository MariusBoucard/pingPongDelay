#include <JuceHeader.h>
/*
This dry wet mixer is made to blend together 2 stereo signals
*/
class DryWetMixer : public juce::AudioProcessor
{
public:
    DryWetMixer()
        : juce::AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::discreteChannels(4))
                                               .withOutput("Output", juce::AudioChannelSet::stereo()))
    {
    }
       void prepareToPlay(double sampleRate, int samplesPerBlock) override {}
    void releaseResources() override {}


    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override
    {
     int numSamples = buffer.getNumSamples();

    for (int i = 0; i < numSamples; ++i)
    {
        for (int j = 0; j < 2; ++j) // Assuming stereo signal
        {
            auto* dryData = buffer.getWritePointer(j, i);
            auto* wetData = buffer.getWritePointer(j + 2, i); // Wet signal is in channels 2 and 3

            auto drySample = *dryData;
            auto wetSample = *wetData;

            // Mix the dry and wet samples
            auto mixedSample = drySample * (1.0f - mix) + wetSample * mix;

            // Write the mixed sample to the dry and wet channels
            *dryData = mixedSample;
            *wetData = mixedSample;
        }
    }
    }

    // ... other necessary AudioProcessor methods ...


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
    void getStateInformation (juce::MemoryBlock& destData) override {}
    void setStateInformation (const void* data, int sizeInBytes) override {}
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override
    {
        // Only mono/stereo and input/output must have same layout
        const int numChannels = layouts.getMainOutputChannelSet().size();
        return numChannels == layouts.getMainInputChannelSet().size() &&
               (numChannels == 1 || numChannels == 2);
    }
    void setMix(float newMix)
    {
        mix = newMix;
    }

private:
    float mix = 0.5f;  // 50% dry, 50% wet
};