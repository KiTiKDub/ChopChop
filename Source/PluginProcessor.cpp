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

    sampler.addVoice(new juce::SamplerVoice());
    manager.registerBasicFormats();

}

ChopChopAudioProcessor::~ChopChopAudioProcessor()
{
    sampler.clearSounds();
    sampler.clearVoices();

    auto directory = getNewFileLocation();
    directory.deleteRecursively(false);
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

    layout.add(std::make_unique<AudioParameterInt>(ParameterID{"chops",1}, "Chops", 2, 100, 5));

    return layout;
}

void ChopChopAudioProcessor::loadFile(const juce::String& path)
{
    sampler.clearSounds();

    orignalFile = juce::File(path);
    reader.reset(manager.createReaderFor(orignalFile));
    filesLoaded++;

    int length = static_cast<int>(reader->lengthInSamples);
    waveform.setSize(1,length);
    reader->read(&waveform, 0, length, 0, true, false);

    juce::BigInteger range;
    range.setRange(0, 128, true);

    sampler.addSound(new juce::SamplerSound("sample", *reader, range, 60, .01, .01, 30));
}

void ChopChopAudioProcessor::chopFile()
{
    if (reader == nullptr)
        return;
    
    int fadeLength = 10;
    float increment = 1 / (float)fadeLength;

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
        auto read = audioBuffers[buffer].getWritePointer(0);
        auto length = audioBuffers[buffer].getNumSamples();

        //Add Fade ins and outs
        for (int i = 0; i < fadeLength; i++)
        {
            read[i] = read[i] * i * increment;
            read[length - i - 1] = read[length - i - 1] * i * increment;
        }

        waveform.addFrom(0, length * buffer, audioBuffers[buffer], 0, 0, length);
    }

    auto fileName = getNewFileName();
    writeChoppedFile(fileName);
    readChoppedFile(fileName);
}

juce::StringArray ChopChopAudioProcessor::getCurrentFile()
{
    return currentFile;
}

juce::File ChopChopAudioProcessor::getNewFileLocation()
{
    #if JUCE_WINDOWS
    auto kitikFolder = juce::File::getSpecialLocation(juce::File::commonApplicationDataDirectory).getChildFile("Application Support").getChildFile(ProjectInfo::companyName);
    #elif JUCE_MAC
    auto kitikFolder  = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile(ProjectInfo::companyName);
    #endif

    auto pluginFolder = kitikFolder.getChildFile(ProjectInfo::projectName);
    auto generatedSamples = pluginFolder.getChildFile("GeneratedSamples");

    if (!generatedSamples.exists())
        generatedSamples.createDirectory();

    return generatedSamples;
}

juce::String ChopChopAudioProcessor::getNewFileName()
{
    if (filesChopped < filesLoaded)
    {
        calls = 0;
        filesChopped++;
    }
        
    calls++;
    auto originalName = orignalFile.getFileNameWithoutExtension(); //need to do check to reset calls
    juce::String fileName = originalName + " v" + (juce::String)calls + ".wav";

    return fileName;
}

void ChopChopAudioProcessor::writeChoppedFile(juce::String& fileName)
{
    juce::WavAudioFormat format;
    auto directory = getNewFileLocation();
    auto file = directory.getChildFile(fileName);
    if (file.exists())
        file.deleteFile();
    std::unique_ptr<juce::AudioFormatWriter> writer;
    writer.reset(format.createWriterFor(new juce::FileOutputStream(file),
        48000.0,
        1,
        24,
        {},
        0));
    if (writer != nullptr)
        writer->writeFromAudioSampleBuffer(waveform, 0, waveform.getNumSamples());
}

void ChopChopAudioProcessor::readChoppedFile(juce::String& fileName)
{
    auto file = getNewFileLocation().getChildFile(fileName);
    currentFile = file.getFullPathName();

    juce::BigInteger range;
    range.setRange(0, 128, true);

    sampler.clearSounds();
    reader.reset(manager.createReaderFor(file));
    sampler.addSound(new juce::SamplerSound(fileName, *reader, range, 60, .01, .01, 30));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChopChopAudioProcessor();
}
