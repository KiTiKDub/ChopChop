/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ChopChopAudioProcessorEditor::ChopChopAudioProcessorEditor (ChopChopAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), dnd(p), overlay(p)
{
    updateRSWL();
    setSize (700, 400);
    setLookAndFeel(&lnf);

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

    chops->setName("Chops");
    skew->setName("Skew");

    addAndMakeVisible(*chops);
    addAndMakeVisible(*skew);
    addAndMakeVisible(chopChop);
    addAndMakeVisible(history);
    addAndMakeVisible(dragToDaw);
    addChildComponent(overlay,2);
    addChildComponent(createdFiles);
    addChildComponent(back);

    dragToDaw.onDrag = [this](dragToDawButton&, const juce::MouseEvent&)
        {
            performExternalDragDropOfFiles(currentFile, true, this);
        };

    history.onClick = [this, &p]()
        {
            chops->setVisible(false);
            skew->setVisible(false);
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
            skew->setVisible(true);
            chopChop.setVisible(true);
            dragToDaw.setVisible(true);
            dnd.setVisible(true);
            history.setVisible(true);

            createdFiles.setVisible(false);
            createdFiles.setRoot(juce::File());
            back.setVisible(false);
        };

    skew->onDragStart = [this]()
    {
        overlay.setVisible(true);
        dnd.setAlpha(.5f);
    };

    skew->onDragEnd = [this]()
    {
        overlay.setVisible(false);
        dnd.setAlpha(1.f);
    };

    addAndMakeVisible(dnd,0);
    addAndMakeVisible(gumroad);
    startTimerHz(10);
}

ChopChopAudioProcessorEditor::~ChopChopAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void ChopChopAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
    auto bounds = getLocalBounds();

    auto waveformArea = bounds.reduced(bounds.getWidth() * .15, bounds.getHeight() * .2);
    auto leftSide = bounds.removeFromLeft(bounds.getWidth() * .15);
    auto rightSide = bounds.removeFromRight(bounds.getWidth() * .177);
    rightSide.removeFromBottom(rightSide.getHeight() * .8);
    auto logoArea = rightSide;
    logoArea.removeFromLeft(logoArea.getWidth() * .35);
    logoArea.removeFromBottom(logoArea.getHeight() * .1);
    auto top = bounds.removeFromTop(bounds.getHeight() * .2);
    top.removeFromLeft(top.getWidth() * .2);
    g.setColour(juce::Colours::white);

    auto logo = juce::ImageCache::getFromMemory(BinaryData::KITIK_LOGO_NO_BKGD_png, BinaryData::KITIK_LOGO_NO_BKGD_pngSize);
    g.drawImage(logo, logoArea.toFloat(), juce::RectanglePlacement::centred);

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
    auto logoArea = bounds.removeFromRight(bounds.getWidth() * .1);
    logoArea.removeFromTop(logoArea.getHeight() * .67);
    auto chopsArea = bounds.removeFromRight(bounds.getWidth() * .15);
    auto otherButtons = bounds.removeFromLeft(bounds.getWidth() * .25);
    back.setBounds(otherButtons);
    auto skewArea = bounds.removeFromRight(bounds.getWidth() * .2);

    auto dragArea = otherButtons.removeFromTop(otherButtons.getHeight() * .5);
    dragArea.reduce(2, 2);
    otherButtons.reduce(2, 2);
    bounds.reduce(2, 2);

    chops->setBounds(chopsArea);
    skew->setBounds(skewArea);
    chopChop.setBounds(bounds);
    history.setBounds(otherButtons);
    dnd.setBounds(waveformArea);
    overlay.setBounds(waveformArea);
    dragToDaw.setBounds(dragArea);

    auto font = juce::Font();

    gumroad.setFont(juce::Font(), false, juce::Justification::centred);
    gumroad.setColour(0x1001f00, juce::Colours::white);
    gumroad.setBounds(logoArea);

}

void ChopChopAudioProcessorEditor::timerCallback()
{
    if (createdFiles.fileReloaded)
    {
        createdFiles.fileReloaded = false;
        currentFile = createdFiles.clickedFile.getFullPathName();
        audioProcessor.loadFile(createdFiles.clickedFile.getFullPathName());

        chops->setVisible(true);
        skew->setVisible(true);
        chopChop.setVisible(true);
        dragToDaw.setVisible(true);
        dnd.setVisible(true);
        history.setVisible(true);

        createdFiles.setVisible(false);
        back.setVisible(false);
        repaint();
    }

    if(overlay.isVisible())
    {
        overlay.repaint();
    }

}

void ChopChopAudioProcessorEditor::updateRSWL()
{
    auto& chopsParam = getParam(audioProcessor.apvts, "chops");
    auto& skewParam = getParam(audioProcessor.apvts, "skew");

    chops = std::make_unique<RotarySliderWithLabels>(&chopsParam, "Sections", "chops");
    skew = std::make_unique<RotarySliderWithLabels>(&skewParam, "", "Skew");

    makeAttachment(chopsAT, audioProcessor.apvts, "chops", *chops);
    makeAttachment(skewAT, audioProcessor.apvts, "skew", *skew);

    addLabelPairs(chops->labels, 1, 3, chopsParam, "" );
    addLabelPairs(skew->labels, 1, 3, skewParam, "");

    chops.get()->onValueChange = [this, &chopsParam]()
        {
            addLabelPairs(chops->labels, 1, 3, chopsParam, "" );
        };
    skew.get()->onValueChange = [this, &skewParam]()
    {
        addLabelPairs(skew->labels, 1, 3, skewParam, "");
    };
}
