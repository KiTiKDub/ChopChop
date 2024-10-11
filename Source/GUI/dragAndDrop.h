/*
  ==============================================================================

    dragAndDrop.h
    Created: 6 Aug 2024 5:02:23pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include "juce_gui_basics/juce_gui_basics.h"
#include "../PluginProcessor.h"

struct dragAndDropComp : public juce::Component, public juce::FileDragAndDropTarget
{
    dragAndDropComp(ChopChopAudioProcessor&);
    ~dragAndDropComp();

    void paint(juce::Graphics& g) override;
    void resized() override;

    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

private:

    ChopChopAudioProcessor& audioProcessor;
    std::vector<float> audioPoints;
};