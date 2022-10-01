DEFINE_PRIVATE_STRUCT(VertexBuffer)
{
    GLuint vao, vbo;
    std::vector<Vertex> verts;
};

DEFINE_PRIVATE_STRUCT(Shader)
{
    std::string source;
    GLuint program;
};

DEFINE_PRIVATE_STRUCT(Texture)
{
    GLuint handle;
    f32 w, h;
    Filter filter;
    Wrap wrap;
};

DEFINE_PRIVATE_STRUCT(Framebuffer)
{
    GLuint handle = GL_NONE; // Framebuffer
    Texture texture = NULL; // Color Attachment
};

struct Screen
{
    Framebuffer buffer;
    Rect bounds;
    ScaleMode scaleMode;
    Filter filter;
};

struct Renderer
{
    Screen screen;
    Rect viewport;
    Framebuffer boundTarget;
    Shader boundShader;
    Texture boundTexture[64];
};

struct ImmContext
{
    DrawMode drawMode;
    VertexBuffer buffer;
    Shader shader;
    Texture texture[64];
    nkMat4 projectionMatrix;
    nkMat4 viewMatrix;
    nkMat4 modelMatrix;
    bool alphaBlending;
    bool textureMapping;
};

static constexpr f32 k_pi32 = 3.141592653590f;
static constexpr f32 k_tau32 = 6.283185307180f;

static Renderer s_renderer;
static ImmContext s_immContext;

static GLuint CompileShader(std::string& source, GLenum type)
{
    GLuint shader = glCreateShader(type);
    const char* cSource = source.c_str();
    glShaderSource(shader, 1, &cSource, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        std::string infoLog;
        infoLog.resize(infoLogLength);
        glGetShaderInfoLog(shader, infoLogLength, NULL, &infoLog[0]);
        printf("Failed to compile shader:\n%s\n", infoLog.c_str());
    }

    return shader;
}

static bool CreateShader(Shader& shader, std::stringstream& stream)
{
    shader = Allocate<GET_PTR_TYPE(shader)>(MEM_SYSTEM);
    if(!shader) printf("Failed to allocate shader!\n"); // @Incomplete: Fatal error should terminate!

    shader->source = stream.str();

    std::string line;

    std::string vertSource;
    std::string fragSource;

    // By default we put the contents into both shaders. It is only once we reach one of the
    // attribute lines [VertProgram], [FragProgram], etc. that we place lines in a specific shader.
    bool inVertProgram = true;
    bool inFragProgram = true;

    while(std::getline(stream, line))
    {
        // Trim leading whitespace.
        line.erase(0,line.find_first_not_of(" \t\n\r\f\v"));

        // Handle our attributes.
        if     (line == "[VertProgram]") inVertProgram = true, inFragProgram = false;
        else if(line == "[FragProgram]") inVertProgram = false, inFragProgram = true;
        else
        {
            // Add lines to the appropriate shader sources.
            if(inVertProgram) vertSource += line + "\n";
            if(inFragProgram) fragSource += line + "\n";
        }
    }

    GLuint vert = CompileShader(vertSource, GL_VERTEX_SHADER);
    GLuint frag = CompileShader(fragSource, GL_FRAGMENT_SHADER);

    DEFER
    {
        glDeleteShader(vert);
        glDeleteShader(frag);
    };

    shader->program = glCreateProgram();
    glAttachShader(shader->program, vert);
    glAttachShader(shader->program, frag);
    glLinkProgram(shader->program);

    GLint success;
    glGetProgramiv(shader->program, GL_LINK_STATUS, &success);
    if(!success)
    {
        GLint infoLogLength;
        glGetProgramiv(shader->program, GL_INFO_LOG_LENGTH, &infoLogLength);
        std::string infoLog;
        infoLog.resize(infoLogLength);
        glGetProgramInfoLog(shader->program, infoLogLength, NULL, &infoLog[0]);
        printf("Failed to link shader:\n%s\n", infoLog.c_str());
        return false;
    }

    return true;
}

static GLenum BPPToGLFormat(s32 bpp)
{
    switch(bpp)
    {
        case(1): return GL_RED; break;
        case(2): return GL_RG; break;
        case(3): return GL_RGB; break;
        case(4): return GL_RGBA; break;
        default:
        {
            ASSERT(false, "Unsupported BPP that has no appropriate format.");
        } break;
    }
    return GL_NONE;
}

static Filter StringToFilter(const std::string& str)
{
    if(str == "nearest") return Filter_Nearest;
    if(str == "linear") return Filter_Linear;
    ASSERT(false, "No filter found for the specified name.");
    return Filter_TOTAL;
}

