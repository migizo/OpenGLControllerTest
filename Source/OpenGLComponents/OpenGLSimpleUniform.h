/*
  ==============================================================================

    OpenGLSimpleUniform.h
    Created: 5 Dec 2021 9:56:19am
    Author:  migizo

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#define PARAM_PAN "pan"
#define PARAM_VOL "volume"

//==============================================================================
/*
*/
class OpenGLSimpleUniform  : public juce::Component, public juce::Timer, public juce::OpenGLRenderer
{
public:
    OpenGLSimpleUniform(juce::AudioProcessorValueTreeState& _vts);
    ~OpenGLSimpleUniform() override;

    // component
    void paint (juce::Graphics&) override;
    void resized() override {};
    void mouseMove(const juce::MouseEvent& event) override;

    // timer
    void timerCallback() override;
    
    // OpenGLAppComponent
    void newOpenGLContextCreated() override {}; // コンテキストが作成された時に呼ばれる
    void renderOpenGL() override;               // 描画処理を行う
    void openGLContextClosing()override {};     // コンテキスト終了時
    
private:
    void initializeFragmentByInternalSource();
    void reloadShader();

    juce::OpenGLContext openGLContext;
    std::unique_ptr<juce::OpenGLGraphicsContextCustomShader> shader;
    juce::String fragmentString;
    juce::File fragmentShaderFile;
    juce::Time fragmentLastModTime;

    juce::AudioProcessorValueTreeState& vts;
    bool isMousePressed = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLSimpleUniform)
};
