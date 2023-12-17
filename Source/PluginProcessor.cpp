/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/
#include "PluginProcessor.h"
#include "resources/LookAndFeelFirst.h"
#include "resources/LookAndFeelThreshold.h"
#include "resources/CompressionValue.h"
#include "audioGraph/nodes/delayPingPongProcessor.h"
#include "audioGraph/nodes/gainProcessor.h"
#include "audioGraph/nodes/DryWetMixer.h"
#include "Components/LissajourComponent.h"
//==============================================================================

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    // Add a ComboBox parameter for the slider
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"noteslength", 1}, "note Time", notesValues, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"delaytime", 1}, "Delay Time", 1.0f, 2000.0f, 500.0f));
    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{"switchDelay", 1}, "switch delay", 0, 1, 0));

    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"pingpongnotes", 1}, "Ping Pong Speed", notesValues, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"pingpongtime", 1}, "Ping Pong Time", 0.1f, 2000.0f, 500.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"pingpongstyle", 1}, "Ping Pong Style", PINGPONG_STYLE, 0));
    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{"switchpingpong", 1}, "switch ping pong", 0, 1, 0));

    // Add other parameters...
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"gain", 1}, "Gain", 0.0f, 1.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"feedback", 1}, "FeedBack", 0.0f, 0.99f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"mix", 1}, "Mix", 0.0f, 1.0f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"width", 1}, "Width", 0, 1.0f, 0.69f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"pan", 1}, "Pan", -1.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{"manualPan", 1}, "Manual Panning", 0, 1, 1));
    return layout;
}

DelayAudioProcessor::DelayAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : foleys::MagicProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                                 .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                                 .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
                                 )
#endif
      ,
      parameters(*this, nullptr, juce::Identifier("Delay"), createParameterLayout())
{
    FOLEYS_SET_SOURCE_PATH(__FILE__);
    FOLEYS_ENABLE_BINARY_DATA;

    auto file = juce::File::getSpecialLocation(juce::File::currentApplicationFile)
                    .getChildFile("Contents")
                    .getChildFile("resources")
                    .getChildFile("magictest.xml");

    if (file.existsAsFile())
        magicState.setGuiValueTree(file);
    else
        magicState.setGuiValueTree(BinaryData::magictest_xml, BinaryData::magictest_xmlSize);
    analyser = magicState.createAndAddObject<foleys::MagicAnalyser>("input");
    analyserOutput = magicState.createAndAddObject<foleys::MagicAnalyser>("output");
    magicState.setPlayheadUpdateFrequency(30);
    audioGraph.clear();
      audioGraph.setPlayConfigDetails(getTotalNumInputChannels(),
                                getTotalNumOutputChannels(),
                                   getSampleRate(),
                                   getBlockSize());
     DelayAudioProcessor::connectNodes();
     addAudioListener();
   // DelayAudioProcessor::debugNodes();
}
void  DelayAudioProcessor::addAudioListener(){
        parameters.addParameterListener("width", this);
        parameters.addParameterListener("gain", this);
        parameters.addParameterListener("feedback", this);
        parameters.addParameterListener("noteslength", this);
        parameters.addParameterListener("delaytime", this);
        parameters.addParameterListener("switchDelay", this);
        parameters.addParameterListener("switchpingpong", this);
        parameters.addParameterListener("pingpongtime", this);
        parameters.addParameterListener("pingpongnotes", this);
        parameters.addParameterListener("pingpongstyle", this);
        parameters.addParameterListener("mix", this);
        parameters.addParameterListener("pan", this);
        parameters.addParameterListener("manualPan", this);

}
void DelayAudioProcessor::debugNodes(){
    std::unique_ptr<juce::AudioProcessor> mixerProcessor = std::make_unique<DryWetMixer>();

    mixerNode = audioGraph.addNode(std::move(mixerProcessor));
    std::unique_ptr<juce::AudioProcessor> gainProcessor = std::make_unique<GainProcessor>();

    // Add the processor to the graph and get the node
    gainNode = audioGraph.addNode(std::move(gainProcessor));
    std::unique_ptr<juce::AudioProcessor> delayProcessor = std::make_unique<DelayPingPongProcessor>();

    // Add the processor to the graph and get the node
    delayNode = audioGraph.addNode(std::move(delayProcessor));

    // Create an AudioProcessor for the input
    std::unique_ptr<juce::AudioProcessor> inputProcessor = std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
        juce::AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode);

    // Add the processor to the graph and get the node
    auto inputNode = audioGraph.addNode(std::move(inputProcessor));

    // Create an AudioProcessor for the output
    std::unique_ptr<juce::AudioProcessor> outputProcessor = std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
        juce::AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode);

    // Add the processor to the graph and get the node
    auto outputNode = audioGraph.addNode(std::move(outputProcessor));
        // Connect the input to the first node
    if (inputNode != nullptr && outputNode != nullptr)
    {
        for (int channel = 0; channel < getTotalNumInputChannels(); ++channel)
        {
            juce::AudioProcessorGraph::Connection connection;
            connection.source = {inputNode->nodeID, channel};
            connection.destination = {outputNode->nodeID, channel};
            audioGraph.addConnection(connection);
        }
    }
}