static GLenum FilterToGLFilter(Filter filter)
{
    switch(filter)
    {
        case(Filter_Nearest): return GL_NEAREST; break;
        case(Filter_Linear): return GL_LINEAR; break;
        default:
        {
            ASSERT(false, "Unsupported texture filter.");
        } break;
    }
    return GL_NONE;
}

static Wrap StringToWrap(const std::string& str)
{
    if(str == "repeat") return Wrap_Repeat;
    if(str == "clamp") return Wrap_Clamp;
    ASSERT(false, "No wrap found for the specified name.");
    return Wrap_TOTAL;
}

static GLenum WrapToGLWrap(Wrap wrap)
{
    switch(wrap)
    {
        case(Wrap_Repeat): return GL_REPEAT; break;
        case(Wrap_Clamp): return GL_CLAMP_TO_EDGE; break;
        default:
        {
            ASSERT(false, "Unsupported texture wrap.");
        } break;
    }
    return GL_NONE;
}

//
// Shader
//

static bool LoadShader(Shader& shader, std::string fileName)
{
    std::ifstream file(fileName);
    if (!file.is_open())
        printf("Failed to load shader from file '%s'!\n", fileName.c_str());
    else
    {
        std::stringstream stream;
        stream << file.rdbuf();
        return CreateShader(shader, stream);
    }
    return false;
}

static void FreeShader(Shader& shader)
{
    if(!shader) return;
    glDeleteProgram(shader->program);
    Deallocate(shader);
}

//
// Texture
//

static bool CreateTexture(Texture& texture, s32 w, s32 h, s32 bpp, void* data, Filter filter, Wrap wrap)
{
    texture = Allocate<GET_PTR_TYPE(texture)>(MEM_SYSTEM);
    if(!texture) printf("Failed to allocate texture!\n"); // @Incomplete: Fatal error should terminate!

    glActiveTexture(GL_TEXTURE0);

    glGenTextures(1, &texture->handle);
    glBindTexture(GL_TEXTURE_2D, texture->handle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     WrapToGLWrap(wrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     WrapToGLWrap(wrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, FilterToGLFilter(filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, FilterToGLFilter(filter));

    GLenum glFormat = BPPToGLFormat(bpp);
    glTexImage2D(GL_TEXTURE_2D, 0, glFormat, w,h, 0, glFormat, GL_UNSIGNED_BYTE, data);

    texture->w = CS_CAST(f32, w);
    texture->h = CS_CAST(f32, h);
    texture->wrap = wrap;
    texture->filter = filter;

    return true;
}

static bool LoadTexture(Texture& texture, std::string fileName, Filter filter, Wrap wrap)
{
    const s32 k_bytesPerPixel = 4;
    s32 width,height,bytesPerPixel;
    u8* rawData = stbi_load(fileName.c_str(), &width,&height,&bytesPerPixel,k_bytesPerPixel); // We force all textures to 4-channel RGBA.
    if(!rawData)
        printf("Failed to load texture from file '%s'!\n", fileName.c_str());
    else
    {
        DEFER { stbi_image_free(rawData); };
        return CreateTexture(texture, width,height,k_bytesPerPixel, rawData, filter, wrap);
    }
    return false;
}

static void FreeTexture(Texture& texture)
{
    if(!texture) return;
    glDeleteTextures(1, &texture->handle);
    Deallocate(texture);
}

static f32 GetTextureWidth(Texture& texture)
{
    return texture->w;
}

static f32 GetTextureHeight(Texture& texture)
{
    return texture->h;
}

static void SetTextureFilter(Texture& texture, Filter filter)
{
    texture->filter = filter;
}

static void SetTextureWrap(Texture& texture, Wrap wrap)
{
    texture->wrap = wrap;
}

//
// VertexBuffer
//

static void CreateVertexBuffer(VertexBuffer& buffer)
{
    buffer = Allocate<GET_PTR_TYPE(buffer)>(MEM_SYSTEM);
    if(!buffer) printf("Failed to allocate vertex buffer!\n"); // @Incomplete: Fatal error should terminate!

    glGenVertexArrays(1, &buffer->vao);
    glBindVertexArray(buffer->vao);

    glGenBuffers(1, &buffer->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), CS_CAST(void*, offsetof(Vertex, position)));
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), CS_CAST(void*, offsetof(Vertex, color)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), CS_CAST(void*, offsetof(Vertex, texCoord)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
}

static void FreeVertexBuffer(VertexBuffer& buffer)
{
    if(!buffer) return;
    glDeleteVertexArrays(1, &buffer->vao);
    glDeleteBuffers(1, &buffer->vbo);
    buffer->verts.clear();
    Deallocate(buffer);
}

static void DrawVertexBuffer(VertexBuffer& buffer, DrawMode drawMode)
{
    if(buffer->verts.empty())
        return;

    GLenum primitive;
    switch(drawMode)
    {
        case(DrawMode_Points): primitive = GL_POINTS; break;
        case(DrawMode_LineStrip): primitive = GL_LINE_STRIP; break;
        case(DrawMode_LineLoop): primitive = GL_LINE_LOOP; break;
        case(DrawMode_Lines): primitive = GL_LINES; break;
        case(DrawMode_TriangleStrip): primitive = GL_TRIANGLE_STRIP; break;
        case(DrawMode_TriangleFan): primitive = GL_TRIANGLE_FAN; break;
        case(DrawMode_Triangles): primitive = GL_TRIANGLES; break;
    }

    glBindVertexArray(buffer->vao);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
    GLsizeiptr size = buffer->verts.size() * sizeof(Vertex);
    glBufferData(GL_ARRAY_BUFFER, size, &buffer->verts[0], GL_DYNAMIC_DRAW);
    glDrawArrays(primitive, 0, CS_CAST(GLsizei, buffer->verts.size()));
    glFlush();
}

static void ClearVertexBuffer(VertexBuffer& buffer)
{
    buffer->verts.clear();
}

//
// Framebuffer
//

static void CreateFramebuffer(Framebuffer& framebuffer, s32 w, s32 h, Filter filter, Wrap wrap, nkVec4 clear)
{
    framebuffer = Allocate<GET_PTR_TYPE(framebuffer)>(MEM_SYSTEM);
    ResizeFramebuffer(framebuffer, w, h, filter, wrap);
    // Clear the render target to the desired color.
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->handle);
    Clear(clear);
    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
}

static void FreeFramebuffer(Framebuffer& framebuffer)
{
    if(!framebuffer) return;
    glDeleteFramebuffers(1, &framebuffer->handle);
    FreeTexture(framebuffer->texture);
    Deallocate(framebuffer);
}

static void ResizeFramebuffer(Framebuffer& framebuffer, s32 w, s32 h, Filter filter, Wrap wrap)
{
    if(w <= 0 || h <= 0) return;

    // Delete the old contents (if any).
    glDeleteFramebuffers(1, &framebuffer->handle);
    FreeTexture(framebuffer->texture);

    glGenFramebuffers(1, &framebuffer->handle);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->handle);

    CreateTexture(framebuffer->texture, w,h, 4, NULL, filter, wrap);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer->texture->handle, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf("Failed to complete framebuffer resize!\n");

    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
}

