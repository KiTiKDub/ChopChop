/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/dragAndDrop.h"

//==============================================================================
/**
*/
class ChopChopAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ChopChopAudioProcessorEditor (ChopChopAudioProcessor&);
    ~ChopChopAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ChopChopAudioProcessor& audioProcessor;

    dragAndDropComp dnd;

    juce::TextButton chopChop, playback, history;
    juce::Slider chops;

    juce::AudioProcessorValueTreeState::ButtonAttachment chopChopAT, playbackAT, historyAT;
    juce::AudioProcessorValueTreeState::SliderAttachment chopsAT;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChopChopAudioProcessorEditor)
};