void DelayAudioProcessor::connectNodes()
{

    std::unique_ptr<juce::AudioProcessor> mixerProcessor = std::make_unique<DryWetMixer>();

    mixerNode = audioGraph.addNode(std::move(mixerProcessor));
    std::unique_ptr<juce::AudioProcessor> gainProcessor = std::make_unique<GainProcessor>();

    // Add the processor to the graph and get the node
    gainNode = audioGraph.addNode(std::move(gainProcessor));
    std::unique_ptr<juce::AudioProcessor> delayProcessor = std::make_unique<DelayPingPongProcessor>();

    // Add the processor to the graph and get the node
    delayNode = audioGraph.addNode(std::move(delayProcessor));

    // Create an AudioProcessor for the input
    std::unique_ptr<juce::AudioProcessor> inputProcessor = std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
        juce::AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode);

    // Add the processor to the graph and get the node
    auto inputNode = audioGraph.addNode(std::move(inputProcessor));

    // Create an AudioProcessor for the output
    std::unique_ptr<juce::AudioProcessor> outputProcessor = std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
        juce::AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode);

    // Add the processor to the graph and get the node
    auto outputNode = audioGraph.addNode(std::move(outputProcessor));

    
    // Connect the input to the first node
    if (inputNode != nullptr && gainNode != nullptr)
    {
        for (int channel = 0; channel < getTotalNumInputChannels(); ++channel)
        {
            juce::AudioProcessorGraph::Connection connection;
            connection.source = {inputNode->nodeID, channel};
            connection.destination = {gainNode->nodeID, channel};
            audioGraph.addConnection(connection);
        }
    }
    // Connect the last node to the output
    if (delayNode != nullptr && gainNode != nullptr)
    {
        for (int channel = 0; channel < getTotalNumInputChannels(); ++channel)
        {
            juce::AudioProcessorGraph::Connection connection;
            connection.source = {gainNode->nodeID, channel};
            connection.destination = {delayNode->nodeID, channel};

            audioGraph.addConnection(connection);
        }
    }

    // // Connect the last node to the output
    // if (delayNode != nullptr && outputNode != nullptr)
    // {
    //     for (int channel = 0; channel < getTotalNumOutputChannels(); ++channel)
    //     {
    //         juce::AudioProcessorGraph::Connection connection;
    //         connection.source = {delayNode->nodeID, channel};
    //         connection.destination = {outputNode->nodeID, channel};

    //         audioGraph.addConnection(connection);
    //     }
    // }

    /*
MAKE A MIXER AROUND ANOTHER NODE
*/
    // Connect the input to the mixer (dry signal)
    if (inputNode != nullptr && mixerNode != nullptr)
    {
        for (int channel = 0; channel < 2; ++channel)
        {
            juce::AudioProcessorGraph::Connection connection;
            connection.source = {inputNode->nodeID, channel};
            connection.destination = {mixerNode->nodeID, channel};

            audioGraph.addConnection(connection);
            juce::AudioProcessorGraph::Connection connectionOut;
            connectionOut.source = {delayNode->nodeID, channel};
            connectionOut.destination = {mixerNode->nodeID, 2 + channel};
            audioGraph.addConnection(connectionOut);

            juce::AudioProcessorGraph::Connection connectionOutMixer;
            connectionOutMixer.source = {mixerNode->nodeID, channel};
            connectionOutMixer.destination = {outputNode->nodeID, channel};
            audioGraph.addConnection(connectionOutMixer);
        }
    }
}

void printValueTree(const juce::ValueTree &tree, const juce::String &indent = "")
{
    std::cout << indent << "ValueTree: " << tree.getType().toString() << std::endl;

    for (int i = 0; i < tree.getNumProperties(); ++i)
    {
        const auto &property = tree.getPropertyName(i);
        std::cout << indent << "  Property: " << property.toString() << " = " << tree[property].toString() << std::endl;
    }

    for (int i = 0; i < tree.getNumChildren(); ++i)
    {
        printValueTree(tree.getChild(i), indent + "  ");
    }
}