static Texture GetFramebufferTexture(Framebuffer& framebuffer)
{
    return framebuffer->texture;
}

//
// Renderer
//

static void InitGraphics()
{
    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
    printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Vendor: %s\n", glGetString(GL_VENDOR));

    s32 width = GetAppConfig().screenSize.x;
    s32 height = GetAppConfig().screenSize.y;

    s_renderer.screen.scaleMode = ScaleMode_Letterbox;
    s_renderer.screen.filter = Filter_Linear;
    CreateFramebuffer(s_renderer.screen.buffer, width,height, s_renderer.screen.filter);
    imm::CreateContext();
}

static void QuitGraphics()
{
    imm::FreeContext();
    FreeFramebuffer(s_renderer.screen.buffer);
}

static void BeginRenderFrame()
{
    f32 windowWidth = CS_CAST(f32, GetWindowWidth());
    f32 windowHeight = CS_CAST(f32, GetWindowHeight());
    f32 screenWidth = s_renderer.screen.buffer->texture->w;
    f32 screenHeight = s_renderer.screen.buffer->texture->h;

    // Adjust the screen's bounds depending on the current scale mode.
    f32 x,y,w,h;
    switch(s_renderer.screen.scaleMode)
    {
        case(ScaleMode_None):
        {
            x = (windowWidth - screenWidth) * 0.5f;
            y = (windowHeight - screenHeight) * 0.5f;
            w = screenWidth;
            h = screenHeight;
        } break;
        case(ScaleMode_Fit):
        {
            ResizeFramebuffer(s_renderer.screen.buffer, CS_CAST(s32,windowWidth),CS_CAST(s32,windowHeight), s_renderer.screen.filter);
            x = 0.0f;
            y = 0.0f;
            w = windowWidth;
            h = windowHeight;
        } break;
        case(ScaleMode_Stretch):
        {
            x = 0.0f;
            y = 0.0f;
            w = windowWidth;
            h = windowHeight;
        } break;
        case(ScaleMode_Letterbox):
        {
            f32 dstWidth = windowWidth;
            f32 dstHeight = windowHeight;
            f32 sx = windowWidth / screenWidth;
            f32 sy = windowHeight / screenHeight;
            if(abs(sx) < abs(sy)) dstHeight = roundf((screenHeight/screenWidth)*dstWidth);
            if(abs(sx) > abs(sy)) dstWidth = roundf((screenWidth/screenHeight)*dstHeight);
            x = (windowWidth - dstWidth) * 0.5f;
            y = (windowHeight - dstHeight) * 0.5f;
            w = dstWidth;
            h = dstHeight;
        } break;
        case(ScaleMode_Pixel):
        {
            f32 dstWidth = screenWidth;
            f32 dstHeight = screenHeight;
            while(dstWidth+screenWidth <= windowWidth && dstHeight+screenHeight <= windowHeight)
            {
                dstWidth += screenWidth;
                dstHeight += screenHeight;
            }
            x = (windowWidth - dstWidth) * 0.5f;
            y = (windowHeight - dstHeight) * 0.5f;
            w = dstWidth;
            h = dstHeight;
        } break;
        default:
        {
            ASSERT(false, "Unknown scaling mode applied to the screen.");
        } break;
    }

    s_renderer.screen.bounds.x = x;
    s_renderer.screen.bounds.y = y;
    s_renderer.screen.bounds.w = w;
    s_renderer.screen.bounds.h = h;

    // If the filter has changed then update the screen buffer.
    if(s_renderer.screen.filter != s_renderer.screen.buffer->texture->filter)
    {
        ResizeFramebuffer(s_renderer.screen.buffer, CS_CAST(s32,s_renderer.screen.buffer->texture->w),CS_CAST(s32,s_renderer.screen.buffer->texture->h), s_renderer.screen.filter);
    }

    // Update these again as they could have changed (e.g. ScaleMode_Fit).
    screenWidth = s_renderer.screen.buffer->texture->w;
    screenHeight = s_renderer.screen.buffer->texture->h;

    Rect viewport = { 0,0,screenWidth,screenHeight };
    SetRenderTarget(s_renderer.screen.buffer);
    SetViewport(&viewport);
    s_immContext.projectionMatrix = nk::orthographic(0.0f,screenWidth,screenHeight,0.0f,0.0f,1.0f);
}

