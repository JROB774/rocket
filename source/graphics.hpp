DECLARE_PRIVATE_STRUCT(VertexBuffer);
DECLARE_PRIVATE_STRUCT(Shader);
DECLARE_PRIVATE_STRUCT(Texture);
DECLARE_PRIVATE_STRUCT(Framebuffer);

enum DrawMode
{
    DrawMode_Points,
    DrawMode_LineStrip,
    DrawMode_LineLoop,
    DrawMode_Lines,
    DrawMode_TriangleStrip,
    DrawMode_TriangleFan,
    DrawMode_Triangles
};

// Different scale modes that change how the screen is drawn to the window.
enum ScaleMode
{
    ScaleMode_None,      // Screen is drawn to the window at its original size (no scaling).
    ScaleMode_Fit,       // Screen matches the window size exactly (no scaling + modifies actual screen size).
    ScaleMode_Stretch,   // Screen stretches to fit the entire window (can cause distortion).
    ScaleMode_Letterbox, // Screen scales to fit the window whilst maintaining original aspect ratio.
    ScaleMode_Pixel,     // Screen scales to fti the window at fixed increments (1:1 aspect pixels).
    ScaleMode_TOTAL
};

enum Filter
{
    Filter_Nearest,
    Filter_Linear,
    Filter_TOTAL
};

enum Wrap
{
    Wrap_Repeat,
    Wrap_Clamp,
    Wrap_TOTAL
};

enum AttribType
{
    AttribType_SignedByte,
    AttribType_UnsignedByte,
    AttribType_SignedInt,
    AttribType_UnsignedInt,
    AttribType_Float
};

enum BufferType
{
    BufferType_Static,
    BufferType_Dynamic,
    BufferType_Stream
};

static void InitGraphics();
static void QuitGraphics();

static void BeginRenderFrame();
static void EndRenderFrame();

static void Clear(f32 r, f32 g, f32 b, f32 a = 1.0f);
static void Clear(nkVec4 color);

static void SetRenderTarget(Framebuffer target = NULL); // NULL sets the screen as the target.
static void SetViewport(Rect* viewport = NULL); // NULL sets the viewport to the whole target.
static void SetScreenScaleMode(ScaleMode scaleMode);
static void SetScreenFilter(Filter filter);

static Framebuffer GetScreen();
static u32 GetScreenTextureInternal();
static Rect GetScreenBounds();
static f32 GetScreenWidth();
static f32 GetScreenHeight();
static ScaleMode GetScreenScaleMode();
static Filter GetScreenFilter();

static Framebuffer GetRenderTarget();
static Rect GetViewport();

static f32 GetRenderTargetWidth();
static f32 GetRenderTargetHeight();

static void UseShader(std::string shaderName);
static void UseShader(Shader shader);

static void UseTexture(std::string textureName, s32 unit = 0);
static void UseTexture(Texture texture, s32 unit = 0);

// These operate on the currently bound shader.
static void SetShaderBool(std::string name, bool val);
static void SetShaderInt(std::string name, s32 val);
static void SetShaderFloat(std::string name, f32 val);
static void SetShaderVec2(std::string name, nkVec2 vec);
static void SetShaderVec3(std::string name, nkVec3 vec);
static void SetShaderVec4(std::string name, nkVec4 vec);
static void SetShaderMat2(std::string name, nkMat2 mat);
static void SetShaderMat3(std::string name, nkMat3 mat);
static void SetShaderMat4(std::string name, nkMat4 mat);

// Shader
static bool LoadShaderFromFile(Shader& shader, std::string fileName);
static bool LoadShaderFromData(Shader& shader, void* data, size_t bytes);
static void FreeShader(Shader& shader);

// Texture
static bool CreateTexture(Texture& texture, s32 w, s32 h, s32 bpp, void* data, Filter filter = Filter_Linear, Wrap wrap = Wrap_Clamp); // Expects RGBA order.
static bool LoadTextureFromFile(Texture& texture, std::string fileName, Filter filter = Filter_Linear, Wrap wrap = Wrap_Clamp);
static bool LoadTextureFromData(Texture& texture, void* data, size_t bytes, Filter filter = Filter_Linear, Wrap wrap = Wrap_Clamp);
static void FreeTexture(Texture& texture);
static f32 GetTextureWidth(Texture& texture);
static f32 GetTextureHeight(Texture& texture);
static void SetTextureFilter(Texture& texture, Filter filter);
static void SetTextureWrap(Texture& texture, Wrap wrap);

// VertexBuffer
static void CreateVertexBuffer(VertexBuffer& buffer);
static void FreeVertexBuffer(VertexBuffer& buffer);
static void SetVertexBufferStride(VertexBuffer& buffer, size_t byteStride);
static void EnableVertexBufferAttrib(VertexBuffer& buffer, u32 index, AttribType type, u32 components, size_t byteOffset);
static void DisableVertexBufferAttrib(VertexBuffer& buffer, u32 index);
static void UpdateVertexBuffer(VertexBuffer& buffer, void* data, size_t bytes, BufferType type);
static void DrawVertexBuffer(VertexBuffer& buffer, DrawMode drawMode, size_t vertexCount);

