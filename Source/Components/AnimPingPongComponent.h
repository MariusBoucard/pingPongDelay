#include   <JuceHeader.h>

//
// Some nice example drawing
class Animation   : public juce::Component,
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

    Animation()
    {
        int imageSize = BinaryData::frame_1_jpgSize;
        for (int i = 0; i <= 58; i++) {
            //const void* imageData = BinaryData::frame_1_jpg;
            //int imageSize = BinaryData::frame_1_jpgSize;
            juce::String imageName = "frame_"+juce::String(i)+"_jpg";
            const void* imageData = BinaryData::getNamedResource(imageName.toUTF8(), imageSize);
            juce::Image image = juce::ImageCache::getFromMemory(imageData, imageSize);
            
            images.add(image);
            }
        // make sure you define some default colour, otherwise the juce lookup will choke
        setColour (backgroundColourId, juce::Colours::black);
        setColour (drawColourId, juce::Colours::green);
        setColour (fillColourId, juce::Colours::green.withAlpha (0.5f));

        startTimerHz (30);
    }


    void paint (juce::Graphics& g) override
    {

        // draw background
       // g.fillAll (findColour (backgroundColourId));
        
        // draw pingpong
        
        //std::cout << indice;
        juce::Image image = images.getReference(indice);
        g.drawImage(image, getLocalBounds().getX(), getLocalBounds().getY(), getWidth(), getHeight(),0,0, image.getWidth(), image.getHeight());

        
       
    }

private:
    void timerCallback() override
    {
        repaint();
        indice += 1;
        if (indice >= 59)
        {
            indice =0;
        }
    }
    juce::Array<juce::Image> images;
    int indice = 0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Animation)
};

// This class is creating and configuring your custom component
class AnimationItem : public foleys::GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (AnimationItem)

    AnimationItem (foleys::MagicGUIBuilder& builder, const juce::ValueTree& node) : foleys::GuiItem (builder, node)
    {
        // Create the colour names to have them configurable
        setColourTranslation ({
            {"animation-background", Animation::backgroundColourId},
            {"animation-draw", Animation::drawColourId},
            {"animation-fill", Animation::fillColourId} });

        addAndMakeVisible (animation);
    }

    

    // Override update() to set the values to your custom component
    void update() override
    {
    }

    juce::Component* getWrappedComponent() override
    {
        return &animation;
    }

private:
    Animation animation;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnimationItem)
};
