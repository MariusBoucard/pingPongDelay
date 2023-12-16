#include   <JuceHeader.h>

//
// Some nice example drawing
class StarDance   : public juce::Component,
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

    StarDance()
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
        
        const auto width = getWidth();

        // draw background
        g.fillAll (findColour (backgroundColourId));
        
        
        // draw stars
        const char* imageGalaxyData = BinaryData::stars_jpeg;
        int imageGalaxySize = BinaryData::stars_jpegSize;
        juce::Image galaxyImage = juce::ImageCache::getFromMemory(imageGalaxyData, imageGalaxySize);
        g.drawImage(galaxyImage, getLocalBounds().getX(), getLocalBounds().getY(), getWidth(), getHeight(),0,0, galaxyImage.getWidth(), galaxyImage.getHeight());

        //-------------------------//
        // draw planet rotating     //
        //-------------------------//
        const char* imageData = BinaryData::saturne_bleu_2_png;
        int imageSize = BinaryData::saturne_bleu_2_pngSize;
        juce::Image myKnobImage = juce::ImageCache::getFromMemory(imageData, imageSize);
        
        juce::AffineTransform rotation;
        rotation=rotation.rotated(phase, myKnobImage.getWidth()/2, myKnobImage.getHeight()/2);
        
        // Create an intermediate image with the same size as the original image
        juce::Image rotatedImage(juce::Image::ARGB, myKnobImage.getWidth(), myKnobImage.getHeight(), true);

        // Create a Graphics context for the rotated image
        juce::Graphics g2(rotatedImage);

        // Apply a rotation transform to the Graphics context
        g2.addTransform(rotation);

        // Draw the original image onto the rotated image with the applied transform
        g2.drawImageAt(myKnobImage, 0, 0);
        
        g.drawImage(rotatedImage, centre.getX() - width/4, centre.getY() - width/4, width/2, width/2, 0, 0, myKnobImage.getWidth(), myKnobImage.getHeight());
        
        //-------------------------//
        // draw josip rotating     //
        //-------------------------//
        imageData = BinaryData::josip_png;
        imageSize = BinaryData::josip_pngSize;
        juce::Image myJosipImage = juce::ImageCache::getFromMemory(imageData, imageSize);
        
        juce::AffineTransform rotation2;
        rotation2=rotation2.rotated(phase, myJosipImage.getWidth()/2, myJosipImage.getHeight()/2);
        
        // Create an intermediate image with the same size as the original image
        juce::Image rotatedImage2(juce::Image::ARGB, myJosipImage.getWidth(), myJosipImage.getHeight(), true);

        // Create a Graphics context for the rotated image
        juce::Graphics g3(rotatedImage2);

        // Apply a rotation transform to the Graphics context
        g3.addTransform(rotation);

        // Draw the original image onto the rotated image with the applied transform
        g3.drawImageAt(myJosipImage, 0, 0);
        
        g.drawImage(rotatedImage2, centre.getX() - width/4, centre.getY() - width/4, width/2, width/2, 0, 0, myJosipImage.getWidth(), myJosipImage.getHeight());
       
    }

private:
    void timerCallback() override
    {
        phase += 0.1f;
        if (phase >= juce::MathConstants<float>::twoPi)
        {
            phase -= 2*juce::MathConstants<float>::twoPi;
        }
       
        repaint();
    }

    float factor = 3.0f;
    float phase = 0.0f;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StarDance)
};

// This class is creating and configuring your custom component
class StarDanceItem : public foleys::GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (StarDanceItem)

    StarDanceItem (foleys::MagicGUIBuilder& builder, const juce::ValueTree& node) : foleys::GuiItem (builder, node)
    {
        // Create the colour names to have them configurable
        setColourTranslation ({
            {"starDance-background", StarDance::backgroundColourId},
            {"starDance-draw", StarDance::drawColourId},
            {"starDance-fill", StarDance::fillColourId} });

        addAndMakeVisible (starDance);
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
        starDance.setFactor (factor.isVoid() ? 3.0f : float (factor));
    }

    juce::Component* getWrappedComponent() override
    {
        return &starDance;
    }

private:
    StarDance starDance;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StarDanceItem)
};
