/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "OpenGLComponents/OpenGLSimple.h"
#include "OpenGLComponents/OpenGLSimpleUniform.h"

//==============================================================================
/**
*/
class OpenGLControllerTestAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
    OpenGLControllerTestAudioProcessorEditor (OpenGLControllerTestAudioProcessor&);
    ~OpenGLControllerTestAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    std::unique_ptr<OpenGLSimple> openGLSimple;
    std::unique_ptr<OpenGLSimpleUniform> openGLSimpleUniform;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    OpenGLControllerTestAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLControllerTestAudioProcessorEditor)
};
