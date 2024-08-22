/*
  ==============================================================================

    dragAndDrop.cpp
    Created: 6 Aug 2024 5:02:23pm
    Author:  kylew

  ==============================================================================
*/

#include "dragAndDrop.h"

dragAndDropComp::dragAndDropComp(ChopChopAudioProcessor& p) : audioProcessor(p)
{

}

dragAndDropComp::~dragAndDropComp()
{

}

void dragAndDropComp::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    g.setColour(juce::Colours::white);
    g.drawRect(getLocalBounds());

    juce::Path p;
    audioPoints.clear();

    auto waveform = audioProcessor.getWaveform();
    if(waveform.getNumSamples() != 0)
    {
        auto buffer = waveform.getReadPointer(0);
        auto scalingFactor = waveform.getNumSamples() / getWidth();

        for (int s = 0; s < waveform.getNumSamples(); s += scalingFactor)
        {
            audioPoints.push_back(buffer[s]);
        }

        p.startNewSubPath(0, getHeight() / 2);

        for (int s = 0; s < audioPoints.size(); s++)
        {
            auto point = juce::jmap<float>(audioPoints[s], -1.0, 1.0, getHeight(), 0);
            p.lineTo(s, point);
        }

        g.strokePath(p, juce::PathStrokeType(1));
    }
}

void dragAndDropComp::resized()
{

}

bool dragAndDropComp::isInterestedInFileDrag(const juce::StringArray& files)
{
    for (auto file : files)
    {
        if (file.contains(".wav") || file.contains(".mp3") || file.contains(".aif"))
        {
        }
        else
        {
            return false;
        }
    }

    return true;
}

void dragAndDropComp::filesDropped(const juce::StringArray& files, int x, int y)
{
    if (isInterestedInFileDrag(files))
    {
        for (auto file : files)
        {
            audioProcessor.loadFile(file);
            repaint();
        }
    }
}
