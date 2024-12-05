/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "BinaryData.h"
#include "juce_core/juce_core.h"
#include "PluginProcessor.h"
#include "GUI/dragAndDrop.h"
#include "GUI/dragToDawButton.h"
#include "GUI/kLookAndFeel.h"
#include "GUI/chopChopBrowserComponent.h"
#include "GUI/rotarySliderWithLabels.h"
#include "GUI/chopOverlayComp.h"

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

    juce::URL url{ "https://kwhaley5.gumroad.com/" };
    juce::HyperlinkButton gumroad{ "Gumroad", url };

    dragAndDropComp dnd;

    juce::TextButton chopChop, history, back;
    dragToDawButton dragToDaw;
    std::unique_ptr<RotarySliderWithLabels> chops, skew;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chopsAT, skewAT;

    juce::StringArray currentFile;
    ChopChopBrowserComp createdFiles { juce::FileBrowserComponent::FileChooserFlags::openMode | juce::FileBrowserComponent::FileChooserFlags::canSelectFiles, juce::File(), nullptr, nullptr };
    ChopOverlayComp overlay;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChopChopAudioProcessorEditor)
};
