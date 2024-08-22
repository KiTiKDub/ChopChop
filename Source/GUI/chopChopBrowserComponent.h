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

    void fileClicked(const juce::File& file, const juce::MouseEvent& e) override
    {
        clickedFile = file;
        fileReloaded = true;
    }

    juce::File clickedFile{ juce::File() };
    bool fileReloaded{ false };
};