static void EndRenderFrame()
{
    // @NOTE: This is kind of a hack so that we don't have any transparency in the screen render target when we blit it.
    // Whilst this doesn't matter for blittingh to the actual window it does matter when we draw to ImGui in debug mode
    // as it means the semi-transparent pixels in the target are actually see-through. Whilst this is fine for now we
    // should, in the future, look into improving our render target creation so we can specify the number of components.
    glColorMask(false,false,false,true);
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
    glColorMask(true,true,true,true);

    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);

    SetViewport(NULL);

    s32 srcX0 = 0;
    s32 srcY0 = 0;
    s32 srcX1 = srcX0 + CS_CAST(s32, s_renderer.screen.buffer->texture->w);
    s32 srcY1 = srcY0 + CS_CAST(s32, s_renderer.screen.buffer->texture->h);

    s32 dstX0 = CS_CAST(s32, s_renderer.screen.bounds.x);
    s32 dstY0 = CS_CAST(s32, s_renderer.screen.bounds.y);
    s32 dstX1 = CS_CAST(s32, s_renderer.screen.bounds.w) + dstX0;
    s32 dstY1 = CS_CAST(s32, s_renderer.screen.bounds.h) + dstY0;

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GL_NONE); // Window
    glBindFramebuffer(GL_READ_FRAMEBUFFER, s_renderer.screen.buffer->handle);

    glBlitFramebuffer(srcX0,srcY0,srcX1,srcY1, dstX0,dstY0,dstX1,dstY1, GL_COLOR_BUFFER_BIT, FilterToGLFilter(s_renderer.screen.filter));
}

static void Clear(f32 r, f32 g, f32 b, f32 a)
{
    glClearColor(r,g,b,a);
    glClear(GL_COLOR_BUFFER_BIT);
}

