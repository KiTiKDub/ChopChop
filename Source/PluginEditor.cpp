/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ChopChopAudioProcessorEditor::ChopChopAudioProcessorEditor (ChopChopAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), dnd(p)
{
    updateRSWL();
    setSize (700, 400);
    setLookAndFeel(&lnf);


    //chops.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    //chops.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);

    chopChop.setButtonText("Chop Chop!");
    history.setButtonText("History");
    dragToDaw.setButtonText("Drag to DAW");
    back.setButtonText("Back");

    chopChop.onClick = [this, &p]() 
        { 
            p.chopFile();
            currentFile = p.getCurrentFile();
            dnd.repaint();  
        };

    addAndMakeVisible(*chops);
    addAndMakeVisible(chopChop);
    addAndMakeVisible(history);
    addAndMakeVisible(dragToDaw);
    addChildComponent(createdFiles);
    addChildComponent(back);

    dragToDaw.onDrag = [this](dragToDawButton&, const juce::MouseEvent&)
        {
            performExternalDragDropOfFiles(currentFile, true, this);
        };

    history.onClick = [this, &p]()
        {
            chops->setVisible(false);
            chopChop.setVisible(false);
            dragToDaw.setVisible(false);
            dnd.setVisible(false);
            history.setVisible(false);

            createdFiles.setRoot(p.getNewFileLocation());
            createdFiles.setVisible(true);
            back.setVisible(true);
            
        };

    back.onClick = [this, &p]()
        {
            chops->setVisible(true);
            chopChop.setVisible(true);
            dragToDaw.setVisible(true);
            dnd.setVisible(true);
            history.setVisible(true);

            createdFiles.setVisible(false);
            back.setVisible(false);
        };

    addAndMakeVisible(dnd);
    startTimerHz(10);
}

ChopChopAudioProcessorEditor::~ChopChopAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void ChopChopAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::black);
    auto bounds = getLocalBounds();

    auto waveformArea = bounds.reduced(bounds.getWidth() * .15, bounds.getHeight() * .2);
    auto leftSide = bounds.removeFromLeft(bounds.getWidth() * .15);
    auto rightSide = bounds.removeFromRight(bounds.getWidth() * .177);
    auto top = bounds.removeFromTop(bounds.getHeight() * .2);
    top.removeFromLeft(top.getWidth() * .2);
    g.setColour (juce::Colours::white);

    if (createdFiles.isVisible())
        g.drawFittedText("Closing the plugin window will perserve history, but removing the plugin will delete all history. So draw to the daw what you like!"
            , top, juce::Justification::centred, 2);
    
}

void ChopChopAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    auto createdFilesArea = bounds;
    createdFilesArea.removeFromTop(createdFilesArea.getHeight() * .2);
    createdFiles.setBounds(createdFilesArea);

    auto waveformArea = bounds.removeFromBottom(bounds.getHeight() * .8);
    auto chopsArea = bounds.removeFromRight(bounds.getWidth() * .15);
    auto otherButtons = bounds.removeFromLeft(bounds.getWidth() * .25);
    back.setBounds(otherButtons);

    auto dragArea = otherButtons.removeFromTop(otherButtons.getHeight() * .5);
    dragArea.reduce(2, 2);
    otherButtons.reduce(2, 2);
    bounds.reduce(2, 2);

    chops->setBounds(chopsArea);
    chopChop.setBounds(bounds);
    history.setBounds(otherButtons);
    dnd.setBounds(waveformArea);
    dragToDaw.setBounds(dragArea);

}

void ChopChopAudioProcessorEditor::timerCallback()
{
    if (createdFiles.fileReloaded)
    {
        createdFiles.fileReloaded = false;
        audioProcessor.loadFile(createdFiles.clickedFile.getFullPathName());

        chops->setVisible(true);
        chopChop.setVisible(true);
        dragToDaw.setVisible(true);
        dnd.setVisible(true);
        history.setVisible(true);

        createdFiles.setVisible(false);
        back.setVisible(false);
    }
}

void ChopChopAudioProcessorEditor::updateRSWL()
{
    auto& chopsParam = getParam(audioProcessor.apvts, "chops");

    chops = std::make_unique<RotarySliderWithLabels>(&chopsParam, "Sections", "chops");

    makeAttachment(chopsAT, audioProcessor.apvts, "chops", *chops);

    addLabelPairs(chops->labels, 1, 3, chopsParam, "" );

    chops.get()->onValueChange = [this, &chopsParam]()
        {
            addLabelPairs(chops->labels, 1, 3, chopsParam, "" );
        };
}