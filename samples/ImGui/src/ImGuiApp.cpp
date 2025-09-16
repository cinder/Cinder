#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Surface.h"
#include "cinder/CinderImGui.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ImGuiApp : public App {
public:
    void setup() override;
    void update() override;
    void draw() override;
    void cleanup() override;

private:
    void createSampleTexture();
    void drawCinderExtensions();

    // Demo window states
    bool mShowDemoWindow = true;
    bool mShowTestWindow = true;
    bool mShowExtensions = true;

    // Basic test controls (from original)
    float mFloatValue = 0.5f;
    int mIntValue = 42;
    vec3 mColorValue = vec3(0.5f, 0.3f, 0.8f);
    char mTextBuffer[256] = "Hello, ImGui v1.92.2b!";
    bool mCheckboxValue = false;
    int mRadioValue = 0;
    int mComboSelection = 0;
    vector<string> mComboItems = { "Option 1", "Option 2", "Option 3", "Option 4" };
    int mListBoxSelection = 0;
    vector<string> mListBoxItems = { "Item A", "Item B", "Item C", "Item D", "Item E" };

    // CinderImGui unique API demonstrations
    glm::vec2 mVec2 = glm::vec2(1.0f, 2.0f);
    glm::vec3 mVec3 = glm::vec3(1.0f, 2.0f, 3.0f);
    glm::vec4 mVec4 = glm::vec4(1.0f, 2.0f, 3.0f, 4.0f);
    glm::ivec2 mIVec2 = glm::ivec2(10, 20);
    glm::ivec3 mIVec3 = glm::ivec3(10, 20, 30);
    glm::ivec4 mIVec4 = glm::ivec4(10, 20, 30, 40);

    ci::Colorf mColorf = ci::Colorf(0.8f, 0.4f, 0.2f);
    ci::ColorAf mColorAf = ci::ColorAf(1.0f, 0.5f, 0.3f, 0.8f);

    gl::Texture2dRef mTexture;
    vec2 mTextureSize = vec2(128, 128);
    vec2 mUV0 = vec2(0.0f, 0.0f);
    vec2 mUV1 = vec2(1.0f, 1.0f);
    vec4 mTint = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    vec4 mBorder = vec4(1.0f, 0.0f, 0.0f, 1.0f);
};

void ImGuiApp::setup()
{
    CI_LOG_I("Initializing ImGui Test Application");

    // Initialize ImGui
    ImGui::Initialize(ImGui::Options()
        .window(getWindow())
        .enableKeyboard(true)
    );
    ImGui::GetStyle().ScaleAllSizes( getWindowContentScale() );
    ImGui::GetStyle().FontScaleMain = getWindowContentScale();

    // Create a simple test texture
    createSampleTexture();

    CI_LOG_I("ImGui Version: " << IMGUI_VERSION);
}

void ImGuiApp::createSampleTexture()
{
    // Create simple checker pattern texture
    const int size = 128;
    Surface8u surface(size, size, false);
    auto iter = surface.getIter();
    while (iter.line()) {
        while (iter.pixel()) {
            int x = iter.x() / 16;
            int y = iter.y() / 16;
            uint8_t color = ((x + y) % 2) ? 200 : 50;
            iter.r() = color;
            iter.g() = color;
            iter.b() = color;
        }
    }
    mTexture = gl::Texture2d::create(surface);
}

void ImGuiApp::update()
{
}

