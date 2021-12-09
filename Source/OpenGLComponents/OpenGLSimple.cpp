/*
  ==============================================================================

    OpenGLSimple.cpp
    Created: 3 Dec 2021 8:45:36am
    Author:  migizo

  ==============================================================================
*/

#include "OpenGLSimple.h"
//------------------------------------------------------------------------------
OpenGLSimple::OpenGLSimple() {    
    // openGLコンテキストにコンポーネントを紐づける。(ヘビーウェイト化(?))
    openGLContext.attachTo (*this);
    
    // ビルド後のResourceフォルダからシェーダーを参照するかどうか。
    // falseの場合はプロジェクトのResourceフォルダ(ユーザー/JUCE/projcets/OpenGLControllerTest/Resources)から参照する
    const bool isBuildResource = false;
    juce::String resourcePath;
    
    // ビルド後のプラグインのResourceディレクトリのシェーダーを参照するかどうか
    if (isBuildResource) {
        resourcePath =  juce::File::getSpecialLocation(juce::File::SpecialLocationType::currentApplicationFile).getFullPathName();
#ifdef JUCE_MAC
        resourcePath += "/Contents/Resources/";
#elif JUCE_WINDOWS
        
#endif
    }
    // 開発プロジェクトのResourceフォルダのshaderを読む
    else {
        resourcePath =  juce::File::getSpecialLocation(juce::File::SpecialLocationType::userDocumentsDirectory).getFullPathName() + "/JUCE/projects/OpenGLControllerTest/Resources/";
    }
    
    // shaderのソースコード読み込み
    fragmentShaderFile = juce::File(resourcePath + "simple.frag");
    fragmentString = fragmentShaderFile.loadFileAsString();
    
    // リアルタイム更新のために最後にアクセスした時間と変更した時間を取得
    fragmentShaderFile.setLastAccessTime(juce::Time::getCurrentTime());
    fragmentLastModTime = fragmentShaderFile.getLastModificationTime();
    
    // 配布時などソースコード直書きのシェーダを参照する。
    // initializeFragmentByInternalSource();
    
    reloadShader();
    
    startTimerHz(60);
}

//------------------------------------------------------------------------------
OpenGLSimple::~OpenGLSimple() {
    
    stopTimer();

    // openGLコンテキストへの紐付け解除
    openGLContext.detach();
}

//------------------------------------------------------------------------------
void OpenGLSimple::paint (juce::Graphics& g) {
    // shaderの背景、shaderで描画失敗時にも表示される。
    g.fillCheckerBoard (getLocalBounds().toFloat(), 48.0f, 48.0f, juce::Colours::lightgrey, juce::Colours::white);
    
    // shaderが有効なものであれば描画更新
    if (shader.get() != nullptr)
    {
        shader->fillRect (g.getInternalContext(), getLocalBounds());
    }
}

//------------------------------------------------------------------------------
void OpenGLSimple::timerCallback() {
    // シェーダーファイルに最後にアクセスした時間から一定時間経過していれば、シェーダーファイルが変更された時間を確認して変わっていれば際描画処理を行う。
    auto currentTime = juce::Time::getCurrentTime();
    if (currentTime.toMilliseconds() - fragmentShaderFile.getLastAccessTime().toMilliseconds() > 1000) {
        fragmentShaderFile.setLastAccessTime(currentTime);
        if (fragmentLastModTime != fragmentShaderFile.getLastModificationTime()) {
            fragmentLastModTime = fragmentShaderFile.getLastModificationTime();
            fragmentString = fragmentShaderFile.loadFileAsString();
            reloadShader();
            repaint();
        }
    }
}

//------------------------------------------------------------------------------
void OpenGLSimple::initializeFragmentByInternalSource() {
    fragmentString = juce::String(R"(
        /* shader file begin */
                                  
        void main() {
            gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        }
                             
        /* shader file end */
    )");
}

//------------------------------------------------------------------------------
void OpenGLSimple::reloadShader() {
    // シェーダー更新処理
    if (shader.get() == nullptr || (shader->getFragmentShaderCode() != fragmentString)) {
        // fragment shaderに変更があればshaderを更新する
        if (fragmentString.isNotEmpty())
        {
            shader.reset (new juce::OpenGLGraphicsContextCustomShader (fragmentString));
        }
    }
}