// SUPER IMPORTANT
juce::ValueTree findChildWithProperty(const juce::ValueTree &tree, const juce::Identifier &property, const juce::var &value)
{
    if (tree.hasProperty(property) && tree[property] == value)
        return tree;

    for (int i = 0; i < tree.getNumChildren(); ++i)
    {
        juce::ValueTree child = tree.getChild(i);
        juce::ValueTree result = findChildWithProperty(child, property, value);
        if (result.isValid())
            return result;
    }

    return juce::ValueTree();
}
  void DelayAudioProcessor::changeSliderParameter(const juce::String &sliderID, const juce::String &newParameterName)
    {
        // Store the new parameter name and slider ID
        pendingSliderID = sliderID;
        pendingParameterName = newParameterName;

        std::cout << "changeSliderParameter: " << sliderID << " -> " << newParameterName << std::endl;
        std::cout << "  pendingSliderID: " << pendingSliderID << std::endl;
        // Trigger an asynchronous update
        triggerAsyncUpdate();
    }


 void DelayAudioProcessor::handleAsyncUpdate() 
    {
        // Change the slider parameter on the message thread
        auto slider = findChildWithProperty(magicState.getGuiTree(), "id", pendingSliderID);
        if (slider.isValid())
        {
            std::cout << "  slider found: " << slider.getType().toString() << std::endl;
            juce::var varParameterName = juce::var(pendingParameterName);
            slider.setProperty("parameter", varParameterName, nullptr);
        }
        else {
            std::cout << "  slider not found: " << pendingSliderID << std::endl;
        }
    }



DelayAudioProcessor::~DelayAudioProcessor()
{
}
void DelayAudioProcessor::initialiseBuilder(foleys::MagicGUIBuilder &builder)
{
    builder.registerJUCEFactories();
    builder.registerJUCELookAndFeels();
    builder.registerFactory("Lissajour", &LissajourItem::factory);
    builder.registerFactory("FanItem", &FanItem::factory);

    builder.registerLookAndFeel("slide", std::make_unique<LookAndFeelFirst>());
    builder.registerLookAndFeel("threshold", std::make_unique<LookAndFeelThreshold>());
}

//==============================================================================
const juce::String DelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DelayAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool DelayAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool DelayAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else

    return false;
#endif
}

double DelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DelayAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
              // so this should be at least 1, even if you're not really implementing programs.
}

int DelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DelayAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String DelayAudioProcessor::getProgramName(int index)
{
    return {};
}

void DelayAudioProcessor::changeProgramName(int index, const juce::String &newName)
{
}

float DelayAudioProcessor::computePan(float bpm, float ppqPosition, float ppqMesure, float timeSecond, std::string panType, int timeSigDenominator, int timeSigNumerator)
{

    // TODO ADD PARAMETER FOR PAN TYPE
    // TODO ADD PARAMETER FOR PAN TIME

    double beatTime = 60.0f / bpm;
    double timeMesure = ppqMesure * beatTime;
    double pan = 0.0f;

    if (panType == "linear")
    {
        double delta = (timeSecond - timeMesure) / beatTime;
        delta = (ppqPosition - ppqMesure) / timeSigNumerator;
        // Delta should be between 0 and timeSigNumerator
        // delta = delta/timeSigNumerator;
        if (delta > 0.5f)
        {
            delta = 1.0f - delta;
        }

        jassert(delta <= 1.0f && delta >= 0.0f);

        pan = delta * 4.0f - 1.0f;

        jassert(pan <= 1.0f && pan >= -1.0f);
    }
    else if (panType == "sin")
    {
        double delta = std::fmod(((*parameters.getRawParameterValue("delaytime") / 1000.0f)) * 0.5f + timeSecond, (*parameters.getRawParameterValue("delaytime") / 1000.0f)) / (*parameters.getRawParameterValue("delaytime") / 1000.0f);
    }
    else if (panType == "sinRandom")
    {
        // VERSION SIN RANDOM
        pan = std::sin(timeSecond * 2.0f * juce::MathConstants<float>::pi);
    }
    return pan;
}

           void DelayAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue) 
    {
        // This method is called when a parameter changes

        
            updateGUI();
            updateDelayParameters();
        

    }

//==============================================================================
void DelayAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());
    analyser->prepareToPlay(sampleRate, samplesPerBlock);
    analyserOutput->prepareToPlay(sampleRate, samplesPerBlock);
    audioGraph.prepareToPlay(sampleRate, samplesPerBlock);
}