void ImGuiApp::draw()
{
    gl::clear(Color(0.1f, 0.1f, 0.1f));

    gl::color( 1, 1, 1 );
    gl::drawSolidCircle( vec2( getWindowCenter() ), (float)std::max( getWindowWidth(), getWindowHeight() ) / 2 );
    
    // Show ImGui demo window
    if (mShowDemoWindow) {
        ImGui::ShowDemoWindow(&mShowDemoWindow);
    }

    // Basic test window (original functionality)
    if (mShowTestWindow) {
        ImGui::Begin("Basic ImGui Test", &mShowTestWindow);

        ImGui::Text("ImGui %s with Cinder", IMGUI_VERSION);
        ImGui::Separator();

        ImGui::Checkbox("Show Demo Window", &mShowDemoWindow);
        ImGui::Checkbox("Show CinderImGui Extensions", &mShowExtensions);
        ImGui::Separator();

        // Basic Controls
        if (ImGui::CollapsingHeader("Standard ImGui Controls", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::SliderFloat("Float Slider", &mFloatValue, 0.0f, 1.0f);
            ImGui::SliderInt("Int Slider", &mIntValue, 0, 100);
            ImGui::ColorEdit3("Color Picker", &mColorValue.x);
            ImGui::InputText("Text Input", mTextBuffer, sizeof(mTextBuffer));
            ImGui::Checkbox("Checkbox", &mCheckboxValue);
            ImGui::Combo("Combo Box", &mComboSelection, mComboItems);
            ImGui::ListBox("List Box", &mListBoxSelection, mListBoxItems, 4);
        }

        ImGui::End();
    }

    // CinderImGui Extensions Demo
    if (mShowExtensions) {
        drawCinderExtensions();
    }
}

void ImGuiApp::drawCinderExtensions()
{
    ImGui::Begin("CinderImGui Extensions", &mShowExtensions);

    ImGui::Text("Unique CinderImGui API Functions");
    ImGui::Separator();

    // GLM Vector Controls
    if (ImGui::CollapsingHeader("GLM Vector Types", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("These functions work directly with GLM types (showing a couple of each):");

        // Float vectors - showing DragFloat and SliderFloat variants
        ImGui::DragFloat2("DragFloat2 (glm::vec2)", &mVec2, 0.1f, -10.0f, 10.0f);
        ImGui::DragFloat3("DragFloat3 (glm::vec3)", &mVec3, 0.1f, -10.0f, 10.0f);
        ImGui::SliderFloat2("SliderFloat2 (glm::vec2)", &mVec2, -5.0f, 5.0f);
        ImGui::SliderFloat3("SliderFloat3 (glm::vec3)", &mVec3, -5.0f, 5.0f);

        ImGui::Spacing();

        // Integer vectors - showing DragInt and InputInt variants
        ImGui::DragInt2("DragInt2 (glm::ivec2)", &mIVec2, 1.0f, 0, 100);
        ImGui::DragInt3("DragInt3 (glm::ivec3)", &mIVec3, 1.0f, 0, 100);
        ImGui::InputInt2("InputInt2 (glm::ivec2)", &mIVec2);
        ImGui::InputInt3("InputInt3 (glm::ivec3)", &mIVec3);

        ImGui::Separator();
    }

    // Cinder Color Types
    if (ImGui::CollapsingHeader("Cinder Color Types", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("These functions work directly with Cinder color types:");

        ImGui::ColorEdit3("ColorEdit3 (ci::Colorf)", &mColorf);
        ImGui::ColorEdit4("ColorEdit4 (ci::ColorAf)", &mColorAf);

        ImGui::Spacing();

        ImGui::Text("ColorPicker with triangle selector:");
        ImGui::ColorPicker4("ColorPicker4 (Triangle)", &mColorAf, ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_DisplayRGB);

        ImGui::Separator();
    }

    // Cinder Texture Display
    if (ImGui::CollapsingHeader("Cinder Texture Display", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Image() function with ci::gl::Texture2dRef:");

        if (mTexture) {
            ImGui::SliderFloat2("Display Size", &mTextureSize.x, 32.0f, 256.0f);
            ImGui::DragFloat2("UV Start", &mUV0.x, 0.01f, -1.0f, 2.0f);
            ImGui::DragFloat2("UV End", &mUV1.x, 0.01f, -1.0f, 2.0f);
            ImGui::ColorEdit4("Tint Color", &mTint.x);
            ImGui::ColorEdit4("Border Color", &mBorder.x);

            // The key unique function: ImGui::Image with ci::gl::Texture2dRef
            ImGui::Image(mTexture, mTextureSize, mUV0, mUV1, mTint, mBorder);

            ImGui::Text("Texture: %dx%d", mTexture->getWidth(), mTexture->getHeight());
        }

        ImGui::Separator();
    }

    // std::vector Integration
    if (ImGui::CollapsingHeader("std::vector Integration")) {
        ImGui::Text("Enhanced Combo/ListBox with std::vector<std::string>:");

        ImGui::Combo("Combo (std::vector)", &mComboSelection, mComboItems);
        ImGui::ListBox("ListBox (std::vector)", &mListBoxSelection, mListBoxItems, 3);
    }

    ImGui::End();
}

void ImGuiApp::cleanup()
{
    CI_LOG_I("Cleaning up ImGui Test Application");
}

CINDER_APP(ImGuiApp, RendererGl, [](App::Settings *settings) {
    settings->setWindowSize(1200, 800);
    settings->setTitle("ImGui v1.92.2b Test - CinderImGui Extensions");
    settings->setHighDensityDisplayEnabled( false ); // to make this 'true' on macOS set the NSHighResolutionCapable Info.plist property to YES
})
