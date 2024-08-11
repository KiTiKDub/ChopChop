/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <random>

//==============================================================================
ChopChopAudioProcessor::ChopChopAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{

    chops = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter("chops"));
    history = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("history"));
    chopChop = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("chopChop"));
    playback = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("playback"));

    sampler.addVoice(new juce::SamplerVoice());
    manager.registerBasicFormats();

}

ChopChopAudioProcessor::~ChopChopAudioProcessor()
{
    reader = nullptr;
}

//==============================================================================
const juce::String ChopChopAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ChopChopAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ChopChopAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ChopChopAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ChopChopAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ChopChopAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ChopChopAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ChopChopAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ChopChopAudioProcessor::getProgramName (int index)
{
    return {};
}

void ChopChopAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ChopChopAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    sampler.setCurrentPlaybackSampleRate(sampleRate);
}

void ChopChopAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ChopChopAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ChopChopAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    sampler.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool ChopChopAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ChopChopAudioProcessor::createEditor()
{
    return new ChopChopAudioProcessorEditor (*this);
}

//==============================================================================
void ChopChopAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void ChopChopAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid())
        apvts.replaceState(tree);
}

juce::AudioProcessorValueTreeState::ParameterLayout ChopChopAudioProcessor::createParameterLayout()
{
    using namespace juce;
    AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<AudioParameterInt>("chops", "Chops", 1, 100, 5));
    layout.add(std::make_unique<AudioParameterBool>("chopChop", "Chop Chop!", false));
    layout.add(std::make_unique<AudioParameterBool>("history", "History", false));
    layout.add(std::make_unique<AudioParameterBool>("playback", "Playback", false));

    return layout;
}

void ChopChopAudioProcessor::loadFile(const juce::String& path)
{
    sampler.clearSounds();

    auto file = juce::File(path);
    reader = manager.createReaderFor(file);

    int length = static_cast<int>(reader->lengthInSamples);
    waveform.setSize(1,length);
    reader->read(&waveform, 0, length, 0, true, false);

    juce::BigInteger range;
    range.setRange(0, 128, true);

    sampler.addSound(new juce::SamplerSound("sample", *reader, range, 60, .01, .01, 30));
}

void ChopChopAudioProcessor::chopFile()
{
    int length = static_cast<int>(reader->lengthInSamples);
    int i = 0;

    auto waveformReader = waveform.getReadPointer(0);
    int seperate = length / chops->get();

    holder.clear();
    holder.setSize(1, seperate);
    audioBuffers.clear();

    //Break sample into sections
    for (int s = 0; s < length; s++)
    {
        holder.addSample(0, i, waveformReader[s]);
        i++;

        if (i == seperate)
        {
            audioBuffers.push_back(holder);
            holder.clear();
            i = 0;
        }

    }

    //Shuffle
    auto rd = std::random_device{};
    auto rng = std::default_random_engine{ rd() };
    std::shuffle(audioBuffers.begin(), audioBuffers.end(), rng);

    waveform.clear();

    //piece back together
    for (int buffer = 0; buffer < audioBuffers.size(); buffer++)
    {
        auto read = audioBuffers[buffer].getReadPointer(0);
        //auto range = audioBuffers[buffer].findMinMax(0, 0, audioBuffers[buffer].getNumSamples());
        //auto check = range.clipValue(0);
        auto length = audioBuffers[buffer].getNumSamples();
        waveform.addFrom(0, length * buffer, audioBuffers[buffer], 0, 0, length);
    }

    juce::BigInteger range;
    range.setRange(0, 128, true);

    juce::WavAudioFormat format;
    auto directory = getNewFileLocation();
    auto file = directory.getChildFile("gen.wav");
    std::unique_ptr<juce::AudioFormatWriter> writer;
    writer.reset(format.createWriterFor(new juce::FileOutputStream(file), //need to write to a path, and then call that path again.
        48000.0,
        waveform.getNumChannels(),
        24,
        {},
        0));
    if (writer != nullptr)
        writer->writeFromAudioSampleBuffer(waveform, 0, waveform.getNumSamples());

    reader = manager.createReaderFor(file);

    sampler.clearSounds();
    sampler.addSound(new juce::SamplerSound("sample", *reader, range, 60, .01, .01, 30));
}

juce::File ChopChopAudioProcessor::getNewFileLocation()
{
    //windows
    auto kitikFolder = juce::File::getSpecialLocation(juce::File::commonApplicationDataDirectory).getChildFile("Application Support").getChildFile(ProjectInfo::companyName);
    auto pluginFolder = kitikFolder.getChildFile(ProjectInfo::projectName);
    auto generatedSamples = pluginFolder.getChildFile("GeneratedSamples");

    if (!generatedSamples.exists())
        generatedSamples.createDirectory();

    auto testFile = generatedSamples.getChildFile("gen.wav");
    if (testFile.exists())
        testFile.deleteFile(); //delete file causes it to crash, may just have to skip and have it start generating new files everytime. 
                               //should make getting history easier than expected...

    return generatedSamples;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChopChopAudioProcessor();
}
