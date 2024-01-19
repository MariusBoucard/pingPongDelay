/*
  ==============================================================================

    RotaryLookAndFeel.h
    Created: 26 Oct 2023 8:56:10pm
    Author:  Joseph Courcelle

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class WoodVintageLookAndFeel  : public juce::LookAndFeel_V4
{
public:
    WoodVintageLookAndFeel()
    {
        // Constructor
    }
    
    
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                         float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override
    {
        // Calculate the angle for the slider position within the full angle range
        float angle = 1.5f * juce::MathConstants<float>::pi + rotaryStartAngle + (rotaryEndAngle - rotaryStartAngle) * sliderPos;

        // Calculate the knob's position
        float centerX = x + width / 2;
        float centerY = y + height / 2;
        

        
        g.setOpacity(1.0f);
        
        /*
        // Draw wood box
        const char* imageBox = BinaryData::boxWood_jpg;
        int imageBoxSize = BinaryData::boxWood_jpgSize;
        juce::Image boxImage = juce::ImageCache::getFromMemory(imageBox, imageBoxSize);
        
        
        g.drawImage(boxImage, x, y, width, height,0,0, boxImage.getWidth(), boxImage.getHeight());
        */
        
        // create fire ball
        const char* imageFire = BinaryData::bouledefeu_png;
        int imageFireSize = BinaryData::bouledefeu_pngSize;
        juce::Image fireImage = juce::ImageCache::getFromMemory(imageFire, imageFireSize);
        
        
        
        // create rotary wood inside box
        const char* imageData = BinaryData::gouvernail_png;
        int imageSize = BinaryData::gouvernail_pngSize;
        juce::Image myKnobImage = juce::ImageCache::getFromMemory(imageData, imageSize);
        
                
        
        
        juce::AffineTransform rotation;
        rotation=rotation.rotated(2*angle*360/(180*juce::MathConstants<float>::pi), myKnobImage.getWidth()/2, myKnobImage.getHeight()/2);
        
        // Create an intermediate image with the same size as the original image
        juce::Image rotatedImage(juce::Image::ARGB, myKnobImage.getWidth(), myKnobImage.getHeight(), true);

        // Create a Graphics context for the rotated image
        juce::Graphics g2(rotatedImage);

        // Apply a rotation transform to the Graphics context
        g2.addTransform(rotation);

        // Draw the original image onto the rotated image with the applied transform
        g2.drawImageAt(myKnobImage, 0, 0);
        
        
        
        g.setOpacity(sliderPos);
        // draw fire ball
        g.drawImage(fireImage, centerX - width/6, centerY - width/6, width/3, width/3,0,0, fireImage.getWidth(), fireImage.getHeight());
        
        
        g.setOpacity(1.0f);
        // draw rotary
        g.drawImage(rotatedImage, centerX - width/4, centerY - width/4, width/2, width/2, 0, 0, myKnobImage.getWidth(), myKnobImage.getHeight());
        
        //g.drawImageWithin(<#const Image &imageToDraw#>, <#int destX#>, <#int destY#>, <#int destWidth#>, <#int destHeight#>, <#RectanglePlacement placementWithinTarget#>)
        
            
               
               
        
    }
    
    void drawLabel (juce::Graphics& g, juce::Label& label) override
    {
        g.fillAll (label.findColour (juce::Label::backgroundColourId));

        if (! label.isBeingEdited())
        {
            auto alpha = label.isEnabled() ? 1.0f : 0.5f;
            const juce::Font font (getLabelFont (label));

            g.setColour (label.findColour (juce::Label::textColourId).withMultipliedAlpha (alpha));
            g.setFont (font);

            auto textArea = getLabelBorderSize (label).subtractedFrom (label.getLocalBounds());

            g.drawFittedText (label.getText(), textArea, label.getJustificationType(),
                              juce::jmax (1, (int) ((float) textArea.getHeight() / font.getHeight())),
                              label.getMinimumHorizontalScale());

            g.setColour (label.findColour (juce::Label::outlineColourId).withMultipliedAlpha (alpha));
        }
        else if (label.isEnabled())
        {
            g.setColour (label.findColour (juce::Label::outlineColourId));
        }

        g.drawRect (label.getLocalBounds());
    }
    
    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override {
        
        
        if (button.getToggleState()){
            const char* image = BinaryData::switchON_jpeg;
            int imageSize = BinaryData::switchON_jpegSize;
            juce::Image myImage = juce::ImageCache::getFromMemory(image, imageSize);
            
            g.drawImage(myImage, button.getX(), button.getY(), button.getWidth(), button.getHeight(),0,0, myImage.getWidth(), myImage.getHeight());
        }
        else{
            const char* image = BinaryData::switchOFF_jpeg;
            int imageSize = BinaryData::switchOFF_jpegSize;
            juce::Image myImage = juce::ImageCache::getFromMemory(image, imageSize);
            g.drawImage(myImage, button.getX(), button.getY(), button.getWidth(), button.getHeight(),0,0, myImage.getWidth(), myImage.getHeight());
        }
        
    }
    
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                  float sliderPos, float minSliderPos, float maxSliderPos,
                              const juce::Slider::SliderStyle style, juce::Slider& slider)
        {
            // Load the images
            const char* image = BinaryData::line_png;
            int imageSize = BinaryData::line_pngSize;
            juce::Image lineImage = juce::ImageCache::getFromMemory(image, imageSize);
            
            const char* image2 = BinaryData::sliderbutton_png;
            int image2Size = BinaryData::sliderbutton_pngSize;
            juce::Image buttonImage = juce::ImageCache::getFromMemory(image2, image2Size);

            // Calculate the position of the button based on the slider value
            
            float buttonX = x + sliderPos;
            float buttonY = y;
            //float buttonX = x + sliderPos * (width - buttonImage.getWidth());
            //float buttonY = y + (height - buttonImage.getHeight()) / 2;

            // Draw the line
            g.drawImage(lineImage, x, y + height / 2 - lineImage.getHeight() / 8, width, lineImage.getHeight()/4, 0, 0, lineImage.getWidth(), lineImage.getHeight());

            // Draw the button
            g.drawImage(buttonImage, buttonX, buttonY,
                        buttonImage.getWidth()/4, buttonImage.getHeight()/4,
                        0, 0, buttonImage.getWidth(), buttonImage.getHeight());
        
            
        }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WoodVintageLookAndFeel)
};
