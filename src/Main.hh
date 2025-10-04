#pragma once

#include "juce_core/juce_core.h"
#include <juce_audio_processors/juce_audio_processors.h>

class AudioPluginAudioProcessorEditor;

class AudioPluginAudioProcessor final : public juce::AudioProcessor {
public:
  AudioPluginAudioProcessor()
      : AudioProcessor(
            BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
        ) {
  }

  ~AudioPluginAudioProcessor() override {};

  const juce::String getName() const override { return JucePlugin_Name; }
  bool hasEditor() const override { return true; };
  juce::AudioProcessorEditor* createEditor() override;

  bool acceptsMidi() const override {
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
  }

  bool producesMidi() const override {
#if JucePlugin_WantsMidiOutput
    return true;
#else
    return false;
#endif
  }

  bool isMidiEffect() const override {
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
  }

  double getTailLengthSeconds() const override { return 0.0; }
  int getNumPrograms() override { return 1; }
  int getCurrentProgram() override { return 0; }
  void setCurrentProgram(int index) override { juce::ignoreUnused(index); }

  const juce::String getProgramName(int index) override {
    juce::ignoreUnused(index);
    return {};
  }
  void changeProgramName(int index, const juce::String &newName) override {
    juce::ignoreUnused(index, newName);
  }

  void getStateInformation(juce::MemoryBlock &destData) override {
    juce::ignoreUnused(destData);
  };
  void setStateInformation(const void *data, int sizeInBytes) override {
    juce::ignoreUnused(data, sizeInBytes);
  };

  bool isBusesLayoutSupported(const BusesLayout &layouts) const override {
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
      return false;
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
      return false;
#endif
    return true;
#endif
  }

  // This is where the larger DSP logic goes, where effects are put together

  void prepareToPlay(double sampleRate, int samplesPerBlock) override {
    juce::ignoreUnused(sampleRate, samplesPerBlock);
  };
  void releaseResources() override {};

  void processBlock(juce::AudioBuffer<float> &buffer,
                    juce::MidiBuffer &midiMessages) override {
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
      buffer.clear(i, 0, buffer.getNumSamples());
    for (auto channel = 0; channel < totalNumInputChannels; ++channel) {
      auto *channelData = buffer.getWritePointer(channel);
      juce::ignoreUnused(channelData);
      for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
      }
    }
    juce::ignoreUnused(totalNumOutputChannels);
  };

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessor)
};

// This is where all the GUI logic goes

class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor {
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
        : AudioProcessorEditor (&p), processorRef (p) {
            juce::ignoreUnused (processorRef);
            setSize(300, 300);
        };
    ~AudioPluginAudioProcessorEditor() override {};


    void paint (juce::Graphics& g) override {

        g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

        g.setColour (juce::Colours::white);
        g.setFont (15.0f);
        g.drawFittedText ("Milking It", getLocalBounds(), juce::Justification::centred, 1);
    };
    void resized() override {};

private:
    const AudioPluginAudioProcessor& processorRef;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
