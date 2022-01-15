#include "cs_debug.hpp"
#include "cs_platform.hpp"
#include "cs_graphics.hpp"
#include "cs_state.hpp"

#include <map>

using namespace cs;
using namespace nlohmann;

CS_PRIVATE_SCOPE
{
    struct DebugUiWindow
    {
        DebugUiWindowCallback callback;
        bool open, hovered;
    };

    struct Logger
    {
        FILE* logFile;

        ~Logger()
        {
            if(logFile)
            {
                fclose(logFile);
                logFile = NULL;
            }
        }
    };

    static bool s_deferDebugMode;
    static bool s_newDebugMode;
    static bool s_debugMode;
    static bool s_debugRender;
    static std::map<std::string,DebugUiWindow> s_debugWindows;
    static std::string s_imguiSettingsFileName;
    static Rect s_gameViewport;
    static Logger s_debugLogger;

    static void DoDockSpace()
    {
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|
            ImGuiWindowFlags_NoBringToFrontOnFocus|ImGuiWindowFlags_NoNavFocus|ImGuiWindowFlags_NoDocking;
        ImGuiDockNodeFlags dockSpaceFlags = ImGuiDockNodeFlags_NoDockingInCentralNode|ImGuiDockNodeFlags_NoWindowMenuButton|ImGuiDockNodeFlags_NoCloseButton;

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
        ImGui::Begin("DockSpace", NULL, windowFlags);
        ImGui::PopStyleVar(3);

        ImGuiID dockSpaceID = ImGui::GetID("DockSpace");
        if(!ImGui::DockBuilderGetNode(dockSpaceID))
        {
            // The debug mode always has the game permanently docked into the center of the window.
            ImGui::DockBuilderRemoveNode(dockSpaceID);
            ImGui::DockBuilderAddNode(dockSpaceID, ImGuiDockNodeFlags_None);
            ImGui::DockBuilderDockWindow("Game", dockSpaceID);
            ImGuiDockNode* node = ImGui::DockBuilderGetNode(dockSpaceID);
            node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
            ImGui::DockBuilderFinish(dockSpaceID);
        }
        ImGui::DockSpace(dockSpaceID, ImVec2(0,0), dockSpaceFlags);

        ImGui::End();
    }

    static void DoGameViewport()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));

        // Due to issues with style colors for docked windows we need to also push ImGuiCol_ChildBg and
        // immediately pop it after ImGui::Begin. For info: https://github.com/ocornut/imgui/issues/2700
        ImGui::PushStyleColor(ImGuiCol_WindowBg, GetAppConfig().clearColor);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, GetAppConfig().clearColor);
        ImGui::Begin("Game", NULL, ImGuiWindowFlags_NoScrollbar);
        ImGui::PopStyleColor();

        // Store the bounds of the game viewport window.
        ImVec2 winMin = ImGui::GetWindowContentRegionMin();
        ImVec2 winMax = ImGui::GetWindowContentRegionMax();
        winMin.x += ImGui::GetWindowPos().x;
        winMin.y += ImGui::GetWindowPos().y;
        winMax.x += ImGui::GetWindowPos().x;
        winMax.y += ImGui::GetWindowPos().y;

        s_gameViewport.x = ImGui::GetWindowPos().x;
        s_gameViewport.y = ImGui::GetWindowPos().y;
        s_gameViewport.w = winMax.x - winMin.x;
        s_gameViewport.h = winMax.y - winMin.y;
        // If multiple viewports are enabled then we need to remove the physical window postion.
        if(CS_CHECK_FLAGS(ImGui::GetIO().ConfigFlags, ImGuiConfigFlags_ViewportsEnable))
        {
            s_gameViewport.x -= GetWindowX();
            s_gameViewport.y -= GetWindowY();
        }

        f32 fpsYPos = ImGui::GetContentRegionAvail().y - ImGui::GetTextLineHeightWithSpacing();

        // Center and draw the screen inside of the game viewport.
        Rect screenBounds = gfx::GetScreenBounds();
        ImTextureID textureID = CS_CAST(ImTextureID, CS_CAST(intptr_t, gfx::GetScreenTextureInternal()));
        ImVec2 imageSize(screenBounds.w,screenBounds.h);
        ImVec2 imagePos = (ImGui::GetWindowSize() + ImVec2(0,ImGui::GetCurrentWindow()->TitleBarHeight()) - imageSize) * 0.5f;
        ImGui::SetCursorPos(imagePos);
        ImGui::Image(textureID, imageSize, ImVec2(0,1), ImVec2(1,0)); // Invert UVs for correct direction.

        // Draw the current FPS inside of the game viewport.
        std::string fpsText = "FPS: " + std::to_string(GetCurrentFPS());
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorStartPos().x, fpsYPos));
        ImGui::Text(fpsText.c_str());

        ImGui::PopStyleColor();
        ImGui::End();

        ImGui::PopStyleVar();
    }
}

