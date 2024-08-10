/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ChopChopAudioProcessorEditor::ChopChopAudioProcessorEditor (ChopChopAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), chopsAT(p.apvts, "chops", chops),
    historyAT(p.apvts, "history", history), playbackAT(p.apvts, "playback", playback),
    chopChopAT(p.apvts, "chopChop", chopChop), dnd(p)
{
    setSize (500, 250);

    chops.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    chops.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);

    chopChop.setButtonText("Chop Chop!");
    history.setButtonText("History");
    playback.setButtonText("Playback");

    chopChop.onClick = [this, &p]() 
        { 
            p.chopFile();
            dnd.repaint();  
        };

    addAndMakeVisible(chops);
    addAndMakeVisible(chopChop);
    addAndMakeVisible(history);
    addAndMakeVisible(playback);

    addAndMakeVisible(dnd);
}

ChopChopAudioProcessorEditor::~ChopChopAudioProcessorEditor()
{
}

//==============================================================================
void ChopChopAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    auto bounds = getLocalBounds();

    auto waveformArea = bounds.reduced(bounds.getWidth() * .15, bounds.getHeight() * .2);
    auto leftSide = bounds.removeFromLeft(bounds.getWidth() * .15);
    auto rightSide = bounds.removeFromRight(bounds.getWidth() * .177);
    auto top = bounds.removeFromTop(bounds.getHeight() * .2);
    g.setColour (juce::Colours::white);
    g.drawRect(waveformArea);
    //g.drawRect(top);
    
}

void ChopChopAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    auto waveformArea = bounds.reduced(bounds.getWidth() * .15, bounds.getHeight() * .2);
    auto leftSide = bounds.removeFromLeft(bounds.getWidth() * .15);
    auto chopsArea = bounds.removeFromRight(bounds.getWidth() * .177);
    chopsArea.removeFromTop(chopsArea.getHeight() * .2);
    chopsArea.removeFromBottom(chopsArea.getHeight() * .25);

    auto chopChopArea = bounds.removeFromTop(bounds.getHeight() * .2);
    auto playbackArea = bounds.removeFromBottom(bounds.getHeight() * .25);

    auto historyArea = leftSide.removeFromTop(leftSide.getHeight() * .2);

    chops.setBounds(chopsArea);
    chopChop.setBounds(chopChopArea);
    playback.setBounds(playbackArea);
    history.setBounds(historyArea);
    dnd.setBounds(waveformArea);
}