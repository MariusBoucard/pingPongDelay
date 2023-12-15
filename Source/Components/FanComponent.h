#include <JuceHeader.h>
//
// Some nice example drawing
class Fan : public juce::Component,
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

    Fan()
    {
        // make sure you define some default colour, otherwise the juce lookup will choke
        setColour(backgroundColourId, juce::Colours::black);
        setColour(drawColourId, juce::Colours::green);
        setColour(fillColourId, juce::Colours::green.withAlpha(0.5f));

        startTimerHz(30);
    }

    void setFactor(float f)
    {
        factor = 0.0f;
    }
    float getFactor()
    {
        return factor;
    }

    void mouseDrag(const juce::MouseEvent &event) override
    {
        // Step 3: Change the width when the user drags the mouse
        auto angleInDegrees = event.getDistanceFromDragStartX();
        float angleInRadians = juce::degreesToRadians(angleInDegrees);

        factor += angleInDegrees / 500.0f;
        if (factor < 0.0f)
        {
            factor = 0.0f;
        }
        else if (factor > 1.0f)
        {
            factor = 1.0f;
        }
        repaint();
    }

    void paint(juce::Graphics &g) override
    {
            const auto centre = juce::Point<float>(getWidth() / 2.0f, getHeight() - (getHeight() / 4.0f));
            const float length = std::min(getWidth(), getHeight()) * 0.7f;

            float test = (1 - factor) * juce::MathConstants<float>::pi / 2.0f;
            // Calculate the end points of the lines
            juce::Point<float> endPoint1 = centre + juce::Point<float>(std::cos(test), -std::sin(test)) * length;
            juce::Point<float> endPoint2 = centre + juce::Point<float>(-std::cos(test), -std::sin(test)) * length;

            // Draw the lines
            g.setColour(findColour(drawColourId));
            g.drawLine(centre.x, centre.y, endPoint1.x, endPoint1.y, 2.0f);
            g.drawLine(centre.x, centre.y, endPoint2.x, endPoint2.y, 2.0f);

        //TODO
            juce::Path path;
            path.startNewSubPath(endPoint1);
    path.addArc(centre.x + length, centre.y + length, 2 * length, 2 * length, juce::MathConstants<float>::pi + factor, juce::MathConstants<float>::pi - factor, true);
    g.strokePath(path, juce::PathStrokeType(2.0f));

        // Calculate the end points of the lines
        g.setColour(juce::Colours::black);
        g.drawText("Factor: " + juce::String(factor), getLocalBounds(), juce::Justification::centredBottom);
    }

private:
    void timerCallback() override
    {
      
        repaint();
    }

    float factor = 0.0f;
    float phase = 0.0f;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Fan)
};

// This class is creating and configuring your custom component
class FanItem : public foleys::GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY(FanItem)

    FanItem(foleys::MagicGUIBuilder &builder, const juce::ValueTree &node) : foleys::GuiItem(builder, node)
    {
    
        // Create the colour names to have them configurable
        setColourTranslation({{"lissajour-background", Fan::backgroundColourId},
                              {"lissajour-draw", Fan::drawColourId},
                              {"lissajour-fill", Fan::fillColourId}});

        addAndMakeVisible(fan);
    }
    void setFactor(float f)
    {
        fan.setFactor(f);
    }
    float getFactor()
    {
        return fan.getFactor();
    }

    std::vector<foleys::SettableProperty> getSettableProperties() const override
    {
        std::vector<foleys::SettableProperty> newProperties;

        newProperties.push_back(
            {configNode, "factor", foleys::SettableProperty::Number, 1.0f, {}}
            );

        return newProperties;
    }

    // Override update() to set the values to your custom component
    void update() override  
    {
        auto factor = getProperty("factor");
        fan.setFactor(factor);
    }


    juce::Component *getWrappedComponent() override
    {
        return &fan;
    }

private:
    Fan fan;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FanItem)
};