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
#include "GUI/kLookAndFeel.h"
#include "GUI/chopChopBrowserComponent.h"

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

private:
    ChopChopAudioProcessor& audioProcessor;

    Laf lnf;

    dragAndDropComp dnd;

    juce::TextButton chopChop, history, load, back; //change history to combo box, load like preset manager
    dragToDawButton dragToDaw;
    juce::Slider chops;

    juce::AudioProcessorValueTreeState::SliderAttachment chopsAT;

    juce::StringArray currentFile;
    ChopChopBrowserComp createdFiles { juce::FileBrowserComponent::FileChooserFlags::openMode | juce::FileBrowserComponent::FileChooserFlags::canSelectFiles, juce::File(), nullptr, nullptr };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChopChopAudioProcessorEditor)
};