static void Clear(nkVec4 color)
{
    glClearColor(color.r,color.g,color.b,color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

static void SetRenderTarget(Framebuffer target)
{
    s_renderer.boundTarget = target;
    if(!target)
        glBindFramebuffer(GL_FRAMEBUFFER, s_renderer.screen.buffer->handle);
    else
        glBindFramebuffer(GL_FRAMEBUFFER, target->handle);
}

static void SetViewport(Rect* viewport)
{
    GLint x,y;
    GLsizei w,h;

    if(viewport) s_renderer.viewport = *viewport;
    else s_renderer.viewport = { 0,0,CS_CAST(f32,GetRenderTargetWidth()),CS_CAST(f32,GetRenderTargetHeight()) };

    x = CS_CAST(GLint,   s_renderer.viewport.x);
    y = CS_CAST(GLint,   s_renderer.viewport.y);
    w = CS_CAST(GLsizei, s_renderer.viewport.w);
    h = CS_CAST(GLsizei, s_renderer.viewport.h);

    glViewport(x,y,w,h);
}

static void SetScreenScaleMode(ScaleMode scaleMode)
{
    s_renderer.screen.scaleMode = scaleMode;
}

static void SetScreenFilter(Filter filter)
{
    s_renderer.screen.filter = filter;
}

static Framebuffer GetScreen()
{
    return s_renderer.screen.buffer;
}

static u32 GetScreenTextureInternal()
{
    return CS_CAST(u32, s_renderer.screen.buffer->texture->handle);
}

static Rect GetScreenBounds()
{
    return s_renderer.screen.bounds;
}

static f32 GetScreenWidth()
{
    return s_renderer.screen.buffer->texture->w;
}

static f32 GetScreenHeight()
{
    return s_renderer.screen.buffer->texture->h;
}

static ScaleMode GetScreenScaleMode()
{
    return s_renderer.screen.scaleMode;
}

static Filter GetScreenFilter()
{
    return s_renderer.screen.filter;
}

static Framebuffer GetRenderTarget()
{
    return (s_renderer.boundTarget) ? s_renderer.boundTarget : s_renderer.screen.buffer;
}

static Rect GetViewport()
{
    return s_renderer.viewport;
}

static f32 GetRenderTargetWidth()
{
    return (s_renderer.boundTarget) ? s_renderer.boundTarget->texture->w : s_renderer.screen.buffer->texture->w;
}

static f32 GetRenderTargetHeight()
{
    return (s_renderer.boundTarget) ? s_renderer.boundTarget->texture->h : s_renderer.screen.buffer->texture->h;
}

static void UseShader(std::string shaderName)
{
    if(shaderName.empty()) UseShader(NULL);
    else
    {
        Shader shader = *GetAsset<Shader>(shaderName);
        if(shader) UseShader(shader);
    }
}

static void UseShader(Shader shader)
{
    if(!shader) glUseProgram(GL_NONE);
    else glUseProgram(shader->program);
    s_renderer.boundShader = shader;
}

static void UseTexture(std::string textureName, s32 unit)
{
    if(textureName.empty()) UseTexture(NULL, unit);
    else
    {
        Texture texture = *GetAsset<Texture>(textureName);
        if(texture) UseTexture(texture, unit);
    }
}

static void UseTexture(Texture texture, s32 unit)
{
    glActiveTexture(GL_TEXTURE0+unit);

    if(!texture) glBindTexture(GL_TEXTURE_2D, GL_NONE);
    else glBindTexture(GL_TEXTURE_2D, texture->handle);

    if(texture)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     WrapToGLWrap(texture->wrap));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     WrapToGLWrap(texture->wrap));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, FilterToGLFilter(texture->filter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, FilterToGLFilter(texture->filter));
    }

    s_renderer.boundTexture[unit] = texture;
}

// These operate on the currently bound shader.
static void SetShaderBool(std::string name, bool val)
{
    ASSERT(s_renderer.boundShader, "No shader is currently bound!");
    if(!s_renderer.boundShader) return;
    GLint location = glGetUniformLocation(s_renderer.boundShader->program, name.c_str());
    if(location == -1) printf("No shader uniform found: %s\n", name.c_str());
    glUniform1i(location, CS_CAST(s32, val));
}
static void SetShaderInt(std::string name, s32 val)
{
    ASSERT(s_renderer.boundShader, "No shader is currently bound!");
    if(!s_renderer.boundShader) return;
    GLint location = glGetUniformLocation(s_renderer.boundShader->program, name.c_str());
    if(location == -1) printf("No shader uniform found: %s\n", name.c_str());
    glUniform1i(location, val);
}
static void SetShaderFloat(std::string name, f32 val)
{
    ASSERT(s_renderer.boundShader, "No shader is currently bound!");
    if(!s_renderer.boundShader) return;
    GLint location = glGetUniformLocation(s_renderer.boundShader->program, name.c_str());
    if(location == -1) printf("No shader uniform found: %s\n", name.c_str());
    glUniform1f(location, val);
}
static void SetShaderVec2(std::string name, nkVec2 vec)
{
    ASSERT(s_renderer.boundShader, "No shader is currently bound!");
    if(!s_renderer.boundShader) return;
    GLint location = glGetUniformLocation(s_renderer.boundShader->program, name.c_str());
    if(location == -1) printf("No shader uniform found: %s\n", name.c_str());
    glUniform2fv(location, 1, vec.raw);
}
static void SetShaderVec3(std::string name, nkVec3 vec)
{
    ASSERT(s_renderer.boundShader, "No shader is currently bound!");
    if(!s_renderer.boundShader) return;
    GLint location = glGetUniformLocation(s_renderer.boundShader->program, name.c_str());
    if(location == -1) printf("No shader uniform found: %s\n", name.c_str());
    glUniform3fv(location, 1, vec.raw);
}
static void SetShaderVec4(std::string name, nkVec4 vec)
{
    ASSERT(s_renderer.boundShader, "No shader is currently bound!");
    if(!s_renderer.boundShader) return;
    GLint location = glGetUniformLocation(s_renderer.boundShader->program, name.c_str());
    if(location == -1) printf("No shader uniform found: %s\n", name.c_str());
    glUniform4fv(location, 1, vec.raw);
}
static void SetShaderMat2(std::string name, nkMat2 mat)
{
    ASSERT(s_renderer.boundShader, "No shader is currently bound!");
    if(!s_renderer.boundShader) return;
    GLint location = glGetUniformLocation(s_renderer.boundShader->program, name.c_str());
    if(location == -1) printf("No shader uniform found: %s\n", name.c_str());
    glUniformMatrix2fv(location, 1, GL_FALSE, mat.raw);
}
static void SetShaderMat3(std::string name, nkMat3 mat)
{
    ASSERT(s_renderer.boundShader, "No shader is currently bound!");
    if(!s_renderer.boundShader) return;
    GLint location = glGetUniformLocation(s_renderer.boundShader->program, name.c_str());
    if(location == -1) printf("No shader uniform found: %s\n", name.c_str());
    glUniformMatrix3fv(location, 1, GL_FALSE, mat.raw);
}
static void SetShaderMat4(std::string name, nkMat4 mat)
{
    ASSERT(s_renderer.boundShader, "No shader is currently bound!");
    if(!s_renderer.boundShader) return;
    GLint location = glGetUniformLocation(s_renderer.boundShader->program, name.c_str());
    if(location == -1) printf("No shader uniform found: %s\n", name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, mat.raw);
}

