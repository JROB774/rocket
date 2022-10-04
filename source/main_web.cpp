void main_loop()
{
    static u64 perfFrequency = SDL_GetPerformanceFrequency();
    static u64 lastCounter = SDL_GetPerformanceCounter();
    static u64 endCounter = 0;
    static u64 elapsedCounter = 0;
    static f32 updateTimer = 0.0f;

    static f32 deltaTime = 1.0f / s_appConfig.tickrate; // We use a fixed update rate to keep things deterministic.

    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
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

    f32 elapsedTime = NK_CAST(f32,elapsedCounter) / NK_CAST(f32,perfFrequency);

    updateTimer += elapsedTime;

    #ifdef BUILD_DEBUG
    f32 currentFPS = NK_CAST(f32,perfFrequency) / NK_CAST(f32,elapsedCounter);
    std::string title = s_appConfig.title + " (FPS: " + std::to_string(currentFPS) + ")";
    SDL_SetWindowTitle(s_context.window, title.c_str());
    #endif // BUILD_DEBUG
}

void actual_main_start()
{
    s_appConfig = AppMain(0, NULL);
    ASSERT(s_appConfig.app, "Need to define an application for the engine to run!");

    // Cache useful paths.
    s_context.execPath = ValidatePath(SDL_GetBasePath());

    printf("Starting Application %s...\n", s_appConfig.title.c_str());

    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_EVENTS|SDL_INIT_GAMECONTROLLER) < 0)
        FatalError("Failed to initialize SDL2!\n");

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

    s32 windowX = SDL_WINDOWPOS_CENTERED;
    s32 windowY = SDL_WINDOWPOS_CENTERED;
    s32 windowW = s_appConfig.window.size.x;
    s32 windowH = s_appConfig.window.size.y;

    s_context.window = SDL_CreateWindow(s_appConfig.title.c_str(), windowX,windowY,windowW,windowH, SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL);
    if(!s_context.window)
        FatalError("Failed to create window!\n");
    SDL_SetWindowMinimumSize(s_context.window, s_appConfig.window.min.x, s_appConfig.window.min.y);

    s_context.glContext = SDL_GL_CreateContext(s_context.window);
    if(!s_context.glContext)
        FatalError("Failed to create OpenGL context!\n");

    InitAssetManager();
    InitGraphics();
    InitAudio();

    SetSoundVolume(0.5f);
    SetMusicVolume(0.5f);

    s_appConfig.app->OnInit();
    s_appConfig.app->m_running = true;

    LockMouse(true);

    emscripten_set_main_loop(main_loop, -1, 1);
}

extern "C" void main_start()
{
    actual_main_start();
}

int main(int argc, char** argv)
{
    EM_ASM
    (
        FS.mkdir("/ROCKET");
        FS.mount(IDBFS, {}, "/ROCKET");
        FS.syncfs(true, function(err)
        {
            assert(!err);
            ccall("main_start");
        });
    );

    return 0;
}
