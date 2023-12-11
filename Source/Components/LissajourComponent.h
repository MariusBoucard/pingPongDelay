#include   <JuceHeader.h>

//
// Some nice example drawing
class Lissajour   : public juce::Component,
                    private juce::Timer
{
public:
    enum ColourIDs
    {
        // we are safe from collissions, because we set the colours on every component directly from the stylesheet
        backgroundColourId,
        drawColourId,
        fillColourId
    };

    Lissajour()
    {
        // make sure you define some default colour, otherwise the juce lookup will choke
        setColour (backgroundColourId, juce::Colours::black);
        setColour (drawColourId, juce::Colours::green);
        setColour (fillColourId, juce::Colours::green.withAlpha (0.5f));

        startTimerHz (30);
    }

    void setFactor (float f)
    {
        factor = f;
    }

    void paint (juce::Graphics& g) override
    {
        const float radius = std::min (getWidth(), getHeight()) * 0.4f;
        const auto  centre = getLocalBounds().getCentre().toFloat();

        g.fillAll (findColour (backgroundColourId));
        juce::Path p;
        p.startNewSubPath (centre + juce::Point<float>(0, std::sin (phase)) * radius);
        for (auto i = 0.1f; i <= juce::MathConstants<float>::twoPi; i += 0.01f)
            p.lineTo (centre + juce::Point<float>(std::sin (i),
                                            std::sin (std::fmod (i * factor + phase,
                                                                 juce::MathConstants<float>::twoPi))) * radius);
        p.closeSubPath();

        g.setColour (findColour (drawColourId));
        g.strokePath (p, juce::PathStrokeType (2.0f));

        const auto fillColour = findColour (fillColourId);
        if (fillColour.isTransparent() == false)
        {
            g.setColour (fillColour);
            g.fillPath (p);
        }
    }

private:
    void timerCallback() override
    {
        phase += 0.1f;
        if (phase >= juce::MathConstants<float>::twoPi)
            phase -= juce::MathConstants<float>::twoPi;

        repaint();
    }

    float factor = 3.0f;
    float phase = 0.0f;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Lissajour)
};

// This class is creating and configuring your custom component
class LissajourItem : public foleys::GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (LissajourItem)

    LissajourItem (foleys::MagicGUIBuilder& builder, const juce::ValueTree& node) : foleys::GuiItem (builder, node)
    {
        // Create the colour names to have them configurable
        setColourTranslation ({
            {"lissajour-background", Lissajour::backgroundColourId},
            {"lissajour-draw", Lissajour::drawColourId},
            {"lissajour-fill", Lissajour::fillColourId} });

        addAndMakeVisible (lissajour);
    }

    std::vector<foleys::SettableProperty> getSettableProperties() const override
    {
        std::vector<foleys::SettableProperty> newProperties;

        newProperties.push_back ({ configNode, "factor", foleys::SettableProperty::Number, 1.0f, {} });

        return newProperties;
    }

    // Override update() to set the values to your custom component
    void update() override
    {
        auto factor = getProperty ("factor");
        lissajour.setFactor (factor.isVoid() ? 3.0f : float (factor));
    }

    juce::Component* getWrappedComponent() override
    {
        return &lissajour;
    }

private:
    Lissajour lissajour;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LissajourItem)
};