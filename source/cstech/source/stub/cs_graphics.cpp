#include "cs_graphics.hpp"

using namespace cs;
using namespace gfx;
using namespace imm;

CS_PRIVATE_SCOPE
{
    // We don't actually use these but we need them so that the imm::GetXXXMatrix functions have something to return.
    static Mat4 s_dummyProjectionMatrix;
    static Mat4 s_dummyViewMatrix;
    static Mat4 s_dummyModelMatrix;
}

CS_PUBLIC_SCOPE::gfx
{
    CS_DEFINE_PRIVATE_STRUCT(VertexBuffer) {};
    CS_DEFINE_PRIVATE_STRUCT(Shader) {};
    CS_DEFINE_PRIVATE_STRUCT(Texture) {};
    CS_DEFINE_PRIVATE_STRUCT(Framebuffer) {};

    //
    // Shader
    //

    CS_API bool LoadShader(Shader& shader, std::string fileName)
    {
        return true;
    }

    CS_API void FreeShader(Shader& shader)
    {
        // Nothing...
    }

    //
    // Texture
    //

    CS_API bool CreateTexture(Texture& texture, s32 w, s32 h, s32 bpp, void* data, Filter filter, Wrap wrap)
    {
        return true;
    }

    CS_API bool LoadTexture(Texture& texture, std::string fileName, Filter filter, Wrap wrap)
    {
        return true;
    }

    CS_API void FreeTexture(Texture& texture)
    {
        // Nothing...
    }

    CS_API f32 GetTextureWidth(Texture& texture)
    {
        return 0.0f;
    }

    CS_API f32 GetTextureHeight(Texture& texture)
    {
        return 0.0f;
    }

    CS_API void SetTextureFilter(Texture& texture, Filter filter)
    {
        // Nothing...
    }

    CS_API void SetTextureWrap(Texture& texture, Wrap wrap)
    {
        // Nothing...
    }

    //
    // VertexBuffer
    //

    CS_API void CreateVertexBuffer(VertexBuffer& buffer)
    {
        // Nothing...
    }

    CS_API void FreeVertexBuffer(VertexBuffer& buffer)
    {
        // Nothing...
    }

    CS_API void DrawVertexBuffer(VertexBuffer& buffer, DrawMode drawMode)
    {
        // Nothing...
    }

    CS_API void ClearVertexBuffer(VertexBuffer& buffer)
    {
        // Nothing...
    }

    //
    // Framebuffer
    //

    CS_API void CreateFramebuffer(Framebuffer& framebuffer, s32 w, s32 h, Filter filter, Wrap wrap)
    {
        // Nothing...
    }

    CS_API void FreeFramebuffer(Framebuffer& framebuffer)
    {
        // Nothing...
    }

    CS_API void ResizeFramebuffer(Framebuffer& framebuffer, s32 w, s32 h, Filter filter, Wrap wrap)
    {
        // Nothing...
    }

    //
    // Renderer
    //

    CS_API void InitGraphics()
    {
        // Nothing...
    }

    CS_API void QuitGraphics()
    {
        // Nothing...
    }

    CS_API void BeginRenderFrame()
    {
        // Nothing...
    }

    CS_API void EndRenderFrame()
    {
        // Nothing...
    }

    CS_API void Clear(f32 r, f32 g, f32 b, f32 a)
    {
        // Nothing...
    }

    CS_API void Clear(Vec4 color)
    {
        // Nothing...
    }

    CS_API void SetRenderTarget(Framebuffer target)
    {
        // Nothing...
    }

    CS_API void SetViewport(Rect* viewport)
    {
        // Nothing...
    }

    CS_API void SetScreenScaleMode(ScaleMode scaleMode)
    {
        // Nothing...
    }

    CS_API void SetScreenFilter(Filter filter)
    {
        // Nothing...
    }

    CS_API Framebuffer GetScreen()
    {
        return NULL;
    }

    CS_API u32 GetScreenTextureInternal()
    {
        return 0;
    }

    CS_API Rect GetScreenBounds()
    {
        return {}
    }

    CS_API f32 GetScreenWidth()
    {
        return 0.0f;
    }

    CS_API f32 GetScreenHeight()
    {
        return 0.0f;
    }

    CS_API ScaleMode GetScreenScaleMode()
    {
        return ScaleMode_None;
    }

    CS_API Filter GetScreenFilter()
    {
        return Filter_Nearest;
    }

    CS_API Framebuffer GetRenderTarget()
    {
        return NULL;
    }

    CS_API Rect GetViewport()
    {
        return {};
    }

    CS_API f32 GetRenderTargetWidth()
    {
        return 0.0f;
    }

    CS_API f32 GetRenderTargetHeight()
    {
        return 0.0f;
    }

    CS_API void UseShader(std::string shaderName)
    {
        // Nothing...
    }

    CS_API void UseShader(Shader shader)
    {
        // Nothing...
    }

    CS_API void UseTexture(std::string textureName)
    {
        // Nothing...
    }

    CS_API void UseTexture(Texture texture)
    {
        // Nothing...
    }

    // These operate on the currently bound shader.
    CS_API void SetShaderBool(std::string name, bool val)
    {
        // Nothing...
    }
    CS_API void SetShaderInt(std::string name, s32 val)
    {
        // Nothing...
    }
    CS_API void SetShaderFloat(std::string name, f32 val)
    {
        // Nothing...
    }
    CS_API void SetShaderVec2(std::string name, Vec2 vec)
    {
        // Nothing...
    }
    CS_API void SetShaderVec3(std::string name, Vec3 vec)
    {
        // Nothing...
    }
    CS_API void SetShaderVec4(std::string name, Vec4 vec)
    {
        // Nothing...
    }
    CS_API void SetShaderMat2(std::string name, Mat2 mat)
    {
        // Nothing...
    }
    CS_API void SetShaderMat3(std::string name, Mat3 mat)
    {
        // Nothing...
    }
    CS_API void SetShaderMat4(std::string name, Mat4 mat)
    {
        // Nothing...
    }
}

