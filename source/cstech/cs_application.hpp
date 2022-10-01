#pragma once

#include "cs_math.hpp"

#include <string>

CS_PUBLIC_SCOPE
{
    class Application;

    enum AppFlags: u32
    {
        AppFlags_None = 0,
        AppFlags_All  = 0xFFFFFFFF
    };

    struct WindowConfig
    {
        Vec2i size = Vec2i(1280,720);
        Vec2i min = Vec2i(0,0);
        bool vSync = true;
        bool maximized = false;
        bool fullscreen = false;
    };

    struct AppConfig
    {
        std::string title = "Chromo";
        f32 tickrate = 60.0f;
        Vec4 clearColor = Vec4(0,0,0,1);
        WindowConfig window = WindowConfig();
        Vec2i screenSize = Vec2i(1280,720);
        AppFlags flags = AppFlags_None;
        Application* app = NULL;
    };

    class Application
    {
    public:
        virtual void OnInit() {}
        virtual void OnQuit() {}
        virtual void OnUpdate(f32 dt) {}
        virtual void OnRender(f32 dt) {}
        virtual ~Application() {}

        bool m_running = false;
    };
}

// Predeclare the csMain function so we can call it from cs_platform.
extern cs::AppConfig csMain(int argc, char** argv);
