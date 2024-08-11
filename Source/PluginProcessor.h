/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class ChopChopAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    ChopChopAudioProcessor();
    ~ChopChopAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

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

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "parameters", createParameterLayout() };

    void loadFile(const juce::String&);
    juce::AudioBuffer<float>& getWaveform() { return waveform; }
    void chopFile();
    juce::File getNewFileLocation();

private:

    juce::AudioParameterInt* chops{ nullptr };

    juce::AudioParameterBool* history{ nullptr };
    juce::AudioParameterBool* chopChop{ nullptr };
    juce::AudioParameterBool* playback{ nullptr };

    juce::Synthesiser sampler;
    juce::AudioFormatManager manager;
    juce::AudioFormatReader* reader{ nullptr };
    juce::AudioBuffer<float> waveform;
    juce::AudioBuffer<float> holder;
    std::vector<juce::AudioBuffer<float>> audioBuffers;



    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChopChopAudioProcessor)
};