CS_PUBLIC_SCOPE
{
    bool Asset<gfx::Shader>::Load(std::string fileName)
    {
        return true;
    }
    void Asset<gfx::Shader>::Free()
    {
        // Nothing...
    }
    void Asset<gfx::Shader>::DoDebugView()
    {
        // Nothing...
    }
    const char* Asset<gfx::Shader>::GetPath() const
    {
        return "shaders/";
    }
    const char* Asset<gfx::Shader>::GetExt() const
    {
        return ".shader";
    }
    const char* Asset<gfx::Shader>::GetType() const
    {
        return "Shader";
    }

    bool Asset<gfx::Texture>::Load(std::string fileName)
    {
        return true;
    }
    void Asset<gfx::Texture>::Free()
    {
        // Nothing...
    }
    void Asset<gfx::Texture>::DoDebugView()
    {
        // Nothing...
    }
    const char* Asset<gfx::Texture>::GetPath() const
    {
        return "textures/";
    }
    csont char* Asset<gfx::Texture>::GetExt() const
    {
        return ".json";
    }
    const char* Asset<gfx::Texture>::GetType() const
    {
        return "Texture";
    }
}

CS_PUBLIC_SCOPE::imm
{
    CS_API void CreateContext()
    {
        // Nothing...
    }

    CS_API void FreeContext()
    {
        // Nothing...
    }

    CS_API void DrawPoint(f32 x, f32 y, Vec4 color)
    {
        // Nothing...
    }

    CS_API void DrawLine(f32 x1, f32 y1, f32 x2, f32 y2, Vec4 color)
    {
        // Nothing...
    }

    CS_API void DrawRectOutline(f32 x1, f32 y1, f32 x2, f32 y2, Vec4 color)
    {
        // Nothing...
    }

    CS_API void DrawRectFilled(f32 x1, f32 y1, f32 x2, f32 y2, Vec4 color)
    {
        // Nothing...
    }

    CS_API void DrawCircleOutline(f32 x, f32 y, f32 r, Vec4 color, s32 segments)
    {
        // Nothing...
    }

    CS_API void DrawCircleFilled(f32 x, f32 y, f32 r, Vec4 color, s32 segments)
    {
        // Nothing...
    }

    CS_API void DrawTexture(std::string textureName, f32 x, f32 y, const Rect* clip, Vec4 color)
    {
        // Nothing...
    }

    CS_API void DrawTexture(std::string textureName, f32 x, f32 y, f32 sx, f32 sy, f32 angle, Flip flip, const Vec2* anchor, const Rect* clip, Vec4 color)
    {
        // Nothing...
    }

    CS_API void DrawTexture(Texture& texture, f32 x, f32 y, const Rect* clip, Vec4 color)
    {
        // Nothing...
    }

    CS_API void DrawTexture(Texture& texture, f32 x, f32 y, f32 sx, f32 sy, f32 angle, Flip flip, const Vec2* anchor, const Rect* clip, Vec4 color)
    {
        // Nothing...
    }

    CS_API void BeginDraw(DrawMode drawMode)
    {
        // Nothing...
    }

    CS_API void EndDraw()
    {
        // Nothing...
    }

    CS_API void PutVertex(Vertex v)
    {
        // Nothing...
    }

    CS_API void EnableAlphaBlending(bool enable)
    {
        // Nothing...
    }

    CS_API void EnableTextureMapping(bool enable)
    {
        // Nothing...
    }

    CS_API bool IsAlphaBlendingEnabled()
    {
        return false;
    }

    CS_API bool IsTextureMappingEnabled()
    {
        return false;
    }

    CS_API void SetCurrentShader(std::string shaderName)
    {
        // Nothing...
    }

    CS_API void SetCurrentShader(Shader shader)
    {
        // Nothing...
    }

    CS_API void SetCurrentTexture(std::string textureName)
    {
        // Nothing...
    }

    CS_API void SetCurrentTexture(Texture texture)
    {
        // Nothing...
    }

    CS_API Mat4& GetProjectionMatrix()
    {
        return s_dummyProjectionMatrix;
    }

    CS_API Mat4& GetViewMatrix()
    {
        return s_dummyViewMatrix;
    }

    CS_API Mat4& GetModelMatrix()
    {
        return s_dummyModelMatrix;
    }
}
