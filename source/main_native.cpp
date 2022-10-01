int main(int argc, char** argv)
{
    DEFER { CheckTrackedMemory(); };

    s_appConfig = AppMain(argc, argv);
    ASSERT(s_appConfig.app, "Need to define an application for the engine to run!");
    DEFER { Deallocate(s_appConfig.app); };

    // Cache useful paths.
    s_context.execPath = ValidatePath(SDL_GetBasePath());

    printf("Starting Application %s...\n", s_appConfig.title.c_str());

    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
        FatalError("Failed to initialize SDL2!\n");
    DEFER { SDL_Quit(); };

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    s32 windowX = SDL_WINDOWPOS_CENTERED;
    s32 windowY = SDL_WINDOWPOS_CENTERED;
    s32 windowW = s_appConfig.window.size.x;
    s32 windowH = s_appConfig.window.size.y;
    bool windowMaximized = s_appConfig.window.maximized;
    bool windowFullscreen = s_appConfig.window.fullscreen;
    s32 windowDisplay = 0;

    // @Incomplete: Reimplement window state loading!
    /*
    // If we have the previous session's window state stored then restore the window.
    json& engineState = GetState();
    if(engineState.contains("window"))
    {
        json windowState = engineState["window"];
        if(windowState.contains("bounds"))
        {
            windowX = GetJSONValueWithDefault<s32>(windowState["bounds"], "x", windowX);
            windowY = GetJSONValueWithDefault<s32>(windowState["bounds"], "y", windowY);
            windowW = GetJSONValueWithDefault<s32>(windowState["bounds"], "w", windowW);
            windowH = GetJSONValueWithDefault<s32>(windowState["bounds"], "h", windowH);
        }
        windowMaximized = GetJSONValueWithDefault<bool>(windowState, "maximized", windowMaximized);
        windowFullscreen = GetJSONValueWithDefault<bool>(windowState, "fullscreen", windowFullscreen);
        windowDisplay = GetJSONValueWithDefault<s32>(windowState, "display", windowDisplay);

        SDL_Rect displayBounds;
        if(SDL_GetDisplayBounds(windowDisplay, &displayBounds) < 0)
        {
            printf("Previous display no longer available, aborting window restore!\n");
        }
        else
        {
            // Make sure the window is not out of bounds at all.
            if(windowX != SDL_WINDOWPOS_CENTERED && windowX < displayBounds.x) windowX = displayBounds.x;
            if(windowX != SDL_WINDOWPOS_CENTERED && (windowX+windowW) >= displayBounds.w) windowX = displayBounds.w - windowW;
            if(windowY != SDL_WINDOWPOS_CENTERED && windowY < displayBounds.y) windowY = displayBounds.y;
            if(windowY != SDL_WINDOWPOS_CENTERED && (windowY+windowH) >= displayBounds.h) windowY = displayBounds.h - windowH;
        }
    }
    */

    s_context.window = SDL_CreateWindow(s_appConfig.title.c_str(), windowX,windowY,windowW,windowH, SDL_WINDOW_HIDDEN|SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL);
    if(!s_context.window)
        FatalError("Failed to create window!\n");
    DEFER { SDL_DestroyWindow(s_context.window); };
    SDL_SetWindowMinimumSize(s_context.window, s_appConfig.window.min.x, s_appConfig.window.min.y);

    s_context.glContext = SDL_GL_CreateContext(s_context.window);
    if(!s_context.glContext)
        FatalError("Failed to create OpenGL context!\n");
    DEFER { SDL_GL_DeleteContext(s_context.glContext); };

    glewInit();

    MaximizeWindow(windowMaximized);
    FullscreenWindow(windowFullscreen);

    InitAssetManager();
    DEFER { QuitAssetManager(); };

    InitGraphics();
    DEFER { QuitGraphics(); };

    InitAudio();
    DEFER { QuitAudio(); };

    s_appConfig.app->OnInit();
    s_appConfig.app->m_running = true;

    u64 perfFrequency = SDL_GetPerformanceFrequency();
    u64 lastCounter = SDL_GetPerformanceCounter();
    u64 endCounter = 0;
    u64 elapsedCounter = 0;
    f32 updateTimer = 0.0f;

    f32 deltaTime = 1.0f / s_appConfig.tickrate; // We use a fixed update rate to keep things deterministic.

    // Enable VSync by default, if we don't get it then oh well.
    if(SDL_GL_SetSwapInterval(1) == 0)
    {
        printf("VSync Enabled!");
    }

    while(s_appConfig.app->m_running)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case(SDL_KEYDOWN):
                {
                    switch(event.key.keysym.sym)
                    {
                        case(SDLK_RETURN): if(!(SDL_GetModState()&KMOD_ALT)) break;
                        case(SDLK_F11): FullscreenWindow(!IsFullscreen()); break;
                    }
                } break;
                case(SDL_QUIT):
                {
                    s_appConfig.app->m_running = false;
                } break;
                case(SDL_WINDOWEVENT):
                {
                    if(event.window.windowID == SDL_GetWindowID(s_context.window))
                    {
                        switch(event.window.event)
                        {
                            case(SDL_WINDOWEVENT_MAXIMIZED): s_context.maximized = true; break;
                            case(SDL_WINDOWEVENT_RESTORED): s_context.maximized = false; break;
                        }
                    }
                } break;
                // Input events.
                case(SDL_CONTROLLERDEVICEADDED): AddGamepad(); break;
                case(SDL_CONTROLLERDEVICEREMOVED): RemoveGamepad(); break;
                case(SDL_MOUSEWHEEL):
                {
                    s_context.input.mouseWheel.x = event.wheel.x;
                    s_context.input.mouseWheel.y = event.wheel.y;
                } break;
            }
        }

        bool didUpdate = false;
        while(updateTimer >= deltaTime)
        {
            UpdateInputState();
            s_appConfig.app->OnUpdate(deltaTime);
            updateTimer -= deltaTime;
            didUpdate = true;
        }
        if(didUpdate)
        {
            SetViewport(NULL);
            Clear(s_appConfig.clearColor);
            BeginRenderFrame();
            s_appConfig.app->OnRender(deltaTime);
        }
        EndRenderFrame();

        SDL_GL_SwapWindow(s_context.window);

        endCounter = SDL_GetPerformanceCounter();
        elapsedCounter = endCounter - lastCounter;
        lastCounter = SDL_GetPerformanceCounter();

        f32 elapsedTime = CAST(f32,elapsedCounter) / CAST(f32,perfFrequency);

        updateTimer += elapsedTime;

        if(BUILD_DEBUG)
        {
            f32 currentFPS = CAST(f32,perfFrequency) / CAST(f32,elapsedCounter);
            std::string title = s_appConfig.title + " (FPS: " + std::to_string(currentFPS) + ")";
            SDL_SetWindowTitle(s_context.window, title.c_str());
        }

        // The window starts out hidden, after the first draw we unhide the window as this looks quite clean.
        if(CHECK_FLAGS(SDL_GetWindowFlags(s_context.window), SDL_WINDOW_HIDDEN))
        {
            SDL_ShowWindow(s_context.window);
        }
    }

    s_appConfig.app->OnQuit();

    // Save the window state so it can be restored for future sessions.
    if(s_context.window)
    {
        SDL_RestoreWindow(s_context.window);
        if(!IsFullscreen())
        {
            SDL_GetWindowPosition(s_context.window, &windowX,&windowY);
            SDL_GetWindowSize(s_context.window, &windowW,&windowH);
        }
        else
        {
            // Use the cached bounds as they represent the actual window pos and size.
            windowX = s_context.cachedWindowXPos;
            windowY = s_context.cachedWindowYPos;
            windowW = s_context.cachedWindowWidth;
            windowH = s_context.cachedWindowHeight;
        }

        windowMaximized = s_context.maximized;
        windowFullscreen = s_context.fullscreen;

        windowDisplay = SDL_GetWindowDisplayIndex(s_context.window);

        // @Incomplete: Reimplement window state saving!
        /*
        engineState["window"] = json::object();
        json& windowState = engineState["window"];

        windowState["bounds"]["x"] = windowX;
        windowState["bounds"]["y"] = windowY;
        windowState["bounds"]["w"] = windowW;
        windowState["bounds"]["h"] = windowH;
        windowState["maximized"] = windowMaximized;
        windowState["fullscreen"] = windowFullscreen;
        windowState["display"] = windowDisplay;
        */
    }

    return 0;
}