int main(int argc, char** argv)
{
    NK_DEFER(CheckTrackedMemory());

    s_appConfig = AppMain(argc, argv);
    ASSERT(s_appConfig.app, "Need to define an application for the engine to run!");
    NK_DEFER(Deallocate(s_appConfig.app));

    // Cache useful paths.
    s_context.execPath = ValidatePath(SDL_GetBasePath());

    printf("Starting Application %s...\n", s_appConfig.title.c_str());

    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
        FatalError("Failed to initialize SDL2!\n");
    NK_DEFER(SDL_Quit());

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    s32 windowX = SDL_WINDOWPOS_CENTERED;
    s32 windowY = SDL_WINDOWPOS_CENTERED;
    s32 windowW = NK_CAST(s32, s_appConfig.window.size.x);
    s32 windowH = NK_CAST(s32, s_appConfig.window.size.y);
    bool windowMaximized = s_appConfig.window.maximized;
    bool windowFullscreen = s_appConfig.window.fullscreen;
    s32 windowDisplay = 0;
    f32 soundVolume = k_defaultSoundVolume;
    f32 musicVolume = k_defaultMusicVolume;

    // If we have the previous session's window state stored then restore the window.
    GonObject engineState;
    printf("Looking for previous engine state...\n");
    std::string stateFileName = GetExecPath() + "state.dat";
    if(!DoesFileExist(stateFileName))
        printf("No previous engine state!\n");
    else
    {
        printf("Previous engine state found!\n");
        engineState = GonObject::Load(stateFileName);
    }

    windowX = engineState["bounds_x"].Int(windowX);
    windowY = engineState["bounds_y"].Int(windowY);
    windowW = engineState["bounds_w"].Int(windowW);
    windowH = engineState["bounds_h"].Int(windowH);
    windowMaximized = engineState["maximized"].Bool(windowMaximized);
    windowFullscreen = engineState["fullscreen"].Bool(windowFullscreen);
    windowDisplay = engineState["display"].Int(windowDisplay);
    soundVolume = NK_CAST(f32, engineState["sound_volume"].Number(soundVolume));
    musicVolume = NK_CAST(f32, engineState["music_volume"].Number(musicVolume));

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

    s_context.window = SDL_CreateWindow(s_appConfig.title.c_str(), windowX,windowY,windowW,windowH, SDL_WINDOW_HIDDEN|SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL);
    if(!s_context.window)
        FatalError("Failed to create window!\n");
    NK_DEFER(SDL_DestroyWindow(s_context.window));
    SDL_SetWindowMinimumSize(s_context.window, NK_CAST(s32,s_appConfig.window.min.x), NK_CAST(s32,s_appConfig.window.min.y));

    s_context.glContext = SDL_GL_CreateContext(s_context.window);
    if(!s_context.glContext)
        FatalError("Failed to create OpenGL context!\n");
    NK_DEFER(SDL_GL_DeleteContext(s_context.glContext));

    glewInit();

    MaximizeWindow(windowMaximized);
    FullscreenWindow(windowFullscreen);

    InitAssetManager();
    NK_DEFER(QuitAssetManager());

    InitGraphics();
    NK_DEFER(QuitGraphics());

    InitAudio();
    NK_DEFER(QuitAudio());

    SetSoundVolume(soundVolume);
    SetMusicVolume(musicVolume);

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
        printf("VSync Enabled!\n");
    }

    while(s_appConfig.app->m_running)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_KEYDOWN:
                {
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_RETURN: if(!(SDL_GetModState()&KMOD_ALT)) break;
                        case SDLK_F11:
                        {
                            FullscreenWindow(!IsFullscreen());
                            ResetCursor();
                        } break;
                    }
                } break;
                case SDL_QUIT:
                {
                    s_appConfig.app->m_running = false;
                } break;
                case SDL_WINDOWEVENT:
                {
                    if(event.window.windowID == SDL_GetWindowID(s_context.window))
                    {
                        switch(event.window.event)
                        {
                            case SDL_WINDOWEVENT_MAXIMIZED: s_context.maximized = true; break;
                            case SDL_WINDOWEVENT_RESTORED: s_context.maximized = false; break;
                        }
                    }
                } break;
                // Input events.
                case SDL_CONTROLLERDEVICEADDED: AddGamepad(); break;
                case SDL_CONTROLLERDEVICEREMOVED: RemoveGamepad(); break;
                case SDL_MOUSEWHEEL:
                {
                    s_context.input.mouseWheel.x = NK_CAST(f32,event.wheel.x);
                    s_context.input.mouseWheel.y = NK_CAST(f32,event.wheel.y);
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

        f32 elapsedTime = NK_CAST(f32,elapsedCounter) / NK_CAST(f32,perfFrequency);

        updateTimer += elapsedTime;

        #ifdef BUILD_DEBUG
        f32 currentFPS = NK_CAST(f32,perfFrequency) / NK_CAST(f32,elapsedCounter);
        std::string title = s_appConfig.title + " (FPS: " + std::to_string(currentFPS) + ")";
        SDL_SetWindowTitle(s_context.window, title.c_str());
        #endif // BUILD_DEBUG

        // The window starts out hidden, after the first draw we unhide the window as this looks quite clean.
        if(NK_CHECK_FLAGS(SDL_GetWindowFlags(s_context.window), SDL_WINDOW_HIDDEN))
        {
            SDL_ShowWindow(s_context.window);
        }
    }

    s_appConfig.app->OnQuit();

    // Save the engine state so it can be restored for future sessions.
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

        soundVolume = GetSoundVolume();
        musicVolume = GetMusicVolume();

        FILE* state = fopen(stateFileName.c_str(), "w");
        if(!state)
            printf("Failed to write out the engine state!\n");
        else
        {
            fprintf(state, "bounds_x %d\n", windowX);
            fprintf(state, "bounds_y %d\n", windowY);
            fprintf(state, "bounds_w %d\n", windowW);
            fprintf(state, "bounds_h %d\n", windowH);
            fprintf(state, "maximized %s\n", (windowMaximized) ? "true" : "false");
            fprintf(state, "fullscreen %s\n", (windowFullscreen) ? "true" : "false");
            fprintf(state, "display %d\n", windowDisplay);
            fprintf(state, "sound_volume %f\n", soundVolume);
            fprintf(state, "music_volume %f\n", musicVolume);
            fclose(state);
        }
    }

    return 0;
}
