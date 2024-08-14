/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/dragAndDrop.h"
#include "GUI/dragToDawButton.h"

//==============================================================================
/**
*/
class ChopChopAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                      public juce::DragAndDropContainer
{
public:
    ChopChopAudioProcessorEditor (ChopChopAudioProcessor&);
    ~ChopChopAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void mouseDrag(const juce::MouseEvent&) override;

private:
    ChopChopAudioProcessor& audioProcessor;

    dragAndDropComp dnd;

    juce::TextButton chopChop, history; //change history to combo box, load like preset manager
    dragToDawButton dragToDaw;
    juce::Slider chops;

    juce::AudioProcessorValueTreeState::SliderAttachment chopsAT;

    juce::StringArray currentFile;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChopChopAudioProcessorEditor)
};