//
// Assets
//

bool Asset<Shader>::Load(std::string fileName)
{
    return LoadShader(m_data, fileName);
}
void Asset<Shader>::Free()
{
    FreeShader(m_data);
}
const char* Asset<Shader>::GetPath() const
{
    return "shaders/";
}
const char* Asset<Shader>::GetExt() const
{
    return ".shader";
}
const char* Asset<Shader>::GetType() const
{
    return "Shader";
}

bool Asset<Texture>::Load(std::string fileName)
{
    return LoadTexture(m_data, fileName);
}
void Asset<Texture>::Free()
{
    FreeTexture(m_data);
}
const char* Asset<Texture>::GetPath() const
{
    return "textures/";
}
const char* Asset<Texture>::GetExt() const
{
    return ".png";
}
const char* Asset<Texture>::GetType() const
{
    return "Texture";
}

//
// Imm
//

namespace imm
{
    static void CreateContext()
    {
        f32 w = s_renderer.screen.buffer->texture->w;
        f32 h = s_renderer.screen.buffer->texture->h;

        CreateVertexBuffer(s_immContext.buffer);

        s_immContext.alphaBlending = true;
        s_immContext.textureMapping = false;

        s_immContext.projectionMatrix = nk::orthographic(0.0f,w,h,0.0f,0.0f,1.0f);
        s_immContext.viewMatrix = nk::mat4_identity();
        s_immContext.modelMatrix = nk::mat4_identity();
    }

    static void FreeContext()
    {
        FreeVertexBuffer(s_immContext.buffer);
    }

    static void DrawPoint(f32 x, f32 y, nkVec4 color)
    {
        BeginDraw(DrawMode_Points);
        PutVertex({ {x,y}, color, {0,0} });
        EndDraw();
    }

    static void DrawLine(f32 x1, f32 y1, f32 x2, f32 y2, nkVec4 color)
    {
        BeginDraw(DrawMode_Lines);
        PutVertex({ {x1,y1}, color, {0,0} });
        PutVertex({ {x2,y2}, color, {1,1} });
        EndDraw();
    }

    static void DrawRectOutline(f32 x1, f32 y1, f32 x2, f32 y2, nkVec4 color)
    {
        x1 += 0.5f;
        y1 += 0.5f;

        BeginDraw(DrawMode_LineLoop);
        PutVertex({ {x1,y1}, color, {0,0} });
        PutVertex({ {x2,y1}, color, {1,0} });
        PutVertex({ {x2,y2}, color, {1,1} });
        PutVertex({ {x1,y2}, color, {0,1} });
        EndDraw();
    }

    static void DrawRectFilled(f32 x1, f32 y1, f32 x2, f32 y2, nkVec4 color)
    {
        BeginDraw(DrawMode_TriangleStrip);
        PutVertex({ {x1,y2}, color, {0,1} });
        PutVertex({ {x1,y1}, color, {0,0} });
        PutVertex({ {x2,y2}, color, {1,1} });
        PutVertex({ {x2,y1}, color, {1,0} });
        EndDraw();
    }

