#include <JuceHeader.h>
#include "faustDSP/FaustEffect.h"

class DelayProcessor : public juce::AudioProcessor
{
public:
    DelayProcessor() : juce::AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo()).withOutput("Output", juce::AudioChannelSet::stereo())) {}

    void prepareToPlay(double sampleRate, int samplesPerBlock) override {
            fDSP.init(sampleRate); // replace sampleRate with your actual sample rate

    }
    void releaseResources() override {}

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override
    {
       juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...

    // Make sure we are processing stereo audio
    if (totalNumInputChannels == 2 && totalNumOutputChannels == 2)
    {
        // Get pointers to the input and output channel data
        const float* inputChannelData[2];
        float* outputChannelData[2];

        inputChannelData[0] = buffer.getReadPointer(0);
        inputChannelData[1] = buffer.getReadPointer(1);
        outputChannelData[0] = buffer.getWritePointer(0);
        outputChannelData[1] = buffer.getWritePointer(1);
        updateDelayParameters();

        // Process the audio data with the Faust DSP object
       fDSP.compute(buffer.getNumSamples(), const_cast<float**>(inputChannelData), outputChannelData);
    }
    }

    void updateDelayParameters(){
       
        fDSP.setParamValue("delay", delay);
        fDSP.setParamValue("feedback", feedback);
    }

    const juce::String getName() const override { return "DelayProcessor"; }
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

    void setDelay(float newGain) { delay = newGain; }
    void setFeedBack(float newGain) { feedback = newGain; }

    void getStateInformation (juce::MemoryBlock& destData) override {}
    void setStateInformation (const void* data, int sizeInBytes) override {}

private:
        mydsp fDSP;
    float delay = 0.5f;
    float feedback = 0.5f;

};