/*==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "resources/HorizontalLineSource.h"
#include "resources/CompressionValue.h"
#include "components/FanComponent.h"
#include "service/PanComputing.h"
// #include "faustDSP/FaustEffect.h"

const static juce::StringArray notesValues = {"1/1","1/2", "1/4", "1/8", "1/16", "1/32"};
const static juce::StringArray PINGPONG_STYLE = {"Linear", "Sinus", "MadSin"};
//==============================================================================

class DelayAudioProcessor : public foleys::MagicProcessor, private juce::AsyncUpdater, public juce::AudioProcessorValueTreeState::Listener
/*      #if JucePlugin_Enable_ARA
       , public juce::AudioProcessorARAExtension
      #endif*/
{
public:
  //==============================================================================
  DelayAudioProcessor();
  void addAudioListener();
  ~DelayAudioProcessor() override;

  //==============================================================================
  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

  void computePanFromParameters();

#ifndef JucePlugin_PreferredChannelConfigurations
  bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
#endif
  // }
  void initialiseBuilder(foleys::MagicGUIBuilder &builder) override;
  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

  void updateGUI(std::string parameterName);

  //==============================================================================
  const juce::String getName() const override;
  void connectNodes();
  void debugNodes();

  void handleAsyncUpdate() override;

  void changeSliderParameter(const juce::String &sliderID, const juce::String &newParameterID);

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  //==============================================================================
  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int index, const juce::String &newName) override;
  void updateDelayParameters();
  //==============================================================================
  void getStateInformation(juce::MemoryBlock &destData) override;
  void setStateInformation(const void *data, int sizeInBytes) override;
  //==============================================================================

  juce::AudioProcessorValueTreeState parameters;
  float getInputVolume() const { return inputVolume; }

  void parameterChanged(const juce::String &parameterID, float newValue) override;

private:

  juce::AudioProcessorGraph audioGraph;
  PanComputing panComputing;
  PanComputing::ParametersPan _mParametersPan;


  juce::AudioProcessorGraph::Node::Ptr delayNode;
  juce::AudioProcessorGraph::Node::Ptr gainNode;
  juce::AudioProcessorGraph::Node::Ptr mixerNode;

  int switchPingPong = 0;
  int switchDelay = 0;
  float inputVolume = 0.0f;
  float pan = 0.0f;
  int manualPan = 0;
  bool manualPanBool = false;
  float bpm = 100.0f;

  juce::AudioPlayHead* playHead = nullptr;

  foleys::MagicPlotSource *analyser = nullptr;
  foleys::MagicPlotSource *analyserOutput = nullptr;
  
  juce::String pendingSliderID;
  juce::String pendingParameterName;

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayAudioProcessor)
};
