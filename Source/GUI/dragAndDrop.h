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
    ~dragAndDropComp() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

private:

    std::unique_ptr<juce::FileChooser> chooser;

    void mouseDoubleClick(const juce::MouseEvent &event) override
    {
      juce::ignoreUnused();
      
      chooser = std::make_unique<juce::FileChooser>("Select a sample", juce::File::getSpecialLocation(juce::File::userDesktopDirectory), "*.wav", "*.mp3", "*.aiff");

      auto folderChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

      chooser->launchAsync(folderChooserFlags, [this](const juce::FileChooser &myChooser)
      {
        juce::File audioFile (myChooser.getResult());
        if(audioFile.exists())
        {
          audioProcessor.loadFile(audioFile.getFullPathName());
          repaint();
        }
      });
      
    }

    ChopChopAudioProcessor& audioProcessor;
    std::vector<float> audioPointsLeft;
    std::vector<float> audioPointsRight;
    juce::AudioBuffer<float> waveformGraph;
};