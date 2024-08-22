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
#include "GUI/rotarySliderWithLabels.h"

//==============================================================================
/**
*/
class ChopChopAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                      public juce::DragAndDropContainer,
                                      public juce::Timer
{
public:
    ChopChopAudioProcessorEditor (ChopChopAudioProcessor&);
    ~ChopChopAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:

    void updateRSWL();

    ChopChopAudioProcessor& audioProcessor;

    Laf lnf;

    dragAndDropComp dnd;

    juce::TextButton chopChop, history, back;
    dragToDawButton dragToDaw;
    std::unique_ptr<RotarySliderWithLabels> chops;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chopsAT;

    juce::StringArray currentFile;
    ChopChopBrowserComp createdFiles { juce::FileBrowserComponent::FileChooserFlags::openMode | juce::FileBrowserComponent::FileChooserFlags::canSelectFiles, juce::File(), nullptr, nullptr };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChopChopAudioProcessorEditor)
};
