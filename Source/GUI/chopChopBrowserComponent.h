/*
  ==============================================================================

    chopChopBrowserComponent.h
    Created: 14 Aug 2024 9:54:28am
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

struct ChopChopBrowserComp : public juce::FileBrowserComponent
{
    using juce::FileBrowserComponent::FileBrowserComponent;

    void fileDoubleClicked(const juce::File& file) override
    {
        clickedFile = file;
        reload = true;
    }

    juce::File clickedFile;
    bool reload;
};