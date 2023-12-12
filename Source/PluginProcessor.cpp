/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/
#include "PluginProcessor.h"
#include "resources/LookAndFeelFirst.h"
#include "resources/LookAndFeelThreshold.h"
#include "resources/CompressionValue.h"
#include "faustDSP/FaustEffect.h"
#include "audioGraph/nodes/delayProcessor.h"
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

    // Add other parameters...
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"gain", 1}, "Gain", 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"feedback", 1}, "FeedBack", 0.0f, 1.0f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"delaytime", 1}, "Delay Time", 1.0f, 2000.0f, 500.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"mix", 1}, "Mix", 0.0f, 1.0f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"width", 1}, "Width", 0, 1.0f, 0.0f));

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

    // Connect the output of the first node to the input of the second
    DelayAudioProcessor::connectNodes();
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

DelayAudioProcessor::~DelayAudioProcessor()
{
}
void DelayAudioProcessor::initialiseBuilder(foleys::MagicGUIBuilder &builder)
{
    builder.registerJUCEFactories();
    builder.registerJUCELookAndFeels();

    // std::unique_ptr<juce::LookAndFeel> lookAndFeel = std::make_unique<juce::LookAndFeel>(LookAndFeel());
    // Step 4: Create your fan component
    // auto fanComponent = std::make_unique<FanComponent>();

    //         // Step 5: Register your fan component
    //   builder.registerFactory("fan", [](foleys::MagicGUIBuilder& /*builder*/, const juce::ValueTree& /*tree*/)
    //     {
    //         return std::make_unique<FanComponent>();
    //     });
    builder.registerFactory("Lissajour", &LissajourItem::factory);

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

//==============================================================================
void DelayAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;

    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());
    // fDSP.init(sampleRate); // replace sampleRate with your actual sample rate
    analyser->prepareToPlay(sampleRate, samplesPerBlock);
    analyserOutput->prepareToPlay(sampleRate, samplesPerBlock);
    // delayProcessor_.prepareToPlay(sampleRate, samplesPerBlock);
    audioGraph.prepareToPlay(sampleRate, samplesPerBlock);
}

void DelayAudioProcessor::releaseResources()
{
    audioGraph.releaseResources();

    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DelayAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
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

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    audioGraph.processBlock(buffer, midiMessages);

    // Make sure we are processing stereo audio
    if (totalNumInputChannels == 2 && totalNumOutputChannels == 2)
    {
        // Get pointers to the input and output channel data
        const float *inputChannelData[2];
        float *outputChannelData[2];

        inputChannelData[0] = buffer.getReadPointer(0);
        inputChannelData[1] = buffer.getReadPointer(1);
        outputChannelData[0] = buffer.getWritePointer(0);
        outputChannelData[1] = buffer.getWritePointer(1);

        // Process the audio data with the Faust DSP object
        //     fDSP.compute(buffer.getNumSamples(), const_cast<float**>(inputChannelData), outputChannelData);
    }
    double bpm = 100.0f;

    pan = 0.0f;
    if (getPlayHead() != nullptr)
    {
        bpm = getPlayHead()->getPosition()->getBpm().orFallback(0.0);

        double beatTime = 60.0f / getPlayHead()->getPosition()->getBpm().orFallback(0.0);

        double value = getPlayHead()->getPosition()->getTimeInSeconds().orFallback(0.0);

        double mesureValue = getPlayHead()->getPosition()->getPpqPositionOfLastBarStart().orFallback(0.0);
        
        double timeMesure = mesureValue * beatTime;
        // Retourne valeur entre 0 et 1, faire gaffe modelisation trajet
        double delta = (value - timeMesure) / beatTime;

        // VERSION SIN RANDOM

        // pan = std::sin( value);
        // double delta = std::fmod( ((*parameters.getRawParameterValue("delaytime") / 1000.0f))*0.5f+value, (*parameters.getRawParameterValue("delaytime") / 1000.0f))/ (*parameters.getRawParameterValue("delaytime") /1000.0f);               //double position = getPlayHead()->getPosition()->getPpqPosition().orFallback(0.0);

        // double lastBeatPosition = getPlayHead()->getPosition()->getPpqPositionOfLastBarStart().orFallback(0.0);
        //   double lastBeatTime = beatTime * lastBeatPosition;
        //         double delta = (value - lastBeatTime)/beatTime;

        pan = delta * 0.5f - 1.0f;
    }
    updateDelayParameters(bpm);
    analyserOutput->pushSamples(buffer);
}

void DelayAudioProcessor::updateDelayParameters(float bpm)
{
    float delay = *parameters.getRawParameterValue("delaytime");
    float feedback = *parameters.getRawParameterValue("feedback");
    float gain = *parameters.getRawParameterValue("gain");
    float width = *parameters.getRawParameterValue("width");
    string notesLength = "1/4";
    auto param = dynamic_cast<juce::AudioParameterChoice *>(parameters.getParameter("noteslength"));
    if (param != nullptr)
    {
        int selectedIndex = param->getIndex();
        // Do something with selectedIndex...
        notesLength = param->choices[selectedIndex].toStdString();
    }

    //  float pan = *parameters.getRawParameterValue("pan");

    auto gainProcessor = dynamic_cast<GainProcessor *>(gainNode->getProcessor());
    if (gainProcessor != nullptr)
    {
        gainProcessor->setGain(gain);
    }
    auto delayProcessor = dynamic_cast<DelayPingPongProcessor *>(delayNode->getProcessor());
    if (delayProcessor != nullptr)
    {
        //  delayProcessor->setDelay(delay);
        delayProcessor->setFeedBack(feedback);
        delayProcessor->setPan(pan);
        delayProcessor->setWidth(width);
        delayProcessor->setNotesLength(notesLength, bpm);
    }
    auto mixerProcessor = dynamic_cast<DryWetMixer *>(mixerNode->getProcessor());
    if (mixerProcessor != nullptr)
    {
        mixerProcessor->setMix(*parameters.getRawParameterValue("mix"));
        // fDSP.setParamValue("feedback", feedback);
        //    delayProcessor_.setDelayP/arameters();
    }
}
// SUPPR HASEDITORS

//

// juce::AudioProcessorEditor* DelayAudioProcessor::createEditor()
// {
//     return new foleys::MagicPluginEditor(magicState,BinaryData::magic_xml,BinaryData::magic_xmlSize);
//   return new DelayAudioProcessorEditor (*this);
// }

//==============================================================================
void DelayAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DelayAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new DelayAudioProcessor();
}
