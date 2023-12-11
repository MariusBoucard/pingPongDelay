#include <JuceHeader.h>

// Define a new MagicPlotSource subclass
class ThresholdLineSource : public foleys::MagicPlotSource
{
public:
    ThresholdLineSource(float threshold) : threshold(threshold) {}

    void prepareToPlay (double sampleRate, int samplesPerBlockExpected) override
    {
        // Add your implementation here
    }

    void pushSamples (const juce::AudioBuffer<float>& buffer) override
    {
        // Add your implementation here
    }
    void createPlotPaths(juce::Path& path, juce::Path& filledPath, juce::Rectangle<float> bounds, foleys::MagicPlotComponent&) override
    {
        auto lineHeight = threshold/(-40.0f);
        auto hauteur=  bounds.getHeight()*lineHeight;

        path.startNewSubPath(bounds.getX(),hauteur );
        path.lineTo(bounds.getRight(), hauteur);
    }

private:
    float threshold;
};
