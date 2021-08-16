#pragma once

#include "cs_define.hpp"
#include "cs_math.hpp"
#include "cs_assets.hpp"

#include <string>

// @NOTE: We're not really doing this properly right now, we will implement a better renderer in the future.
// Right now we are just emulating our old SDL renderer and getting stuff on the screen. As we figure out
// what sort of rendering we are going to want to do, we will come and implement a better rendering system.

CS_PUBLIC_SCOPE::gfx
{
    CS_DECLARE_PRIVATE_STRUCT(VertexBuffer);
    CS_DECLARE_PRIVATE_STRUCT(Shader);
    CS_DECLARE_PRIVATE_STRUCT(Texture);
    CS_DECLARE_PRIVATE_STRUCT(Framebuffer);

    struct Vertex
    {
        Vec2 position;
        Vec4 color;
        Vec2 texCoord;
    };

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

    CS_API void InitGraphics();
    CS_API void QuitGraphics();

    CS_API void BeginRenderFrame();
    CS_API void EndRenderFrame();

    CS_API void Clear(f32 r, f32 g, f32 b, f32 a = 1.0f);
    CS_API void Clear(Vec4 color);

    CS_API void SetRenderTarget(Framebuffer target = NULL); // NULL sets the screen as the target.
    CS_API void SetViewport(Rect* viewport = NULL); // NULL sets the viewport to the whole target.
    CS_API void SetScreenScaleMode(ScaleMode scaleMode);
    CS_API void SetScreenFilter(Filter filter);

    CS_API Framebuffer GetScreen();
    CS_API u32 GetScreenTextureInternal();
    CS_API Rect GetScreenBounds();
    CS_API f32 GetScreenWidth();
    CS_API f32 GetScreenHeight();
    CS_API ScaleMode GetScreenScaleMode();
    CS_API Filter GetScreenFilter();

    CS_API Framebuffer GetRenderTarget();
    CS_API Rect GetViewport();

    CS_API f32 GetRenderTargetWidth();
    CS_API f32 GetRenderTargetHeight();

    CS_API void UseShader(std::string shaderName);
    CS_API void UseShader(Shader shader);

    CS_API void UseTexture(std::string textureName, s32 unit = 0);
    CS_API void UseTexture(Texture texture, s32 unit = 0);

    // These operate on the currently bound shader.
    CS_API void SetShaderBool(std::string name, bool val);
    CS_API void SetShaderInt(std::string name, s32 val);
    CS_API void SetShaderFloat(std::string name, f32 val);
    CS_API void SetShaderVec2(std::string name, Vec2 vec);
    CS_API void SetShaderVec3(std::string name, Vec3 vec);
    CS_API void SetShaderVec4(std::string name, Vec4 vec);
    CS_API void SetShaderMat2(std::string name, Mat2 mat);
    CS_API void SetShaderMat3(std::string name, Mat3 mat);
    CS_API void SetShaderMat4(std::string name, Mat4 mat);

    // Shader
    CS_API bool LoadShader(Shader& shader, std::string fileName);
    CS_API void FreeShader(Shader& shader);

    // Texture
    CS_API bool CreateTexture(Texture& texture, s32 w, s32 h, s32 bpp, void* data, Filter filter = Filter_Linear, Wrap wrap = Wrap_Clamp); // Expects RGBA order.
    CS_API bool LoadTexture(Texture& texture, std::string fileName, Filter filter = Filter_Linear, Wrap wrap = Wrap_Clamp);
    CS_API void FreeTexture(Texture& texture);
    CS_API f32 GetTextureWidth(Texture& texture);
    CS_API f32 GetTextureHeight(Texture& texture);
    CS_API void SetTextureFilter(Texture& texture, Filter filter);
    CS_API void SetTextureWrap(Texture& texture, Wrap wrap);

    // VertexBuffer
    CS_API void CreateVertexBuffer(VertexBuffer& buffer);
    CS_API void FreeVertexBuffer(VertexBuffer& buffer);
    CS_API void DrawVertexBuffer(VertexBuffer& buffer, DrawMode drawMode);
    CS_API void ClearVertexBuffer(VertexBuffer& buffer);

    // Framebuffer
    CS_API void CreateFramebuffer(Framebuffer& framebuffer, s32 w, s32 h, Filter filter = Filter_Linear, Wrap wrap = Wrap_Clamp, Vec4 clear = Vec4(0,0,0,1));
    CS_API void FreeFramebuffer(Framebuffer& framebuffer);
    CS_API void ResizeFramebuffer(Framebuffer& framebuffer, s32 w, s32 h, Filter filter = Filter_Linear, Wrap wrap = Wrap_Clamp);
    CS_API Texture GetFramebufferTexture(Framebuffer& framebuffer);
}

