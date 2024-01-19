#include <JuceHeader.h>
#include "../../faustDSP/EchoMonoToStereo.h"

class DelayPingPongProcessor : public juce::AudioProcessor
{
public:
    DelayPingPongProcessor() : juce::AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo()).withOutput("Output", juce::AudioChannelSet::stereo())) {}

    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        fDSP.init(sampleRate); // replace sampleRate with your actual sample rate
    }
    void releaseResources() override {}

    void processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) override
    {
        juce::ScopedNoDenormals noDenormals;
        auto totalNumInputChannels = getTotalNumInputChannels();
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
            const float *inputChannelData[2];
            float *outputChannelData[2];

            inputChannelData[0] = buffer.getReadPointer(0);
            inputChannelData[1] = buffer.getReadPointer(1);
            outputChannelData[0] = buffer.getWritePointer(0);
            outputChannelData[1] = buffer.getWritePointer(1);
            updateDelayParameters();

            // Process the audio data with the Faust DSP object
            fDSP.compute(buffer.getNumSamples(), const_cast<float **>(inputChannelData), outputChannelData);
            // Pan value ranges from -1 (full left) to 1 (full right)

            // Calculate left and right volume multipliers based on pan value
            float leftVol = pan <= 0.0f ? 1.0f : 1.0f - pan * width;
            float rightVol = pan >= 0.0f ? 1.0f : 1.0f + pan * width;

            // Apply panning to the audio data
            for (int i = 0; i < buffer.getNumSamples(); ++i)
            {
                outputChannelData[0][i] = inputChannelData[0][i] * leftVol;
                outputChannelData[1][i] = inputChannelData[1][i] * rightVol;
            }
        }
    }

    void updateDelayParameters()
    {

        fDSP.setParamValue("delay", delay);
        fDSP.setParamValue("feedback", feedback);
        fDSP.setParamValue("delta", delta);
    }

    const juce::String getName() const override { return "DelayPingPongProcessor"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int index) override {}
    const juce::String getProgramName(int index) override { return {}; }
    void changeProgramName(int index, const juce::String &newName) override {}

    bool hasEditor() const override { return false; }
    juce::AudioProcessorEditor *createEditor() override { return nullptr; }

    bool isBusesLayoutSupported(const BusesLayout &layouts) const override
    {
        // Only mono/stereo and input/output must have same layout
        const int numChannels = layouts.getMainOutputChannelSet().size();
        return numChannels == layouts.getMainInputChannelSet().size() &&
               (numChannels == 1 || numChannels == 2);
    }

    void setDelay(float newGain) { delay = newGain; }
    void setFeedBack(float newGain) { feedback = newGain; }
    void setPan(float newGain)
    {
        pan = newGain;
    }

    void setWidth(float newWidth) { width = newWidth; }
    void getStateInformation(juce::MemoryBlock &destData) override {}
    void setStateInformation(const void *data, int sizeInBytes) override {}

    // TODO : handle the logic to get the input from GUI and convert it to the right value for the DSP
    void setNotesLength(std::string noteTyp, float bpm)
    {
        bpm = 100.0f;
        float quarterNote = (60.0f / bpm) * 1000;
        if(noteTyp == "1/1"){
            delay = quarterNote * 4;
        }
        if (noteTyp == "1/2")
        {
            delay = quarterNote * 2;
        }
        else if (noteTyp == "1/4")
        {
            delay = quarterNote;
        }
        else if (noteTyp == "1/8")
        {
            delay = quarterNote / 2;
        }
        else if (noteTyp == "1/16")
        {
            delay = quarterNote / 4;
        }
        else if (noteTyp == "1/32")
        {
            delay = quarterNote / 8;
        }
        else
        {
            delay = quarterNote;
        }
    }

private:
    EchoMonoToStereo fDSP;
    float delay = 0.5f;
    float feedback = 0.5f;
    float pan = 0.0f;
    float delta = 0.0f;
    float width = 0.0f;
};
