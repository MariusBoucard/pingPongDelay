#include <juce_gui_basics/juce_gui_basics.h>

class LookAndFeelThreshold  : public juce::LookAndFeel_V4
{
public:
  LookAndFeelThreshold()
  {
 
    
  }

  
  void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
             float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override
  {
      auto radius = width/2.0f-20.0f;
    auto centreX = x + width  * 0.5f;
    auto centreY = y + height * 0.5f;

       auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = 3.0f;
    auto arcRadius  = radius + lineW * 4.0f;

    juce::Path backgroundArc;
    backgroundArc.addCentredArc (centreX,
                                 centreY,
                                 arcRadius,
                                 arcRadius,
                                 0.0f,
                                 rotaryStartAngle,
                                 rotaryEndAngle,
                                 true);

    g.setColour (juce::Colours::grey);
    g.strokePath (backgroundArc, juce::PathStrokeType (lineW, juce::PathStrokeType::curved, juce::PathStrokeType::butt));

juce::Path volumePath;
volumePath.addCentredArc(
  centreX,
  centreY,arcRadius,
  arcRadius,
  0.0f,
  rotaryStartAngle,
  endAngle,
  true);
          g.setColour(juce::Colours::red);
        g.strokePath(volumePath, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::butt));



 juce::Path overlay;
    overlay.addCentredArc (centreX,
                                 centreY,
                                 arcRadius,
                                 arcRadius,
                                 0.0f,
                                 rotaryStartAngle,
                                ( rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle)),
                                 true);

    g.setColour (juce::Colours::white);
    g.strokePath (overlay, juce::PathStrokeType (lineW, juce::PathStrokeType::curved, juce::PathStrokeType::butt));

    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    auto outlineThickness = radius * 0.3f;
    // // fill
    juce::ColourGradient grad(juce::Colours::lightcyan, 0.0f, 0.0f, juce::Colours::darkcyan, radius * 2.0f, radius * 2.0f, true);
    g.setGradientFill(grad);
    g.fillEllipse(rx, ry, radius * 2.0f, radius * 2.0f);
    // outline
    juce::ColourGradient gradient(juce::Colours::orchid, 0.0f, 0.0f, juce::Colours::darkorchid, radius * 2.0f, radius * 2.0f, true);
    g.setGradientFill(gradient);
    g.drawEllipse(rx, ry, radius * 2.0f, radius * 2.0f, outlineThickness);

    // Draw lines on the ellipse, splitting it into 6 parts
    g.setColour(juce::Colours::black);
    auto littleLineThickness = outlineThickness * 0.1f;
    auto nbSplit = 8;
    for (int i = 0; i <= nbSplit; ++i) {
      auto angle =  i * (rotaryEndAngle - rotaryStartAngle) / nbSplit + rotaryStartAngle - juce::MathConstants<float>::halfPi;
      auto x1 = centreX + (radius + outlineThickness/2.0f) * std::cos(angle);
      auto y1 = (centreY + (radius  + outlineThickness/2.0f) * std::sin(angle));
      auto x2 = centreX + (radius - outlineThickness/2.0f ) * std::cos(angle);
      auto y2 = (centreY + (radius - outlineThickness/2.0f) * std::sin(angle));
      g.drawLine(x1, y1, x2, y2, littleLineThickness);
    }
     
    juce::Path p;
    auto halfThickness = outlineThickness * 0.5f;
    p.addTriangle(0.0f ,
             -(radius+0.5f*outlineThickness),
            halfThickness,
            -(radius-halfThickness),
            -halfThickness,
           -(radius-halfThickness));
                     
    p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

    // pointer
    g.setColour(juce::Colours::black);
    g.fillPath(p);
    g.setColour(juce::Colours::white);
    g.setFont(10.0f);
    // Create the string with two decimal places and the unit "ms"
    juce::String sliderPosString = juce::String::formatted("%.2f ms", sliderPos);

    slider.setTextValueSuffix(" ms");

    // Draw the line that fills up the knob based on the slider value

  }
      void setEndAngle(float newEndAngle)
    {
        endAngle = newEndAngle;
    }


private:
float endAngle = 90.0f;
    juce::Image backgroundImage;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LookAndFeelThreshold)
};
