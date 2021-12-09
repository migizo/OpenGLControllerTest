/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OpenGLControllerTestAudioProcessorEditor::OpenGLControllerTestAudioProcessorEditor (OpenGLControllerTestAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    // ComponentPeer :
    // Componentクラスの内部で使用されている。
    // ComponentPeer自体は抽象クラスで,プラットフォーム毎の実装が内部で用意されている
    if (juce::ComponentPeer* peer = getPeer()) {
        // macの場合
        // 0:Software Renderer -> NSView等
        // 1:CoreGraphics Renderer -> CoreGraphicsでの実装
        // サンプルを見ると0の値を指定している。
         peer->setCurrentRenderingEngine (0);
    }
    
//    openGLSimple.reset(new OpenGLSimple());
//    addAndMakeVisible (openGLSimple.get());
//    openGLSimple->setBounds (0, 0, getWidth(), getHeight());
//
    openGLSimpleUniform.reset(new OpenGLSimpleUniform(audioProcessor.vts));
    addAndMakeVisible (openGLSimpleUniform.get());
    openGLSimpleUniform->setBounds (0, 0, 400, 300);
    
    setSize (400, 300);
}

OpenGLControllerTestAudioProcessorEditor::~OpenGLControllerTestAudioProcessorEditor()
{
    openGLSimple = nullptr;
    openGLSimpleUniform = nullptr;
}

//==============================================================================
void OpenGLControllerTestAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void OpenGLControllerTestAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
