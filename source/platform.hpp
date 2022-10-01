struct InputState
{
    bool   hasGamepad;
    nkVec2 mouseWheel;
    nkVec2 mousePos;
    nkVec2 relativeMousePos;
    bool   previousKeyState[KeyCode_TOTAL];
    bool   currentKeyState[KeyCode_TOTAL];
    bool   previousMouseButtonState[MouseButton_TOTAL];
    bool   currentMouseButtonState[MouseButton_TOTAL];
    bool   previousButtonState[GamepadButton_TOTAL];
    bool   currentButtonState[GamepadButton_TOTAL];
    s16    previousAxisState[GamepadAxis_TOTAL];
    s16    currentAxisState[GamepadAxis_TOTAL];
};

static const AppConfig& GetAppConfig();

static std::string GetExecPath();

static void FatalError(const char* format, ...);

static void PositionWindow(s32 x, s32 y);
static void ResizeWindow(s32 w, s32 h);

static void MaximizeWindow(bool enable);
static bool IsMaximized();
static void FullscreenWindow(bool enable);
static bool IsFullscreen();

static void EnableVSync(bool enable);
static bool IsVSyncOn();

static s32 GetWindowX();
static s32 GetWindowY();
static s32 GetWindowWidth();
static s32 GetWindowHeight();

static void ShowCursor(bool show);
static bool IsCursorVisible();

static void LockMouse(bool lock);
static bool IsMouseLocked();

// File System
static bool            DoesFileExist(std::string fileName);
static bool            DoesPathExist(std::string pathName);
static bool            IsFile(std::string fileName);
static bool            IsPath(std::string pathName);
static std::string     ValidatePath(std::string pathName);
static bool            CreatePath(std::string pathName);
static size_t          GetSizeOfFile(std::string fileName);
static std::string     StripFileExtension(std::string fileName);
static std::string     StripFilePath(std::string fileName);
static std::string     StripFileExtensionAndPath(std::string fileName);
static std::string     StripFileName(std::string fileName);
static std::string     GetFileExtension(std::string fileName);
static std::string     GetFilePath(std::string fileName);
static std::string     ReadEntireFile(std::string fileName);
static void            WriteEntireFile(std::string fileName, std::string content);
static std::vector<u8> ReadBinaryFile(std::string fileName);
static void            WriteBinaryFile(std::string fileName, void* data, size_t size);
static void            ListPathFiles(std::string pathName, std::vector<std::string>& files, bool recursive);

// Returns a structure with all the raw information about the input for this frame. It is best to not
// use this function directly and instead use the wrapper functions in cs_input which provide a better
// interface for querying specific input state information and conditions.
static const InputState& GetInput();
