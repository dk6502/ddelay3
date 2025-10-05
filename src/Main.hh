#pragma once

#include "BinaryData.h"
#include "Delay.hh"
#include "Theme.hh"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_core/juce_core.h"
#include "juce_events/juce_events.h"
#include "juce_graphics/juce_graphics.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include <memory>
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
                ),
        params(*this, nullptr, "Params", createParameterLayout()) {
  }

  ~AudioPluginAudioProcessor() override {};

  const juce::String getName() const override { return JucePlugin_Name; }
  bool hasEditor() const override { return true; };
  juce::AudioProcessorEditor *createEditor() override;

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
    delay.reinit(getTotalNumInputChannels(), sampleRate);
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
        channelData[sample] += delay.process_sample(
            channelData[sample], *params.getRawParameterValue("feedback"),
            channel, *params.getRawParameterValue("time"), 140,
            *params.getRawParameterValue("timed"));
      }
    }
    juce::ignoreUnused(totalNumOutputChannels);
  };

  juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(
        std::make_unique<juce::AudioParameterBool>("timed", "Timed", true));
    layout.add(std::make_unique<juce::AudioParameterInt>("time", "Delay Time",
                                                         1, 8, 4));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "feedback", "Feedback", juce::NormalisableRange<float>(0.0f, 1.0f),
        0.5f));
    return layout;
  }

  // This is where class members go
private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessor)
  // All Params stored here
  juce::AudioProcessorValueTreeState params;
  dkdsp::Delay delay;
};

// This is where all the GUI logic goes

class AudioPluginAudioProcessorEditor final
    : public juce::AudioProcessorEditor {
private:
  // Feedback components
  juce::Slider feedbackSlider;
  juce::Label feedbackLabel;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
      feedbackAttachment;
  // Delay Time Components
  juce::Slider timeSlider;
  juce::Label timeLabel;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
      timeAttachment;
  // isTimed components
  juce::TextButton isTimed;
  std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
      isTimedAttachment;
  const AudioPluginAudioProcessor &processorRef;

  juce::Label title;
  juce::Image bg;
  OtherLookAndFeel Theme;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorEditor);

public:
    explicit AudioPluginAudioProcessorEditor(
      AudioPluginAudioProcessor &p, juce::AudioProcessorValueTreeState &vts)
      : AudioProcessorEditor(&p), processorRef(p) {
    juce::ignoreUnused(processorRef);


    setLookAndFeel(&Theme);

    setSize(400, 200);

    title.setText("DDelay 3", juce::dontSendNotification);
    title.setBounds(150, 20, 100, 25);
    title.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(title);

    auto const knobSize = 100;
    feedbackAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(
            vts, "feedback", feedbackSlider));
    feedbackSlider.setSliderStyle(
        juce::Slider::SliderStyle::RotaryHorizontalDrag);
    feedbackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    feedbackSlider.setBounds(knobSize - 80, 10, knobSize, knobSize);
    feedbackLabel.setText("Feedback", juce::NotificationType::sendNotification);
    feedbackLabel.setBounds(knobSize - 80, 10 + knobSize, knobSize,
                            knobSize / 4);
    feedbackLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(feedbackLabel);
    addAndMakeVisible(feedbackSlider);

    timeAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(vts, "time",
                                                                 timeSlider));
    timeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalDrag);
    timeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    timeSlider.setBounds(getWidth() - knobSize - 20, 10, knobSize, knobSize);
    timeLabel.setText("Time", juce::dontSendNotification);
    timeLabel.setBounds(getWidth() - knobSize - 20, 10 + knobSize, knobSize,
                        knobSize / 4);
    timeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(timeLabel);
    addAndMakeVisible(timeSlider);

    isTimedAttachment.reset(
        new juce::AudioProcessorValueTreeState::ButtonAttachment(vts, "timed",
                                                                 isTimed));
    isTimed.setButtonText("BPM");
    isTimed.setClickingTogglesState(true);
    isTimed.setBounds(getWidth() / 2 - 40, 150, 80, 20);
    addAndMakeVisible(isTimed);
    bg = juce::ImageFileFormat::loadFrom(BinaryData::bg_png,
                                         BinaryData::bg_pngSize);
  };
  ~AudioPluginAudioProcessorEditor() override {
      setLookAndFeel(nullptr);
  };

  void paint(juce::Graphics &g) override {

    g.drawImageAt(bg, 0, 0);
  };
  void resized() override {};

  // this is where all the components are listed
};
