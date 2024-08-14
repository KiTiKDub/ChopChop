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
      dnd(p)
{
    setSize (500, 250);
    setLookAndFeel(&lnf);

    chops.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    chops.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);

    chopChop.setButtonText("Chop Chop!");
    history.setButtonText("History");
    dragToDaw.setButtonText("Drag to DAW");
    load.setButtonText("Load");
    back.setButtonText("Back");

    chopChop.onClick = [this, &p]() 
        { 
            p.chopFile();
            currentFile = p.getCurrentFile();
            dnd.repaint();  
        };

    addAndMakeVisible(chops);
    addAndMakeVisible(chopChop);
    addAndMakeVisible(history);
    addAndMakeVisible(dragToDaw);
    addChildComponent(createdFiles);
    addChildComponent(load);
    addChildComponent(back);

    dragToDaw.onDrag = [this](dragToDawButton&, const juce::MouseEvent&)
        {
            performExternalDragDropOfFiles(currentFile, true, this);
        };

    history.onClick = [this, &p]()
        {
            chops.setVisible(false);
            chopChop.setVisible(false);
            dragToDaw.setVisible(false);
            dnd.setVisible(false);
            history.setVisible(false);

            createdFiles.setRoot(p.getNewFileLocation());
            createdFiles.setVisible(true);
            load.setVisible(true);
            back.setVisible(true);
            
        };

    load.onClick = [this, &p]()
        {
            p.loadFile(createdFiles.clickedFile.getFullPathName());
            createdFiles.reload = false;

            chops.setVisible(true);
            chopChop.setVisible(true);
            dragToDaw.setVisible(true);
            dnd.setVisible(true);
            history.setVisible(true);

            createdFiles.setVisible(false);
            load.setVisible(false);
            back.setVisible(false);
        };

    back.onClick = [this, &p]()
        {
            chops.setVisible(true);
            chopChop.setVisible(true);
            dragToDaw.setVisible(true);
            dnd.setVisible(true);
            history.setVisible(true);

            createdFiles.setVisible(false);
            load.setVisible(false);
            back.setVisible(false);
        };

    addAndMakeVisible(dnd);
}

ChopChopAudioProcessorEditor::~ChopChopAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
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
    auto secondBounds = bounds;

    auto waveformArea = bounds.reduced(bounds.getWidth() * .15, bounds.getHeight() * .2);
    auto leftSide = bounds.removeFromLeft(bounds.getWidth() * .15);
    auto chopsArea = bounds.removeFromRight(bounds.getWidth() * .177);
    auto dragArea = chopsArea.removeFromTop(chopsArea.getHeight() * .2);
    chopsArea.removeFromBottom(chopsArea.getHeight() * .25);

    auto chopChopArea = bounds.removeFromTop(bounds.getHeight() * .2);
    auto playbackArea = bounds.removeFromBottom(bounds.getHeight() * .25);

    auto historyArea = leftSide.removeFromTop(leftSide.getHeight() * .2);

    chops.setBounds(chopsArea);
    chopChop.setBounds(chopChopArea);
    history.setBounds(historyArea);
    dnd.setBounds(waveformArea);
    dragToDaw.setBounds(dragArea);


    auto buttonArea = secondBounds.removeFromLeft(leftSide.getWidth());
    auto topButton = buttonArea.removeFromTop(buttonArea.getHeight() * .5);
    load.setBounds(buttonArea);
    back.setBounds(topButton);
    createdFiles.setBounds(secondBounds);
}
