/*==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "resources/HorizontalLineSource.h"
#include "resources/CompressionValue.h"
#include "faustDSP/FaustEffect.h"
 

 const static      juce::StringArray notesValues = { "1/2", "1/4", "1/8", "1/16", "1/32" };

//==============================================================================

class DelayAudioProcessor  : public foleys::MagicProcessor
                      /*      #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif*/
{
public:
    //==============================================================================
    DelayAudioProcessor() ;
    ~DelayAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif
// }
void initialiseBuilder(foleys::MagicGUIBuilder& builder) override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;


    //==============================================================================
    const juce::String getName() const override;
    void connectNodes ();

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;
void updateDelayParameters(float bpm);
    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    // juce::AudioProcessorEditor* createEditor();
    //==============================================================================
  struct DelayParameters
        {
            float delayTime = 0.5f;
            float feedback = 0.5f;
            float wetLevel = 0.5f;
            float dryLevel = 0.5f;
        } delayParameters;
       
    juce::AudioProcessorValueTreeState parameters;
    float getInputVolume() const { return inputVolume; }


private:
  juce::AudioProcessorGraph audioGraph;

  // std::unique_ptr<juce::AudioProcessor> gainProcessor;
  //   std::unique_ptr<juce::AudioProcessor> delayProcessor;

  juce::AudioProcessorGraph::Node::Ptr delayNode;
  juce::AudioProcessorGraph::Node::Ptr gainNode;
    juce::AudioProcessorGraph::Node::Ptr mixerNode;


        float inputVolume = 0.0f;
   // void initialiseBuilder(foleys::MagicGUIBuilder& builder);
    //==============================================================================
    // Your private member variables go here...
        foleys::MagicPlotSource* analyser = nullptr;
        foleys::MagicPlotSource* analyserOutput = nullptr;
        float pan = 0.0f;
        // mydsp fDSP;
      // foleys::MagicProcessorState magicState { *this };
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayAudioProcessor)
};
