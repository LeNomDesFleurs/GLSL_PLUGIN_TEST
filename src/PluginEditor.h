/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"
#include "OpenGLComponent.hpp"


//==============================================================================
/**
 */
class TestpluginAudioProcessorEditor : public juce::AudioProcessorEditor {
 public:
  TestpluginAudioProcessorEditor(TestpluginAudioProcessor &);
  ~TestpluginAudioProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics &) override;
  void resized() override;

 private:
  // OpenGlplugInAudioProcessor &processor;
  OpenGLComponent openGLComponent;
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  TestpluginAudioProcessor &audioProcessor;
  // juce::OpenGLAppComponent OGLComp;
  std::unique_ptr<juce::Drawable> svgimg;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestpluginAudioProcessorEditor)
};