void DelayAudioProcessor::releaseResources()
{
    audioGraph.releaseResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DelayAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif
void DelayAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    analyser->pushSamples(buffer);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

 audioGraph.processBlock(buffer, midiMessages);

    // // Make sure we are processing stereo audio
    if (totalNumInputChannels == 2 && totalNumOutputChannels == 2)
    {
        // Get pointers to the input and output channel data
        const float *inputChannelData[2];
        float *outputChannelData[2];

        inputChannelData[0] = buffer.getReadPointer(0);
        inputChannelData[1] = buffer.getReadPointer(1);
        outputChannelData[0] = buffer.getWritePointer(0);
        outputChannelData[1] = buffer.getWritePointer(1);
    }

    pan = 0.0f;
    if (getPlayHead() != nullptr)
    {
        bpm = getPlayHead()->getPosition()->getBpm().orFallback(0.0);
        float ppqPosition = getPlayHead()->getPosition()->getPpqPosition().orFallback(0.0);
        float ppqMesure = getPlayHead()->getPosition()->getPpqPositionOfLastBarStart().orFallback(0.0);
        double timeSecond = getPlayHead()->getPosition()->getTimeInSeconds().orFallback(0.0);

        /**
         * Error with time signature optional
         */
        int timeSigDenominator = 4;
        int timeSigNumerator = 4;
        juce::Optional<juce::AudioPlayHead::TimeSignature> timsigFromHost = getPlayHead()->getPosition()->getTimeSignature();
        if (timsigFromHost)
        {
            timeSigNumerator = timsigFromHost->numerator;
            timeSigDenominator = timsigFromHost->denominator;
        }

        std::string panType = "linear";

        // Retourne valeur entre -1 et 1, faire gaffe modelisation trajet

        pan = computePan(bpm, ppqPosition, ppqMesure, timeSecond, panType, timeSigDenominator, timeSigNumerator);
    }
    // updateGUI();
    // updateDelayParameters();
    analyserOutput->pushSamples(buffer);
}
void DelayAudioProcessor::updateGUI()
{
    
    switchDelay = *parameters.getRawParameterValue("switchDelay");

    if (switchDelay == 1)
    {
        std::cout<<"switch1 " <<std::endl;
        changeSliderParameter("delaySlider", "delaytime");
    }
    else
    {
        changeSliderParameter("delaySlider", "noteslength");
    }

    switchPingPong = *parameters.getRawParameterValue("switchpingpong");
    if (switchPingPong == 1)
    {
        changeSliderParameter("pingPongSlider", "pingpongtime");
    }
    else
    {
        changeSliderParameter("pingPongSlider", "pingpongnotes");
    }
}
void DelayAudioProcessor::updateDelayParameters()
{
    float feedback = *parameters.getRawParameterValue("feedback");
    float gain = *parameters.getRawParameterValue("gain");
    float width = *parameters.getRawParameterValue("width");
    auto rootGui = magicState.getGuiTree();

   
    std::string notesLength = "1/4";
    auto param = dynamic_cast<juce::AudioParameterChoice *>(parameters.getParameter("noteslength"));
    if (param != nullptr)
    {
        int selectedIndex = param->getIndex();
        notesLength = param->choices[selectedIndex].toStdString();
    }

    auto gainProcessor = dynamic_cast<GainProcessor *>(gainNode->getProcessor());
    if (gainProcessor != nullptr)
    {
        gainProcessor->setGain(gain);
    }

    auto delayProcessor = dynamic_cast<DelayPingPongProcessor *>(delayNode->getProcessor());
    if (delayProcessor != nullptr)
    {
        if (switchDelay == 1)
        {
            float delay = *parameters.getRawParameterValue("delaytime");
            delayProcessor->setDelay(delay);
        }
        else
        {
            delayProcessor->setNotesLength(notesLength, bpm);
        }

        delayProcessor->setFeedBack(feedback);
        delayProcessor->setWidth(width);
    }

    int manualPan = *parameters.getRawParameterValue("manualPan");
    if (manualPan == 1)
    {
        pan = *parameters.getRawParameterValue("pan");
        delayProcessor->setPan(pan);
    }
    auto mixerProcessor = dynamic_cast<DryWetMixer *>(mixerNode->getProcessor());
    if (mixerProcessor != nullptr)
    {
        mixerProcessor->setMix(*parameters.getRawParameterValue("mix"));
    }
}

//==============================================================================
void DelayAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    // Create an XML element that represents the plugin's current state
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());

    // Use this helper function to stuff it into the binary blob and return it
    copyXmlToBinary(*xml, destData);
}

void DelayAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    // // This getXmlFromBinary helper function retrieves our XML from the binary blob
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        // Make sure that it's actually our type of XML object
        if (xmlState->hasTagName(parameters.state.getType()))
        {
            // Now reload our plugin's state from the XML object
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new DelayAudioProcessor();
}
