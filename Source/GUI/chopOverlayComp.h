#pragma once
#include "juce_gui_basics/juce_gui_basics.h"
#include "../PluginProcessor.h"

struct ChopOverlayComp : public juce::Component
{
    ChopOverlayComp(ChopChopAudioProcessor&);
    ~ChopOverlayComp() {}

    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    ChopChopAudioProcessor& audioProcessor;

};