CS_PUBLIC_SCOPE
{
    CS_DECLARE_ASSET(gfx::Shader)
    {
    public:
        gfx::Shader m_data;

        bool        Load(std::string fileName) override;
        void        Free() override;
        void        DoDebugView() override;
        const char* GetPath() const override;
        const char* GetExt() const override;
        const char* GetType() const override;
    };

    CS_DECLARE_ASSET(gfx::Texture)
    {
    public:
        gfx::Texture m_data;

        bool        Load(std::string fileName) override;
        void        Free() override;
        void        DoDebugView() override;
        const char* GetPath() const override;
        const char* GetExt() const override;
        const char* GetType() const override;
    };
}

CS_PUBLIC_SCOPE::imm
{
    enum Flip
    {
        Flip_None       = 0,
        Flip_Horizontal = 1 << 0,
        Flip_Vertical   = 1 << 1,
        Flip_Both       = Flip_Horizontal|Flip_Vertical
    };

    CS_API void CreateContext();
    CS_API void FreeContext();

    CS_API void DrawPoint(f32 x, f32 y, Vec4 color);
    CS_API void DrawLine(f32 x1, f32 y1, f32 x2, f32 y2, Vec4 color);
    CS_API void DrawRectOutline(f32 x1, f32 y1, f32 x2, f32 y2, Vec4 color);
    CS_API void DrawRectFilled(f32 x1, f32 y1, f32 x2, f32 y2, Vec4 color);
    CS_API void DrawCircleOutline(f32 x, f32 y, f32 r, Vec4 color, s32 segments = 64);
    CS_API void DrawCircleFilled(f32 x, f32 y, f32 r, Vec4 color, s32 segments = 64);
    CS_API void DrawTexture(std::string textureName, f32 x, f32 y, const Rect* clip = NULL, Vec4 color = Vec4(1));
    CS_API void DrawTexture(std::string textureName, f32 x, f32 y, f32 sx, f32 sy, f32 angle, Flip flip, const Vec2* anchor = NULL, const Rect* clip = NULL, Vec4 color = Vec4(1));
    CS_API void DrawTexture(gfx::Texture& texture, f32 x, f32 y, const Rect* clip = NULL, Vec4 color = Vec4(1));
    CS_API void DrawTexture(gfx::Texture& texture, f32 x, f32 y, f32 sx, f32 sy, f32 angle, Flip flip, const Vec2* anchor = NULL, const Rect* clip = NULL, Vec4 color = Vec4(1));
    CS_API void DrawFramebuffer(gfx::Framebuffer& framebuffer, f32 x, f32 y);

    CS_API void BeginDraw(gfx::DrawMode drawMode);
    CS_API void EndDraw();
    CS_API void PutVertex(gfx::Vertex v);

    CS_API void EnableAlphaBlending(bool enable);
    CS_API void EnableTextureMapping(bool enable);

    CS_API bool IsAlphaBlendingEnabled();
    CS_API bool IsTextureMappingEnabled();

    CS_API void SetCurrentShader(std::string shaderName);
    CS_API void SetCurrentShader(gfx::Shader shader);
    CS_API void SetCurrentTexture(std::string textureName, s32 unit = 0);
    CS_API void SetCurrentTexture(gfx::Texture texture, s32 unit = 0);

    CS_API Mat4& GetProjectionMatrix();
    CS_API Mat4& GetViewMatrix();
    CS_API Mat4& GetModelMatrix();
}
