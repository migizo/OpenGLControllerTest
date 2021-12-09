/*
  ==============================================================================

    OpenGLSimple.h
    Created: 3 Dec 2021 8:45:36am
    Author:  migizo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class OpenGLSimple: public juce::Component, public juce::Timer {
public:
    OpenGLSimple();
    ~OpenGLSimple();
    
    // component
    void paint (juce::Graphics&) override;
    void resized() override {}

    // timer
    void timerCallback() override;

private:
    void initializeFragmentByInternalSource();
    void reloadShader();
    
    juce::OpenGLContext openGLContext;
    std::unique_ptr<juce::OpenGLGraphicsContextCustomShader> shader;
    juce::String fragmentString;
    juce::File fragmentShaderFile;
    juce::Time fragmentLastModTime;
};