// Framebuffer
static void CreateFramebuffer(Framebuffer& framebuffer, s32 w, s32 h, Filter filter = Filter_Linear, Wrap wrap = Wrap_Clamp, nkVec4 clear = { 0,0,0,1 });
static void FreeFramebuffer(Framebuffer& framebuffer);
static void ResizeFramebuffer(Framebuffer& framebuffer, s32 w, s32 h, Filter filter = Filter_Linear, Wrap wrap = Wrap_Clamp);
static Texture GetFramebufferTexture(Framebuffer& framebuffer);

DECLARE_ASSET(Shader)
{
public:
    Shader m_data;

    bool        LoadFromFile(std::string fileName) override { return LoadShaderFromFile(m_data, fileName); }
    bool        LoadFromData(void* data, size_t bytes) override { return LoadShaderFromData(m_data, data, bytes); }
    void        Free() override { FreeShader(m_data); }
    #ifndef __EMSCRIPTEN__
    const char* GetPath() const override { return "shaders/gl_330/"; }
    #else
    const char* GetPath() const override { return "shaders/es_300/"; }
    #endif
    const char* GetExt() const override { return ".shader"; }
    const char* GetType() const override { return "Shader"; }
};

DECLARE_ASSET(Texture)
{
public:
    Texture m_data;

    bool        LoadFromFile(std::string fileName) override { return LoadTextureFromFile(m_data, fileName); }
    bool        LoadFromData(void* data, size_t bytes) override { return LoadTextureFromData(m_data, data, bytes); }
    void        Free() override { FreeTexture(m_data); }
    const char* GetPath() const override { return "textures/"; }
    const char* GetExt() const override { return ".png"; }
    const char* GetType() const override { return "Texture"; }
};

namespace imm
{
    struct Vertex
    {
        nkVec4 position;
        nkVec4 color;
        nkVec2 texCoord;
    };

    enum Flip
    {
        Flip_None       = 0,
        Flip_Horizontal = 1 << 0,
        Flip_Vertical   = 1 << 1,
        Flip_Both       = Flip_Horizontal|Flip_Vertical
    };

    static void CreateContext();
    static void FreeContext();

    static void DrawPoint(f32 x, f32 y, nkVec4 color);
    static void DrawLine(f32 x1, f32 y1, f32 x2, f32 y2, nkVec4 color);
    static void DrawRectOutline(f32 x1, f32 y1, f32 x2, f32 y2, nkVec4 color);
    static void DrawRectFilled(f32 x1, f32 y1, f32 x2, f32 y2, nkVec4 color);
    static void DrawCircleOutline(f32 x, f32 y, f32 r, nkVec4 color, s32 segments = 64);
    static void DrawCircleFilled(f32 x, f32 y, f32 r, nkVec4 color, s32 segments = 64);
    static void DrawTexture(std::string textureName, f32 x, f32 y, const Rect* clip = NULL, nkVec4 color = { 1,1,1,1 });
    static void DrawTexture(std::string textureName, f32 x, f32 y, f32 sx, f32 sy, f32 angle, Flip flip, const nkVec2* anchor = NULL, const Rect* clip = NULL, nkVec4 color = { 1,1,1,1 });
    static void DrawTexture(Texture& texture, f32 x, f32 y, const Rect* clip = NULL, nkVec4 color = { 1,1,1,1 });
    static void DrawTexture(Texture& texture, f32 x, f32 y, f32 sx, f32 sy, f32 angle, Flip flip, const nkVec2* anchor = NULL, const Rect* clip = NULL, nkVec4 color = { 1,1,1,1 });
    static void DrawFramebuffer(Framebuffer& framebuffer, f32 dstX0, f32 dstY0, f32 dstX1, f32 dstY1);

    static void BeginDraw(DrawMode drawMode);
    static void EndDraw();
    static void PutVertex(Vertex v);

    static void BeginTextureBatch(std::string textureName);
    static void BeginTextureBatch(Texture& texture);
    static void EndTextureBatch();
    static void DrawBatchedTexture(f32 x, f32 y, const Rect* clip = NULL, nkVec4 color = { 1,1,1,1 });
    static void DrawBatchedTexture(f32 x, f32 y, f32 sx, f32 sy, f32 angle, Flip flip, const nkVec2* anchor = NULL, const Rect* clip = NULL, nkVec4 color = { 1,1,1,1 });

    static void EnableAlphaBlending(bool enable);
    static void EnableTextureMapping(bool enable);

    static bool IsAlphaBlendingEnabled();
    static bool IsTextureMappingEnabled();

    static void SetCurrentShader(std::string shaderName);
    static void SetCurrentShader(Shader shader);
    static void SetCurrentTexture(std::string textureName, s32 unit = 0);
    static void SetCurrentTexture(Texture texture, s32 unit = 0);

    static nkMat4& GetProjectionMatrix();
    static nkMat4& GetViewMatrix();
    static nkMat4& GetModelMatrix();
}
