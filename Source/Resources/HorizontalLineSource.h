#include <JuceHeader.h>
#ifndef HORIZONTAL_LINE_SOURCE_H
#define HORIZONTAL_LINE_SOURCE_H

#include "../JuceLibraryCode/JuceHeader.h"
#include "foleys_gui_magic/foleys_gui_magic.h"

class HorizontalLineSource : public foleys::MagicAnalyser
{
public:
    float yPosition = 0.0f;
    HorizontalLineSource() : yPosition(0.0f) {}
    ~HorizontalLineSource() override{};
    // TODO MAKE WAY TO MUCH CALCULATION
    // void prepareToPlay (double sampleRate, int samplesPerBlockExpected) override{} ;

    // void pushSamples (const juce::AudioBuffer<float>& buffer)   override{} ;

    float calcHeight(juce::Rectangle<float> bounds)
    {
        auto lineHeight = bounds.getHeight() * yPosition;
        return lineHeight;
    }
    void createScale(juce::Path &path, juce::Rectangle<float> bounds)
    {
        path.startNewSubPath(10, 0);
        path.lineTo(10, bounds.getHeight());
        path.closeSubPath();

        // Add scale on each five db
        for (int i = 0; i < 8; i++)
        {
            path.startNewSubPath(10, i * bounds.getHeight() / 8);
            path.lineTo(20, i * bounds.getHeight() / 8);
            path.closeSubPath();
            auto label = juce::String(i * 5) + "dB";
            auto font = juce::Font(12.0f);
            auto labelWidth = font.getStringWidth(label);
            auto labelHeight = font.getHeight();
            auto x = 25;
            auto y = i * bounds.getHeight() / 8 - labelHeight / 2;
            auto labelBounds = juce::Rectangle<float>(x, y, labelWidth, labelHeight);

            // Draw the label on the labelBounds rectangle

            path.addRectangle(labelBounds);
        }
    }
    void createPlotPaths(juce::Path &path, juce::Path &filledPath, juce::Rectangle<float> bounds, foleys::MagicPlotComponent &) override
    {
        path.clear();

        path.startNewSubPath(bounds.getX(), (calcHeight(bounds)));
        path.lineTo(bounds.getRight(), (calcHeight(bounds)));

        path.closeSubPath();
        createScale(path, bounds);
        // Add text values on the plot
        // auto font = juce::Font(12.0f);
        // auto labelWidth = font.getStringWidth("0dB");
        // auto labelHeight = font.getHeight();
        // auto x = bounds.getX() + 5;
        // auto y = calcHeight(bounds) - labelHeight / 2;
        // auto labelBounds = juce::Rectangle<float>(x, y, labelWidth, labelHeight);
        // juce::Graphics::drawText(*this, "0dB", labelBounds, juce::Justification::left, false);
    }

    void setYPosition(float newPosition)
    {
        yPosition = newPosition;
    }

    bool isActive() const { return active; }
    void setActive(bool shouldBeActive) { active = shouldBeActive; }

    juce::int64 getLastDataUpdate() const { return 0; }

    void resetLastDataFlag() { ; }

private:
    bool active = true;
};
#endif // HORIZONTAL_LINE_SOURCE_H