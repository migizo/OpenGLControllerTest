/*
  ==============================================================================

    OpenGLSimpleUniform.cpp
    Created: 5 Dec 2021 9:56:19am
    Author:  migizo

  ==============================================================================
*/

#include <JuceHeader.h>
#include "OpenGLSimpleUniform.h"

//==============================================================================
OpenGLSimpleUniform::OpenGLSimpleUniform(juce::AudioProcessorValueTreeState& _vts) : vts(_vts)
{
    // openGLコンテキストにコンポーネントを紐づける。(ヘビーウェイト化(?))
    openGLContext.attachTo (*this);
    
    // レンダラーとしてセット
    openGLContext.setRenderer(this);
    
    // repaint loop ON.
    openGLContext.setContinuousRepainting(true);
    
    // paint関数はrenderOpenGLから使用する
    openGLContext.setComponentPaintingEnabled (false);
    
    // opaque = 不透明であると明示するかどうか
    setOpaque(false);

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
//    fragmentShaderFile = juce::File(resourcePath + "simpleUniform.frag");
    fragmentShaderFile = juce::File(resourcePath + "simpleUniformRaymarching.frag");
    
    fragmentString = fragmentShaderFile.loadFileAsString();
    
    // リアルタイム更新のために最後にアクセスした時間と変更した時間を取得
    fragmentShaderFile.setLastAccessTime(juce::Time::getCurrentTime());
    fragmentLastModTime = fragmentShaderFile.getLastModificationTime();
    
    // 配布時などソースコード直書きのシェーダを参照する
    // initializeFragmentByInternalSource();
    
    reloadShader();
    
    startTimerHz(60);
}

//------------------------------------------------------------------------------
OpenGLSimpleUniform::~OpenGLSimpleUniform()
{
    stopTimer();

    // openGLコンテキストへの紐付け解除
    openGLContext.detach();
}

//------------------------------------------------------------------------------
void OpenGLSimpleUniform::paint (juce::Graphics& g)
{
    auto mouseRel = getMouseXYRelative().toFloat();
    auto bounds = g.getClipBounds().toFloat();
    
    // shaderの背景、shaderで描画失敗時にも表示される。
    g.fillCheckerBoard (bounds, 48.0f, 48.0f, juce::Colours::lightgrey, juce::Colours::white);

    // shaderが有効なものであれば更新
    if (shader.get() != nullptr)
    {
        shader->onShaderActivated = [&](juce::OpenGLShaderProgram& p)
        {
            p.setUniform("uPan", vts.getRawParameterValue(PARAM_PAN)->load());
            p.setUniform("uVolume", vts.getRawParameterValue(PARAM_VOL)->load());
            
            GLfloat uTime = (float)juce::Time::getMillisecondCounterHiRes() * 0.001f;
            p.setUniform("uTime", uTime);
            
            p.setUniform("uResolution", bounds.getWidth(), bounds.getHeight());
        };

        shader->fillRect (g.getInternalContext(), g.getClipBounds());
    }
}

//------------------------------------------------------------------------------
void OpenGLSimpleUniform::mouseMove(const juce::MouseEvent& event) {    
    auto mouseRel = getMouseXYRelative().toFloat();

    const auto& mappedPan = juce::jmap((float)mouseRel.x / (float)getWidth(), -1.0f, 1.0f);
    vts.getRawParameterValue(PARAM_PAN)->store(juce::jmin(1.0f, juce::jmax(-1.0f, mappedPan)));
    
    vts.getRawParameterValue(PARAM_VOL)->store(1.0f - juce::jmin(1.0f, juce::jmax(0.0f, (float)mouseRel.y / (float)getHeight())));
}

//------------------------------------------------------------------------------
void OpenGLSimpleUniform::timerCallback() {
    // シェーダーファイルに最後にアクセスした時間から一定時間経過していれば、シェーダーファイルが変更された時間を確認して変わっていれば際描画処理を行う。
    auto currentTime = juce::Time::getCurrentTime();
    if (currentTime.toMilliseconds() - fragmentShaderFile.getLastAccessTime().toMilliseconds() > 1000) {
        fragmentShaderFile.setLastAccessTime(currentTime);
        if (fragmentLastModTime != fragmentShaderFile.getLastModificationTime()) {
            fragmentLastModTime = fragmentShaderFile.getLastModificationTime();
            fragmentString = fragmentShaderFile.loadFileAsString();
            reloadShader();
        }
    }
}

//------------------------------------------------------------------------------
void OpenGLSimpleUniform::renderOpenGL() {
    // 描画クリア
    juce::OpenGLHelpers::clear (juce::Colours::black);
    
    auto desktopScale = openGLContext.getRenderingScale();
    std::unique_ptr<juce::LowLevelGraphicsContext> glContext (createOpenGLGraphicsContext (openGLContext,
        juce::roundToInt (desktopScale * getWidth()),
        juce::roundToInt (desktopScale * getHeight())));
    
    if (glContext.get() == nullptr)
        return;

    // 描画処理
    juce::Graphics g (*glContext.get());
    paint (g);
}

//------------------------------------------------------------------------------
void OpenGLSimpleUniform::initializeFragmentByInternalSource() {
    fragmentString = juce::String(R"(
        /* shader file begin */

      uniform float uTime;
      uniform vec2 uMouseNorm;
      uniform vec2 uResolution;

      void main() {
        float w = 0.0;
        vec2 uv = gl_FragCoord.xy / uResolution.xy;
        vec2 mouse = vec2(uMouseNorm.x, 1.0 - uMouseNorm.y);

        // line
        float lineWidth = 0.01;
        w += step(mouse.x - lineWidth * 0.5, uv.x) * step(uv.x, mouse.x + lineWidth * 0.5);
        w += step(mouse.y - lineWidth * 0.5, uv.y) * step(uv.y, mouse.y + lineWidth * 0.5);

        // circle
        float radius = 0.1;
        w += smoothstep(radius + 0.2, radius, distance(mouse.xy, uv.xy));

        // wave
        float dist = distance(vec2(0.5), uv);
        float r = (sin(pow(dist * 2.0, 1.5) * 10.0 - uTime + 0.0) + 1.0) * 0.5;
        float g = (sin(pow(dist * 2.0, 1.5) * 10.0 - uTime + 0.75) + 1.0) * 0.5;
        float b = (sin(pow(dist * 2.0, 1.5) * 10.0 - uTime + 1.4) + 1.0) * 0.5;

        gl_FragColor = vec4(vec3(w) + vec3(r, g, b), 1.0);
      }

                                  
        /* shader file end */
    )");
}

//------------------------------------------------------------------------------
void OpenGLSimpleUniform::reloadShader() {
    // シェーダー更新処理
    if (shader.get() == nullptr || (shader->getFragmentShaderCode() != fragmentString)) {
        // fragment shaderに変更があればshaderを更新する
        if (fragmentString.isNotEmpty())
        {
            shader.reset (new juce::OpenGLGraphicsContextCustomShader (fragmentString));
        }
    }
}