    static void DrawCircleOutline(f32 x, f32 y, f32 r, nkVec4 color, s32 segments)
    {
        BeginDraw(DrawMode_LineLoop);
        for(s32 i=0; i<segments; ++i)
        {
            f32 theta = 2 * k_tau32 * CS_CAST(f32,i) / CS_CAST(f32,segments);
            f32 xx = r * cosf(theta);
            f32 yy = r * sinf(theta);
            PutVertex({ {xx+x,yy+y}, color, {0,0} });
        }
        EndDraw();
    }

    static void DrawCircleFilled(f32 x, f32 y, f32 r, nkVec4 color, s32 segments)
    {
        BeginDraw(DrawMode_TriangleFan);
        PutVertex({ {x,y}, color, {0,0} });
        for(s32 i=0; i<=segments; ++i)
        {
            f32 theta = 2 * k_tau32 * CS_CAST(f32,i) / CS_CAST(f32,segments);
            f32 xx = r * cosf(theta);
            f32 yy = r * sinf(theta);
            PutVertex({ {xx+x,yy+y}, color, {0,0} });
        }
        EndDraw();
    }

    static void DrawTexture(std::string textureName, f32 x, f32 y, const Rect* clip, nkVec4 color)
    {
        Texture texture = *GetAsset<Texture>(textureName);
        if(!texture) return;
        DrawTexture(texture, x, y, clip, color);
    }

    static void DrawTexture(std::string textureName, f32 x, f32 y, f32 sx, f32 sy, f32 angle, Flip flip, const nkVec2* anchor, const Rect* clip, nkVec4 color)
    {
        Texture texture = *GetAsset<Texture>(textureName);
        if(!texture) return;
        DrawTexture(texture, x, y, sx, sy, angle, flip, anchor, clip, color);
    }

    static void DrawTexture(Texture& texture, f32 x, f32 y, const Rect* clip, nkVec4 color)
    {
        f32 s1 = 0;
        f32 t1 = 0;
        f32 s2 = texture->w;
        f32 t2 = texture->h;

        if(clip)
        {
            s1 = clip->x;
            t1 = clip->y;
            s2 = s1+clip->w;
            t2 = t1+clip->h;
        }

        f32 x1 = x - ((s2-s1)*0.5f);
        f32 y1 = y - ((t2-t1)*0.5f);
        f32 x2 = x1+(s2-s1);
        f32 y2 = y1+(t2-t1);

        // Normalize the texture coords.
        s1 /= texture->w;
        t1 /= texture->h;
        s2 /= texture->w;
        t2 /= texture->h;

        bool textureMapping = s_immContext.textureMapping;
        s_immContext.textureMapping = true;

        SetCurrentTexture(texture);

        BeginDraw(DrawMode_TriangleStrip);
        PutVertex({ {x1,y2}, color, {s1,t2} });
        PutVertex({ {x1,y1}, color, {s1,t1} });
        PutVertex({ {x2,y2}, color, {s2,t2} });
        PutVertex({ {x2,y1}, color, {s2,t1} });
        EndDraw();

        s_immContext.textureMapping = textureMapping;
    }

    static void DrawTexture(Texture& texture, f32 x, f32 y, f32 sx, f32 sy, f32 angle, Flip flip, const nkVec2* anchor, const Rect* clip, nkVec4 color)
    {
        f32 s1 = 0;
        f32 t1 = 0;
        f32 s2 = texture->w;
        f32 t2 = texture->h;

        if(clip)
        {
            s1 = clip->x;
            t1 = clip->y;
            s2 = s1+clip->w;
            t2 = t1+clip->h;
        }

        f32 ox = x;
        f32 oy = y;

        f32 ax = ((anchor) ? anchor->x : (s2-s1)*0.5f);
        f32 ay = ((anchor) ? anchor->y : (t2-t1)*0.5f);

        x -= ax;
        y -= ay;

        f32 x1 = 0.0f;
        f32 y1 = 0.0f;
        f32 x2 = (s2-s1);
        f32 y2 = (t2-t1);

        // Normalize the texture coords.
        s1 /= texture->w;
        t1 /= texture->h;
        s2 /= texture->w;
        t2 /= texture->h;

        if(CHECK_FLAGS(flip, Flip_Horizontal)) sx = -sx;
        if(CHECK_FLAGS(flip, Flip_Vertical)) sy = -sy;

        nkMat4& modelMatrix = GetModelMatrix();
        nkMat4 cachedMatrix = modelMatrix;

        modelMatrix = nk::mat4_identity();
        modelMatrix = nk::translate(modelMatrix, { ox,oy,0.0f });
        modelMatrix = nk::scale(modelMatrix, { sx,sy,1.0f });
        modelMatrix = nk::rotate(modelMatrix, { 0.0f,0.0f,1.0f }, angle);
        modelMatrix = nk::translate(modelMatrix, { -ox,-oy,0.0f });
        modelMatrix = nk::translate(modelMatrix, { x,y,0.0f });

        bool textureMapping = s_immContext.textureMapping;
        s_immContext.textureMapping = true;

        SetCurrentTexture(texture);

        BeginDraw(DrawMode_TriangleStrip);
        PutVertex({ {x1,y2}, color, {s1,t2} });
        PutVertex({ {x1,y1}, color, {s1,t1} });
        PutVertex({ {x2,y2}, color, {s2,t2} });
        PutVertex({ {x2,y1}, color, {s2,t1} });
        EndDraw();

        s_immContext.textureMapping = textureMapping;
        modelMatrix = cachedMatrix;
    }

