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

    g.setColour(juce::Colours::whitesmoke);
    g.drawRect(getLocalBounds());

    juce::Path leftChannelPath;
    juce::Path rightChannelPath;
    audioPointsLeft.clear();
    audioPointsRight.clear();

    auto waveform = audioProcessor.getWaveform();
    if(waveform.getNumSamples() != 0)
    {
        auto bufferLeft = waveform.getReadPointer(0);
        auto bufferRight = waveform.getReadPointer(1);
        auto scalingFactor = waveform.getNumSamples() / getWidth();

        for (int s = 0; s < waveform.getNumSamples(); s += scalingFactor)
        {
            audioPointsLeft.push_back(bufferLeft[s]);
            audioPointsRight.push_back(bufferRight[s]);
        }
        auto waveFormGraphSize = std::max(audioPointsLeft.size(), audioPointsRight.size());
        waveformGraph.setSize(2, waveFormGraphSize);
        waveformGraph.clear();
        waveformGraph.addFrom(0,0,audioPointsLeft.data(), audioPointsLeft.size());
        waveformGraph.addFrom(1, 0, audioPointsRight.data(), audioPointsRight.size());

        leftChannelPath.startNewSubPath(0, getHeight() * 3 / 4);
        rightChannelPath.startNewSubPath(0, getHeight() / 4);

        for (int s = 0; s < waveformGraph.getNumSamples(); s++)
        {
            auto pointLeftChannel = juce::jmap<float>(waveformGraph.getReadPointer(0)[s], -1.0, 1.0, getHeight() / 2, 0);
            auto pointRightChannel = juce::jmap<float>(waveformGraph.getReadPointer(1)[s], -1.0, 1.0, getHeight(), getHeight() / 2);
            leftChannelPath.lineTo(s, pointLeftChannel);
            rightChannelPath.lineTo(s, pointRightChannel);
        }

        g.strokePath(leftChannelPath, juce::PathStrokeType(1));
        g.strokePath(rightChannelPath, juce::PathStrokeType(1));
    }

    auto reducedBounds = getLocalBounds().reduced(5, 5);
    g.setColour(juce::Colours::whitesmoke.withAlpha(.75f));
    g.drawFittedText("Left Channel", reducedBounds, juce::Justification::topLeft, 1);
    g.drawFittedText("Right Channel", reducedBounds, juce::Justification::centredLeft, 1);
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
