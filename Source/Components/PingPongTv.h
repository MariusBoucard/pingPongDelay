#include   <JuceHeader.h>

//
// Some nice example drawing
class PingPongTv   : public juce::Component
                    // private juce::Timer
{
public:
    enum ColourIDs
    {
        // we are safe from collissions, because we set the colours on every component directly from the stylesheet
        backgroundColourId,
        drawColourId,
        fillColourId
    };

    PingPongTv()
    {
        int ballSize = BinaryData::ball_pngSize;
        const void* ballData = BinaryData::getNamedResource("ball_png", ballSize);
         ball = juce::ImageCache::getFromMemory(ballData, ballSize);

        int leftPlayerSize = BinaryData::leftplayer_pngSize;
        const void* leftPlayerData = BinaryData::getNamedResource("leftplayer_png", leftPlayerSize);
            leftPlayer = juce::ImageCache::getFromMemory(leftPlayerData, leftPlayerSize);

        int rightPlayerSize = BinaryData::rightplayer_pngSize;
        const void* rightPlayerData = BinaryData::getNamedResource("rightplayer_png", rightPlayerSize);
            rightPlayer = juce::ImageCache::getFromMemory(rightPlayerData, rightPlayerSize);

        int pingPongTableSize = BinaryData::pingpongtable_pngSize;
        const void* pingPongData = BinaryData::getNamedResource("pingpongtable_png", pingPongTableSize);
            pingPongTable = juce::ImageCache::getFromMemory(pingPongData, pingPongTableSize);

        int refereeSize = BinaryData::referee_pngSize;
        const void* refereeData = BinaryData::getNamedResource("referee_png", refereeSize);
            referee = juce::ImageCache::getFromMemory(refereeData, refereeSize);


        initRefereePosition();

         // For player movement
        // for (int i = 0; i <= 58; i++) {
        //     //const void* imageData = BinaryData::frame_1_jpg;
        //     //int imageSize = BinaryData::frame_1_jpgSize;
        //     juce::String imageName = "frame_"+juce::String(i)+"_jpg";
        //     const void* imageData = BinaryData::getNamedResource(imageName.toUTF8(), imageSize);
        //     juce::Image image = juce::ImageCache::getFromMemory(imageData, imageSize);
            
        //     images.add(image);
        //     }
        // make sure you define some default colour, otherwise the juce lookup will choke
        setColour (backgroundColourId, juce::Colours::black);
        setColour (drawColourId, juce::Colours::green);
        setColour (fillColourId, juce::Colours::green.withAlpha (0.5f));

        // startTimerHz (30);
    }


    void paint (juce::Graphics& g) override
    {
        bounds = getLocalBounds();
        widthBounds = bounds.getWidth();
        heightBounds = bounds.getHeight();

        setPlayerPosition();

       if(manualPan){
        setRefereePosition();
        refereePosition.y =  getLocalBounds().getY()    + getHeight()/4;
        refereePosition.width = getWidth()/10;
        refereePosition.height = getHeight()/1.3;
        g.drawImage(referee, refereePosition.x, refereePosition.y, refereePosition.width, refereePosition.height, 0, 0, referee.getWidth(), referee.getHeight());
       }
       
       g.drawImage(pingPongTable, getLocalBounds().getX(), getLocalBounds().getY()+20, getWidth(), getHeight(),0,0, pingPongTable.getWidth(),pingPongTable.getHeight());

        // draw referee      

        // leftPlayerPosition.x = getLocalBounds().getX()+getWidth()/8;
        leftPlayerPosition.y =  getLocalBounds().getY()    + getHeight()/4;
        leftPlayerPosition.width = getWidth()/10;
        leftPlayerPosition.height = getHeight()/1.3;
        // draw player 1
        g.drawImage(leftPlayer, leftPlayerPosition.x, leftPlayerPosition.y, leftPlayerPosition.width, leftPlayerPosition.height, 0, 0, leftPlayer.getWidth(), leftPlayer.getHeight());

// rightPlayerPosition.x = getLocalBounds().getX()+getWidth()*7/8 - getWidth()/10;
        rightPlayerPosition.y =  getLocalBounds().getY()    + getHeight()/4;
        rightPlayerPosition.width = getWidth()/10;
        rightPlayerPosition.height = getHeight()/1.3;
        // draw player 2
        g.drawImage(rightPlayer, rightPlayerPosition.x, rightPlayerPosition.y, rightPlayerPosition.width, rightPlayerPosition.height, 0, 0, rightPlayer.getWidth(), rightPlayer.getHeight());
   
        // draw ball
        setBallPosition();
        ballPosition.y =  getLocalBounds().getY()    + getHeight()/3 - getHeight()/20;
        ballPosition.width = getWidth()/10;
        ballPosition.height = getHeight()/10;
        g.drawImage(ball, ballPosition.x, ballPosition.y, ballPosition.width, ballPosition.height, 0, 0, ball.getWidth(), ball.getHeight());
   
   
          // Convert the values to strings
            juce::String panStr = "Pan: " + juce::String(pan);
            juce::String widthStr = "Width: " + juce::String(width);
            juce::String manualPanStr = "Manual Pan: " + (manualPan ? juce::String("True") : juce::String("False"));

            // Calculate the position for the text
            int textHeight = 20;  // Adjust as needed
            int textY = getHeight() - textHeight;

            // Draw the text
            g.setColour(juce::Colours::white);  // Set the text color
            g.drawText(panStr, 0, textY, getWidth(), textHeight, juce::Justification::left);
    g.drawText(widthStr, 0, textY - textHeight, getWidth(), textHeight, juce::Justification::left);
    g.drawText(manualPanStr, 0, textY - 2 * textHeight, getWidth(), textHeight, juce::Justification::left);
   
    }

    void setPlayerPosition(){
        float middleX = getWidth()/2;
        float maxDistMid = middleX*0.9;

        leftPlayerPosition.x = middleX - maxDistMid*width;
        rightPlayerPosition.x = middleX + maxDistMid*width - rightPlayerPosition.width;
    }
    void setRefereePosition(){
        float middleX = getWidth()/2;
        float maxDistMid = middleX*0.9;

        refereePosition.x = middleX + pan*maxDistMid - refereePosition.width/2;
    }
    void setBallPosition(){
        float middleX = getWidth()/2;
        float maxDistMid = middleX*0.9;

        ballPosition.x = middleX + pan*maxDistMid*width - ballPosition.width/2;
    }

    void resized() override
    {
        // This is called when the component is resized.
        // If you add any child components, this is where you should
        // update their positions.
        
        // draw ball
        //g.drawImage(ball, ballPosition.x, ballPosition.y, ballPosition.width, ballPosition.height, 0, 0, ball.getWidth(), ball.getHeight());
    }

    void initRefereePosition(){
        refereePosition.x = getLocalBounds().getX();
        refereePosition.y =  getLocalBounds().getY();
        refereePosition.width = getWidth();
        refereePosition.height = getHeight();
       
    }

/*
Les getters foutent la merde !!
*/
//     // Getter for pan
// float getPan() const
// {
//     return pan;
// }

// // Setter for pan
void setPan(float newPan)
{
    pan = newPan;
}

// // Getter for width
// float getWidth() const
// {
//     return width;
// }

// // Setter for width
void setWidth(float newWidth)
{
    width = newWidth;
}

// // Getter for manualPan
// bool getManualPan() const
// {
//     return manualPan;
// }

// // Setter for manualPan
void setManualPan(bool newManualPan)
{
    manualPan = newManualPan;
}
private:

juce::Rectangle<int> bounds;
int widthBounds;
int heightBounds;

struct imagePosition {
    int x;
    int y;
    int width;
    int height;
};

    imagePosition leftPlayerPosition;
    imagePosition rightPlayerPosition;
    imagePosition refereePosition;
    imagePosition ballPosition;
    



    // void timerCallback() override
    // {
    //     // repaint();

    // }
     juce::Image ball;
     juce::Image leftPlayer;
        juce::Image rightPlayer;
        juce::Image pingPongTable;
        juce::Image referee;

        juce::Array<juce::Image> leftPlayerImages;
        juce::Array<juce::Image> rightPlayerImages;

        float pan = 0.0f;
        float width= 0.0f;
        bool manualPan = false;

        float lastPan;

        // We could implement a game with state every time you stop the playback and points

    int indicePlayerLeft = 0;
    int indicePlayerRight = 0;
   std:: string selectedFunction = "Linear";
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PingPongTv)
};