CS_PUBLIC_SCOPE
{
    CS_API void InitDebugUi()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        s_imguiSettingsFileName = GetDataPath() + "imgui.ini";

        ImGuiIO& io = ImGui::GetIO();
        CS_SET_FLAGS(io.ConfigFlags, ImGuiConfigFlags_NavEnableKeyboard);
        CS_SET_FLAGS(io.ConfigFlags, ImGuiConfigFlags_DockingEnable);
        CS_SET_FLAGS(io.ConfigFlags, ImGuiConfigFlags_ViewportsEnable);
        io.ConfigWindowsResizeFromEdges = true;
        io.IniFilename = s_imguiSettingsFileName.c_str();

        // Setup our custom style and colors for the debug UI.
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        style.WindowRounding = 0.0f;
        style.ChildRounding = 0.0f;
        style.FrameRounding = 0.0f;
        style.GrabRounding = 0.0f;
        style.PopupRounding = 0.0f;
        style.ScrollbarRounding = 0.0f;
        style.TabRounding = 0.0f;

        colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.47f, 0.78f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_Border] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.11f, 0.59f, 0.93f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.00f, 0.47f, 0.78f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.32f, 0.32f, 0.33f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.35f, 0.35f, 0.37f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.35f, 0.35f, 0.37f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 0.47f, 0.78f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.11f, 0.59f, 0.93f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 0.47f, 0.78f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.11f, 0.59f, 0.93f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.11f, 0.59f, 0.93f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.11f, 0.59f, 0.93f, 1.00f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 0.47f, 0.78f, 1.00f);
        colors[ImGuiCol_Separator] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.32f, 0.32f, 0.33f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.00f, 0.47f, 0.78f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.11f, 0.59f, 0.93f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.00f, 0.47f, 0.78f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.11f, 0.59f, 0.93f, 1.00f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_DockingPreview] = ImVec4(0.00f, 0.47f, 0.78f, 1.00f);
        colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_TabActive] = ImVec4(0.00f, 0.47f, 0.78f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.00f, 0.47f, 0.78f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.11f, 0.59f, 0.93f, 1.00f);

        // std::string fontFileName = GetAssetPath<Font>("LiberationMono-Regular");
        // io.Fonts->AddFontFromFileTTF(fontFileName.c_str(), 12.0f);
    }

    CS_API void QuitDebugUi()
    {
        ImGui::DestroyContext();

        json debugState = json::object();
        for(auto& [name,window]: s_debugWindows)
            debugState[ToSnakeCase(name)] = window.open;
        GetState()["debug"] = debugState;
    }

    CS_API void ToggleDebugMode(bool enable)
    {
        s_debugMode = enable;
    }

    CS_API bool IsDebugMode()
    {
        return s_debugMode;
    }

    CS_API void EnableDebugRender(bool enable)
    {
        s_debugRender = enable;
    }

    CS_API bool IsDebugRender()
    {
        return s_debugRender;
    }

    CS_API void UpdateDebugUi()
    {
        // We defer toggling the debug UI so it doesn't happen partway through a debug UI frame.
        if(IsKeyPressed(KeyCode_F1))
        {
            s_newDebugMode = !s_debugMode;
            s_deferDebugMode = true;
        }
        if(IsKeyPressed(KeyCode_F2))
            s_debugRender = !s_debugRender;
    }

    CS_API void BeginDebugUiFrame()
    {
        static bool s_demoWindowOpen = false;

        ImGui::NewFrame();

        if(s_debugMode)
        {
            // Do the dockspace.
            DoDockSpace();

            // Do the main menu bar.
            ImGui::BeginMainMenuBar();
            if(ImGui::BeginMenu("View"))
            {
                for(auto& [name,window]: s_debugWindows)
                    if(ImGui::MenuItem(name.c_str(), NULL, window.open))
                        window.open = !window.open;
                ImGui::Separator();
                if(ImGui::MenuItem("ImGui Demo", NULL, s_demoWindowOpen))
                    s_demoWindowOpen = !s_demoWindowOpen;
                ImGui::EndMenu();
            }
            if(ImGui::MenuItem("Reset Window"))
                ResetWindow();
            ImGui::EndMainMenuBar();

            // Do the debug windows.
            for(auto& [name,window]: s_debugWindows)
            {
                window.hovered = false;
                if(window.open)
                {
                    if(ImGui::Begin(name.c_str(), &window.open, ImGuiWindowFlags_NoCollapse))
                    {
                        window.callback(window.open);
                        window.hovered = ImGui::IsWindowHovered();
                    }
                    ImGui::End();
                }
            }

            // Do the ImGui demo window.
            if(s_demoWindowOpen)
            {
                ImGui::ShowDemoWindow(&s_demoWindowOpen);
            }
        }
    }

    CS_API void EndDebugUiFrame()
    {
        if(s_debugMode)
            DoGameViewport();
        ImGui::Render();

        // We do not want to allow multiple viewports when in fullscreen because it can get confusing and annoying.
        // We do this at the end of the debug frame because if we do it at the start we trigger an ImGui assert.
        ImGuiIO& io = ImGui::GetIO();
        if(IsFullscreen())
            CS_UNSET_FLAGS(io.ConfigFlags, ImGuiConfigFlags_ViewportsEnable);
        else
            CS_SET_FLAGS(io.ConfigFlags, ImGuiConfigFlags_ViewportsEnable);

        if(s_deferDebugMode)
            s_debugMode = s_newDebugMode;
    }

    CS_API void RegisterDebugUiWindow(std::string name, DebugUiWindowCallback windowCallback)
    {
        if(s_debugWindows.find(name) != s_debugWindows.end())
            CS_DEBUG_LOG("Debug window with the name '%s' is already registered.", name.c_str());
        bool open = false;
        json& engineState = GetState();
        if(engineState.contains("debug"))
        {
            std::string stateName = ToSnakeCase(name);
            json& debugState = engineState["debug"];
            open = (debugState.contains(stateName)) ? debugState[stateName] : false; // Restore the state or start off closed.
        }
        s_debugWindows.insert({ name, { windowCallback, open, false } });
    }

    CS_API void UnregisterDebugUiWindow(std::string name)
    {
        s_debugWindows.erase(name);
    }

    CS_API bool DoesDebugUiWantMouseInput()
    {
        if(!s_debugMode) return false;
        for(auto& [name,window]: s_debugWindows)
            if(window.hovered) return true;
        return ImGui::IsAnyItemHovered();
    }

    CS_API Rect GetDebugGameViewport()
    {
        return s_gameViewport;
    }

    CS_API void DebugLog(const char* file, const char* format, ...)
    {
        if(!s_debugLogger.logFile)
        {
            std::string logFileName = GetDataPath() + "debug.log";
            s_debugLogger.logFile = fopen(logFileName.c_str(), "w");
        }

        std::string message;
        va_list(args);
        va_start(args, format);
        s32 length = vsnprintf(NULL, 0, format, args);
        message.resize(length);
        vsnprintf(&message[0], message.length()+1, format, args);
        va_end(args);

        if(s_debugLogger.logFile)
            fprintf(s_debugLogger.logFile, "%s\n", message.c_str());
        if(CS_DEBUG)
            fprintf(stdout, "%s\n", message.c_str());
    }
}