    static void DrawFramebuffer(Framebuffer& framebuffer, f32 x, f32 y)
    {
        f32 s1 = 0;
        f32 t1 = 0;
        f32 s2 = framebuffer->texture->w;
        f32 t2 = framebuffer->texture->h;

        f32 x1 = x - ((s2-s1)*0.5f);
        f32 y1 = y - ((t2-t1)*0.5f);
        f32 x2 = x1+(s2-s1);
        f32 y2 = y1+(t2-t1);

        // Normalize the texture coords.
        s1 /= framebuffer->texture->w;
        t1 /= framebuffer->texture->h;
        s2 /= framebuffer->texture->w;
        t2 /= framebuffer->texture->h;

        // Flip the texture.
        std::swap(t1,t2);

        bool textureMapping = s_immContext.textureMapping;
        s_immContext.textureMapping = true;

        SetCurrentTexture(framebuffer->texture);

        BeginDraw(DrawMode_TriangleStrip);
        PutVertex({ { x1,y2 }, { 1,1,1,1 }, { s1,t2 } });
        PutVertex({ { x1,y1 }, { 1,1,1,1 }, { s1,t1 } });
        PutVertex({ { x2,y2 }, { 1,1,1,1 }, { s2,t2 } });
        PutVertex({ { x2,y1 }, { 1,1,1,1 }, { s2,t1 } });
        EndDraw();

        s_immContext.textureMapping = textureMapping;
    }

    static void BeginDraw(DrawMode drawMode)
    {
        ClearVertexBuffer(s_immContext.buffer);
        s_immContext.drawMode = drawMode;

        // Set shader.
        if(!s_immContext.shader) UseShader("imm");
        else UseShader(s_immContext.shader);

        // Set texture.
        for(s32 i=0; i<64; ++i)
            UseTexture(s_immContext.texture[i], i);
    }

    static void EndDraw()
    {
        // Set state.
        if(!s_immContext.alphaBlending)
            glDisable(GL_BLEND);
        else
        {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_BLEND);
        }

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);

        // Set uniforms.
        SetShaderMat4("u_projectionMatrix", s_immContext.projectionMatrix);
        SetShaderMat4("u_viewMatrix", s_immContext.viewMatrix);
        SetShaderMat4("u_modelMatrix", s_immContext.modelMatrix);
        SetShaderBool("u_textureMapping", s_immContext.textureMapping);
        SetShaderInt ("u_texture0", 0);

        // Draw stuff.
        DrawVertexBuffer(s_immContext.buffer, s_immContext.drawMode);
    }

    static void PutVertex(Vertex v)
    {
        s_immContext.buffer->verts.push_back(v);
    }

    static void EnableAlphaBlending(bool enable)
    {
        s_immContext.alphaBlending = enable;
    }

    static void EnableTextureMapping(bool enable)
    {
        s_immContext.textureMapping = enable;
    }

    static bool IsAlphaBlendingEnabled()
    {
        return s_immContext.alphaBlending;
    }

    static bool IsTextureMappingEnabled()
    {
        return s_immContext.textureMapping;
    }

    static void SetCurrentShader(std::string shaderName)
    {
        if(shaderName.empty()) SetCurrentShader(NULL);
        else
        {
            Shader shader = *GetAsset<Shader>(shaderName);
            if(!shader) return;
            SetCurrentShader(shader);
        }
    }

    static void SetCurrentShader(Shader shader)
    {
        s_immContext.shader = shader;
    }

    static void SetCurrentTexture(std::string textureName, s32 unit)
    {
        if(textureName.empty()) SetCurrentTexture(NULL);
        else
        {
            Texture texture = *GetAsset<Texture>(textureName);
            if(!texture) return;
            SetCurrentTexture(texture, unit);
        }
    }

    static void SetCurrentTexture(Texture texture, s32 unit)
    {
        s_immContext.texture[unit] = texture;
    }

    static nkMat4& GetProjectionMatrix()
    {
        return s_immContext.projectionMatrix;
    }

    static nkMat4& GetViewMatrix()
    {
        return s_immContext.viewMatrix;
    }

    static nkMat4& GetModelMatrix()
    {
        return s_immContext.modelMatrix;
    }
}