// This class is creating and configuring your custom component
class PingPongTvItem : public foleys::GuiItem, juce::Timer
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (PingPongTvItem)

    PingPongTvItem (foleys::MagicGUIBuilder& builder, const juce::ValueTree& node) : foleys::GuiItem (builder, node)
    {
        // Create the colour names to have them configurable
        setColourTranslation ({
            {"PingPongTv-background", PingPongTv::backgroundColourId},
            {"PingPongTv-draw", PingPongTv::drawColourId},
            {"PingPongTv-fill", PingPongTv::fillColourId} });
                // startTimerHz(30);

        addAndMakeVisible (PingPongTv);
    }

        std::vector<foleys::SettableProperty> getSettableProperties() const override
    {
        std::vector<foleys::SettableProperty> newProperties;

        newProperties.push_back(
            {configNode, "widthComponent", foleys::SettableProperty::Number, 0.0f, {}}
            );
                  newProperties.push_back(
            {configNode, "pan", foleys::SettableProperty::Number, 0.0f, {}}
            ); 
       newProperties.push_back(
            {configNode, "manualPan", foleys::SettableProperty::Toggle, false, {}}
            );
        newProperties.push_back(
             {configNode, "manualPanBool", foleys::SettableProperty::Toggle, false, {}}
             );
  
        return newProperties;
    }

    // TODO GET PARAMETERS

    // Override update() to set the values to your custom component
    void update() override
    {
        auto width = getProperty("widthComponent");
        auto pan = getProperty("pan");
        auto manualPan = getProperty("manualPanBool");
        PingPongTv.setWidth(width);
        PingPongTv.setPan(pan);
        PingPongTv.setManualPan(manualPan);
        
        // TODO : Set parameters
    }

    juce::Component* getWrappedComponent() override
    {
        return &PingPongTv;
    }
    void timerCallback() override
    {
        repaint();
      //  update();
    }
private:
    // void timerCallback() override
    // {
    //     // repaint();
    //    configNode.setProperty("width",  PingPongTv.getWidth(),nullptr);
    //    configNode.setProperty("pan",  PingPongTv.getPan(),nullptr);
    //    configNode.setProperty("manualPan",  PingPongTv.getManualPan(),nullptr);

    // }
    PingPongTv PingPongTv;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PingPongTvItem)
};
